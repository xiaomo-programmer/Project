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
			//����MAX_SIZEȥ��pagecache


			//���ߴ���128ҳ��ֻ��ȥ��ϵͳȥҪ�ڴ����
			size_t npage = SizeClass::Roundup(size) >> PAGE_SHIFT; //����size = 65KB��Roundup��ʹsize���68KB���ٳ�4KB�õ�ҳ��
			//�����ȡ������span��ָ�루��ʼ��ַ��
			Span* span = PageCache::GetInstance()->Newspan(npage);

			span->_objsize = size;
			void* ptr = (void*)((span->_Page_Id) << PAGE_SHIFT);
			return ptr;


		}
		else
		{   //С��MAX_SIZEȥ�ҹ�ϣӳ�����������Ķ���Ͱ
			if (tls_threadcache == nullptr)
			{
				tls_threadcache = new ThreadCache;  //thread�Ǵ��ڶ��ϵģ�ֻ�Ǵ���tls��һ��ָ��
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
			//pagecache���ͷ�
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
