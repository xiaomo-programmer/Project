#pragma once
#include "Common.h"
#include "PageMap.h"
class PageCache
{

public:
	static PageCache* GetInstance()
	{
		return &pageCache;
	}

	// 向系统申请k页内存挂到自由链表
	void* SystemAllocPage(size_t k);


	Span* Newspan(size_t k);


	// 获取从对象到span的映射
	Span* MapObjectToSpan(void* obj);


	// 释放空闲span回到Pagecache，并合并相邻的span
	void ReleaseSpanToPageCahce(Span* span);
private:
	SpanList _spanList[NPAGES]; //按页数映射


	//std::mutex _map_mtx;
	//std::unordered_map<PageId,Span*> _idSpanMap;   
	//tcmalloc里面没有使用map，使用的是基数树 了解一下，效率更高


	TCMalloc_PageMap2<32 - PAGE_SHIFT> _idSpanMap;
	//tcmalloc 基数数 效率更高	

	std::recursive_mutex _mtx;
private:
	PageCache()
	{}
	PageCache(const PageCache&) = delete;
	//单例模式
	static PageCache pageCache;
		 
};

