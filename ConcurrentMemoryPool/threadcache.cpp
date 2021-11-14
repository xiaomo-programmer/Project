#include "threadcache.h"
// thread cache 保证不会有多线程访问同一个thread cache
#include "CentralCache.h"


//从中心缓存获取内存对象
void* ThreadCache::FetchFromCentralCache(size_t index, size_t size)
{
	//不管从central cache获取多少个对象，至少返回一个
	//获取一批对象，获取的数量使用慢启动方式
	size_t batchNum = min(SizeClass::NumMoveSize(size), _freeList[index].MAX_SIZE());

	//去中心缓存获取 batch_num 个内存对象
	void* start = nullptr;
	void* end   = nullptr;
	size_t actualNum = CentralCache::GetInstance()->FetchRangeObj(start,end,batchNum,SizeClass::Roundup(size));
	assert(actualNum > 0);

	//如果大于1，返回一个，其余的链到自由链表上
	//这样下次就不需要再去找中心缓存
	//减少对于锁的竞争，提高效率
	if (actualNum > 1)
	{
		                          //NextObj（start）是start的下一个
		_freeList[index].PushRange(NextObj(start), end,actualNum -1);
	}

	if (_freeList[index].MAX_SIZE() == batchNum)
	{
		_freeList[index].SetMaxSize(_freeList[index].MAX_SIZE() + 1);
	}


	//start 多个对象的第一个对象首地址
	return start; 
}

void* ThreadCache::Allocate(size_t size)
{
	size_t i = SizeClass::Index(size);
	if (!_freeList[i].Empty()) //如果有空间，就返回内存对象
	{
		return _freeList[i].Pop();
	}
	else
	{
		//...如果没有内存空间，就从大对象(central cache)切出来一块

		return FetchFromCentralCache(i, size);
	}
}


void ThreadCache::Deallocate(void* ptr,size_t size)
{
	size_t i = SizeClass::Index(size);
	
	_freeList[i].Push(ptr);

	//if(_freelist too long) release into (central cache) 
	if (_freeList[i].Size() >= _freeList[i].MAX_SIZE())
	{
		ListTooLong(_freeList[i], size);
	}
}




void ThreadCache::ListTooLong(FreeList& list, size_t size)
{

	size_t batchNum = list.MAX_SIZE();
	void* start = nullptr;
	void* end = nullptr;
	list.PopRange(start, end, batchNum);

    //这里把list中pop出来的还给centralcache
	CentralCache::GetInstance()->ReleaseListToSpans(start, size);
}