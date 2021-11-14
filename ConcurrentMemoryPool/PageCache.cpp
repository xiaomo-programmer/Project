#include "PageCache.h"
PageCache PageCache::pageCache;
// ��ϵͳ����kҳ�ڴ�
void* PageCache::SystemAllocPage(size_t k)
{
	//����ϵͳ�Ľӿ� VirtualAlloc
	return  ::SystemAlloc(k);
}


Span* PageCache::Newspan(size_t k)
{
	//����
	std::lock_guard<std::recursive_mutex> lock(_mtx);



	//����Ҫ��ҳ��span����pagecache��û��4page��span��������ҳ��span
	//����ҳ��span�и��һ����ҳ��span����һ��һҳ��span
	//��ҳ����ȥ���ϲ�ȥ�ã�һҳ�����ӵ�1page��span��

	//����������128ҳ����������ֱ���������NPAGES�Ĵ���ڴ棬ֱ����ϵͳҪ
	if (k >= NPAGES)
	{
		void* ptr = SystemAllocPage(k);
		Span* span = new Span;
		span->_Page_Id = (ADDRES_INT)ptr >> PAGE_SHIFT;
		span->_n = k;
		//����Ĵ�С
		//span->_objsize = k << PAGE_SHIFT;

		{
			//std::lock_guard<std::mutex> lock(_map_mtx);

			_idSpanMap[span->_Page_Id] = span;
		}
		return span;
	}


	//�ȿ�һ��_spanList[k] ������û��span
	if (!_spanList[k].Empty())
	{
		/*Span* it = _spanList[k].Begin();
		_spanList[k].Erase(it);
		return it;*/

		Span* it = _spanList[k].PopFront();
		return it;
	}
	//�ߵ�����˵��_spanList[k] û��span
	for (size_t i=k+1; i < NPAGES; i++)
	{
		//����Ѵ�ҳ��С ,�г� kҳ��span���أ����г�i-k��ҳ�ҵ���Ӧҳ��
		if (!_spanList[i].Empty())
		{

			////ͷ��
			//Span* span = _spanList[i].Begin();
			//_spanList->Erase(span);


			//Span* splitspan = new Span;
			//splitspan->_Page_Id = span->_Page_Id + k;
			//splitspan->_n = span->_n - k;

			//span->_n = k;

			//_spanList[splitspan->_n].Insert(_spanList[splitspan->_n].Begin(), splitspan);

			//return span;

			//β�г�һ��Kҳ��span
			Span* span = _spanList[i].PopFront();
		

			Span* split = new Span;
			split->_Page_Id = span->_Page_Id + span->_n - k;
			split->_n = k; //�г�ȥ��split����ҳ��

			{
				//std::lock_guard<std::mutex> lock(_map_mtx);

				//������иı��г�����ҳ�ź�span��ӳ���ϵ
				for (PageId i = 0; i < k; ++i)
				{
					PageId id = split->_Page_Id + i;
					_idSpanMap[id] = split;
				}
			}
			span->_n = span->_n - k;//ʣ�µ�span����ҳ��

			_spanList[span->_n].PushFront(span);//���кõ�kҳ�ҵ�_spanList[k]��ȥ

			return split;
		}
	}

	Span* bigSpan = new Span;
	void* memory = SystemAllocPage(NPAGES-1);
	bigSpan->_Page_Id = (size_t)(memory) >> 12;
	bigSpan->_n = NPAGES - 1;

	{
		//std::lock_guard<std::mutex> lock(_map_mtx);

		//����pageid��span��ӳ���ϵ
		for (PageId i = 0; i < bigSpan->_n; i++)
		{
			//��ǰҳ��ҳ�ž�����ʼҳ+i
			PageId id = bigSpan->_Page_Id + i;
			_idSpanMap[id] = bigSpan;
		}
	}

	//����ΪʲôҪ��һ��128�ģ� ��С�ỹ�����ϲ��ɴ��ڴ棬��С���ʵ��ڴ���Ƭ���⡣
	_spanList[NPAGES - 1].Insert(_spanList[NPAGES - 1].Begin(), bigSpan);


	return Newspan(k);
}



