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

	// ��ϵͳ����kҳ�ڴ�ҵ���������
	void* SystemAllocPage(size_t k);


	Span* Newspan(size_t k);


	// ��ȡ�Ӷ���span��ӳ��
	Span* MapObjectToSpan(void* obj);


	// �ͷſ���span�ص�Pagecache�����ϲ����ڵ�span
	void ReleaseSpanToPageCahce(Span* span);
private:
	SpanList _spanList[NPAGES]; //��ҳ��ӳ��


	//std::mutex _map_mtx;
	//std::unordered_map<PageId,Span*> _idSpanMap;   
	//tcmalloc����û��ʹ��map��ʹ�õ��ǻ����� �˽�һ�£�Ч�ʸ���


	TCMalloc_PageMap2<32 - PAGE_SHIFT> _idSpanMap;
	//tcmalloc ������ Ч�ʸ���	

	std::recursive_mutex _mtx;
private:
	PageCache()
	{}
	PageCache(const PageCache&) = delete;
	//����ģʽ
	static PageCache pageCache;
		 
};

