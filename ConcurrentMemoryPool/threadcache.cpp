#include "threadcache.h"
// thread cache ��֤�����ж��̷߳���ͬһ��thread cache
#include "CentralCache.h"


//�����Ļ����ȡ�ڴ����
void* ThreadCache::FetchFromCentralCache(size_t index, size_t size)
{
	//���ܴ�central cache��ȡ���ٸ��������ٷ���һ��
	//��ȡһ�����󣬻�ȡ������ʹ����������ʽ
	size_t batchNum = min(SizeClass::NumMoveSize(size), _freeList[index].MAX_SIZE());

	//ȥ���Ļ����ȡ batch_num ���ڴ����
	void* start = nullptr;
	void* end   = nullptr;
	size_t actualNum = CentralCache::GetInstance()->FetchRangeObj(start,end,batchNum,SizeClass::Roundup(size));
	assert(actualNum > 0);

	//�������1������һ�����������������������
	//�����´ξͲ���Ҫ��ȥ�����Ļ���
	//���ٶ������ľ��������Ч��
	if (actualNum > 1)
	{
		                          //NextObj��start����start����һ��
		_freeList[index].PushRange(NextObj(start), end,actualNum -1);
	}

	if (_freeList[index].MAX_SIZE() == batchNum)
	{
		_freeList[index].SetMaxSize(_freeList[index].MAX_SIZE() + 1);
	}


	//start �������ĵ�һ�������׵�ַ
	return start; 
}

void* ThreadCache::Allocate(size_t size)
{
	size_t i = SizeClass::Index(size);
	if (!_freeList[i].Empty()) //����пռ䣬�ͷ����ڴ����
	{
		return _freeList[i].Pop();
	}
	else
	{
		//...���û���ڴ�ռ䣬�ʹӴ����(central cache)�г���һ��

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

    //�����list��pop�����Ļ���centralcache
	CentralCache::GetInstance()->ReleaseListToSpans(start, size);
}