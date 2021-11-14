#include "CentralCache.h"
#include "PageCache.h"
CentralCache CentralCache::centralCache;

Span* CentralCache::GetOneSpan(SpanList& list, size_t size)
{
	//先去SpanList中去找还有内存的span
	Span* it = list.Begin();
	while (it !=list.end())
	{ 
		if (it->_List)
		{
			return it;
		}




		it = it->_next;
	}
	//走到这里代表所有的span都没有内存了，这时只能找pagecache
	Span* span = PageCache::GetInstance()->Newspan(SizeClass::NumMovePage(size));

	//这里的span是从Pagecache中得来的，还没有切分好
	//要切割好然后挂在list中
	char* start = (char*)(span->_Page_Id << PAGE_SHIFT);
	char* end = start + (span->_n << PAGE_SHIFT);
	
	while (start < end)
	{
		//切对象头插
		char* next = start + size;
		NextObj(start) = span->_List;
		span->_List = start;
		start = next;

	    //如果切割8字节，切的时候如果最后小于8字节，只能舍弃
	}
	span->_objsize = size;
	list.PushFront(span);
	return span;
}

size_t CentralCache::FetchRangeObj(void*& start, void*& end, size_t n, size_t size)
{
	size_t index = SizeClass::Index(size); 
	//_spanList[index].lock();
	//SpanList& splist = _spanList[i];
	
	std::lock_guard<std::mutex> lock(_spanList[index]._mtx);

	//获取一个新 span
	Span* span = GetOneSpan(_spanList[index],size);

	//走到这里要把span给切出来，它需要n个大小为size的对象
	//情况1：这个span中有比n多或者等于n个的内存对象,这时候是够用的
	//情况2：这个n大于span中内存对象的个数，这个时候有多少给多少

	size_t i = 1;
	start = span->_List;
	void* cur = start;
	void* prev = start;
	while (i <= n && cur != nullptr)
	{
		prev = cur;
		cur = NextObj(cur);
		++i;
		//每取走一个 _use_count++
		span->_use_count++;
	}

	span->_List = cur;
	end = prev;
	NextObj(prev) = nullptr;

	//_spanList[index].unlock();
	
	return i - 1;

}


// 将一定数量的对象释放到span跨度
void CentralCache::ReleaseListToSpans(void* start, size_t size)
{
	//加锁
	size_t index = SizeClass::Index(size);
	std::lock_guard<std::mutex> lock(_spanList[index]._mtx);

	
	
	
	while (start != nullptr)
	{
		void* next = NextObj(start);
		//找start内存块属于哪个span
		Span* span = PageCache::GetInstance()->MapObjectToSpan(start);

		//然后把对象头插到对应的span的list上
		NextObj(start) = span->_List;
		span->_List = start;
		//每还回来一个对象 _use_count--;
		span->_use_count--;
		//如果span->_use_count == 0，说明这个span切出去的大块内存全还回来了
		if (span->_use_count == 0)
		{
			_spanList[index].Erase(span);
			span->_List = nullptr;

			PageCache::GetInstance()->ReleaseSpanToPageCahce(span);
		}


		start = next;
	}
}