#include "CentralCache.h"
#include "PageCache.h"
CentralCache CentralCache::centralCache;

Span* CentralCache::GetOneSpan(SpanList& list, size_t size)
{
	//��ȥSpanList��ȥ�һ����ڴ��span
	Span* it = list.Begin();
	while (it !=list.end())
	{ 
		if (it->_List)
		{
			return it;
		}




		it = it->_next;
	}
	//�ߵ�����������е�span��û���ڴ��ˣ���ʱֻ����pagecache
	Span* span = PageCache::GetInstance()->Newspan(SizeClass::NumMovePage(size));

	//�����span�Ǵ�Pagecache�е����ģ���û���зֺ�
	//Ҫ�и��Ȼ�����list��
	char* start = (char*)(span->_Page_Id << PAGE_SHIFT);
	char* end = start + (span->_n << PAGE_SHIFT);
	
	while (start < end)
	{
		//�ж���ͷ��
		char* next = start + size;
		NextObj(start) = span->_List;
		span->_List = start;
		start = next;

	    //����и�8�ֽڣ��е�ʱ��������С��8�ֽڣ�ֻ������
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

	//��ȡһ���� span
	Span* span = GetOneSpan(_spanList[index],size);

	//�ߵ�����Ҫ��span���г���������Ҫn����СΪsize�Ķ���
	//���1�����span���б�n����ߵ���n�����ڴ����,��ʱ���ǹ��õ�
	//���2�����n����span���ڴ����ĸ��������ʱ���ж��ٸ�����

	size_t i = 1;
	start = span->_List;
	void* cur = start;
	void* prev = start;
	while (i <= n && cur != nullptr)
	{
		prev = cur;
		cur = NextObj(cur);
		++i;
		//ÿȡ��һ�� _use_count++
		span->_use_count++;
	}

	span->_List = cur;
	end = prev;
	NextObj(prev) = nullptr;

	//_spanList[index].unlock();
	
	return i - 1;

}


// ��һ�������Ķ����ͷŵ�span���
void CentralCache::ReleaseListToSpans(void* start, size_t size)
{
	//����
	size_t index = SizeClass::Index(size);
	std::lock_guard<std::mutex> lock(_spanList[index]._mtx);

	
	
	
	while (start != nullptr)
	{
		void* next = NextObj(start);
		//��start�ڴ�������ĸ�span
		Span* span = PageCache::GetInstance()->MapObjectToSpan(start);

		//Ȼ��Ѷ���ͷ�嵽��Ӧ��span��list��
		NextObj(start) = span->_List;
		span->_List = start;
		//ÿ������һ������ _use_count--;
		span->_use_count--;
		//���span->_use_count == 0��˵�����span�г�ȥ�Ĵ���ڴ�ȫ��������
		if (span->_use_count == 0)
		{
			_spanList[index].Erase(span);
			span->_List = nullptr;

			PageCache::GetInstance()->ReleaseSpanToPageCahce(span);
		}


		start = next;
	}
}