#pragma once

#include "Common.h"
#include "threadcache.h"
#include "PageCache.h"
// void* tcmalloc(size_t size) google



static void* ConcurrentAlloc(size_t size)
{
	try
	{
		if (size > MAX_BYTES)
		{
			//大于MAX_SIZE去找pagecache


			//再者大于128页了只能去找系统去要内存对象
			size_t npage = SizeClass::Roundup(size) >> PAGE_SHIFT; //假如size = 65KB，Roundup会使size变成68KB，再除4KB得到页数
			//这里获取到的是span的指针（起始地址）
			Span* span = PageCache::GetInstance()->Newspan(npage);

			span->_objsize = size;
			void* ptr = (void*)((span->_Page_Id) << PAGE_SHIFT);
			return ptr;


		}
		else
		{   //小于MAX_SIZE去找哈希映射的自由链表的对象桶
			if (tls_threadcache == nullptr)
			{
				tls_threadcache = new ThreadCache;  //thread是存在堆上的，只是存了tls的一个指针
			}
			return tls_threadcache->Allocate(size);
		}
	}
	catch (const std::exception& e)
	{
		cout << e.what() << endl;
	}
	return nullptr;

}

static void ConcurrentFree(void* ptr)
{

	
	try
	{
		Span* span = PageCache::GetInstance()->MapObjectToSpan(ptr);
		size_t size = span->_objsize;

		if (size > MAX_BYTES)

		{
			//pagecache的释放
			//Span* span = PageCache::GetInstance()->MapObjectToSpan(ptr);
			PageCache::GetInstance()->ReleaseSpanToPageCahce(span);

		}
		else
		{
			assert(tls_threadcache);
			tls_threadcache->Deallocate(ptr, size);

		}
	}
	catch (const std::exception& e)
	{
		cout << e.what() << endl;
	}
}