Span* PageCache::MapObjectToSpan(void* obj)
{
	//std::lock_guard<std::mutex> lock(_map_mtx);




	PageId id = (ADDRES_INT)obj >> PAGE_SHIFT;
	//auto ret = _idSpanMap.find(id);
	//if (ret != _idSpanMap.end())
	//{
	//	return ret->second;
	//}
	//else
	//{
	//	//û���ҵ���һ���������ط���������
	//	assert(false);
	//	return nullptr;
	//}

	Span* span = _idSpanMap.get(id);
	if (span != nullptr)
	{
		return span;
	}
	else
	{
		assert(false);
		return nullptr;
	}

	
}



void PageCache::ReleaseSpanToPageCahce(Span* span)
{
	


	//���ﻻ�ڴ�Ļ�������Ǵ���NPAGES�Ĵ���ڴ棬
	if (span->_n >= NPAGES)
	{
		{
			//std::lock_guard<std::mutex> lock(_map_mtx);


			//_idSpanMap.erase(span->_Page_Id);

			_idSpanMap.erase(span->_Page_Id);

		}
		//�ͷ��ڴ棬Ҫ������ͷ���ʼ��ַ
		void* ptr = (void*)(span->_Page_Id << PAGE_SHIFT);
		SystemFree(ptr);
		delete span;
		return;
	}

	//����
	std::lock_guard<std::recursive_mutex> lock(_mtx);

	//���ǰ�����span��ҳ�����кϲ�������ڴ���Ƭ������

	//����ǰ�ϲ�
	while (1)
	{


		PageId preId = span->_Page_Id - 1;
		//auto ret = _idSpanMap.find(preId);

		//if (ret == _idSpanMap.end()) //˵������ط�ǰ��û��span
		//{
		//	break;
		//}

		Span* preSpan = _idSpanMap.get(preId);
		if (preSpan == nullptr)
		{
			break;
		}



		////������ҵ���
		//Span* preSpan = ret->second;


		

		if (preSpan->_use_count != 0)//����˵ǰһ��span���ڣ����ǲ������У���ʹ���оͽ�����ǰ�ϲ�
		{
			break;
		}

		//�ߵ�������ζ��ǰһ��span���ڣ������ǿ��еģ���ʼ�ϲ�

		//�������ҳ����������128���Ͳ����кϲ�
		if (preSpan->_n + span->_n >= NPAGES)
		{
			break;
		}



		//���ڴӶ���Ӧ��_spanList�е�span��List�н�����
		_spanList[preSpan->_n].Erase(preSpan);



		//����ϲ���ǰһ��ҳ�ϲ�����һ��ҳ��֮��ɾ����ǰһ��ҳ
		span->_Page_Id = preSpan->_Page_Id;
		span->_n = span->_n + preSpan->_n;

		{
			//std::lock_guard<std::mutex> lock(_map_mtx);

			//����Ҫ����ҳ�ź�span֮���ӳ���ϵ
			for (PageId i = 0; i < preSpan->_n; i++)
			{
				_idSpanMap[preSpan->_Page_Id + i] = span;
			}
		}
		delete preSpan;
	}


	//���ϲ�
	while (1)
	{
		PageId preId1 = span->_Page_Id +span->_n;
		//auto ret1 = _idSpanMap.find(preId1);
		//if (ret1 == _idSpanMap.end())
		//{
		//	break;
		//}
		//Span* curSpan = ret1->second;

		Span* curSpan = _idSpanMap.get(preId1);
		if (curSpan == nullptr)
		{
			break;
		}
		if (curSpan->_use_count != 0)
		{
			break;
		}

		//����128ҳ���Ͳ���Ҫ���кϲ���
		if (curSpan->_n + span->_n >= NPAGES)
		{
			break;
		}


		_spanList[curSpan->_n].Erase(curSpan);

		span->_n = span->_n + curSpan->_n;
		{
			//std::lock_guard<std::mutex> lock(_map_mtx);

			for (PageId i = 0; i < curSpan->_n; i++)
			{
				_idSpanMap[curSpan->_Page_Id + i] = span;
			}
		}
		delete curSpan;
	}
		
	//ǰ��ϲ������span֮�󣬲��뵽��Ӧ��������ȥ
	_spanList[span->_n].PushFront(span);
	
}