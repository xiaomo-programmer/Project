#include "PageCache.h"
PageCache PageCache::pageCache;
// 向系统申请k页内存
void* PageCache::SystemAllocPage(size_t k)
{
	//调用系统的接口 VirtualAlloc
	return  ::SystemAlloc(k);
}


Span* PageCache::Newspan(size_t k)
{
	//加锁
	std::lock_guard<std::recursive_mutex> lock(_mtx);



	//当需要四页的span，但pagecache中没有4page的span，但有五页的span
	//把五页的span切割成一个四页的span，和一个一页的span
	//四页的拿去给上层去用，一页的链接到1page的span中

	//如果这里大于128页数，解决针对直接申请大于NPAGES的大块内存，直接找系统要
	if (k >= NPAGES)
	{
		void* ptr = SystemAllocPage(k);
		Span* span = new Span;
		span->_Page_Id = (ADDRES_INT)ptr >> PAGE_SHIFT;
		span->_n = k;
		//对象的大小
		//span->_objsize = k << PAGE_SHIFT;

		{
			//std::lock_guard<std::mutex> lock(_map_mtx);

			_idSpanMap[span->_Page_Id] = span;
		}
		return span;
	}


	//先看一下_spanList[k] 这里有没有span
	if (!_spanList[k].Empty())
	{
		/*Span* it = _spanList[k].Begin();
		_spanList[k].Erase(it);
		return it;*/

		Span* it = _spanList[k].PopFront();
		return it;
	}
	//走到这里说明_spanList[k] 没有span
	for (size_t i=k+1; i < NPAGES; i++)
	{
		//在这把大页切小 ,切成 k页的span返回，再切出i-k的页挂到对应页上
		if (!_spanList[i].Empty())
		{

			////头切
			//Span* span = _spanList[i].Begin();
			//_spanList->Erase(span);


			//Span* splitspan = new Span;
			//splitspan->_Page_Id = span->_Page_Id + k;
			//splitspan->_n = span->_n - k;

			//span->_n = k;

			//_spanList[splitspan->_n].Insert(_spanList[splitspan->_n].Begin(), splitspan);

			//return span;

			//尾切出一个K页的span
			Span* span = _spanList[i].PopFront();
		

			Span* split = new Span;
			split->_Page_Id = span->_Page_Id + span->_n - k;
			split->_n = k; //切出去的split的总页数

			{
				//std::lock_guard<std::mutex> lock(_map_mtx);

				//下面进行改变切出来的页号和span的映射关系
				for (PageId i = 0; i < k; ++i)
				{
					PageId id = split->_Page_Id + i;
					_idSpanMap[id] = split;
				}
			}
			span->_n = span->_n - k;//剩下的span的总页数

			_spanList[span->_n].PushFront(span);//把切好的k页挂到_spanList[k]上去

			return split;
		}
	}

	Span* bigSpan = new Span;
	void* memory = SystemAllocPage(NPAGES-1);
	bigSpan->_Page_Id = (size_t)(memory) >> 12;
	bigSpan->_n = NPAGES - 1;

	{
		//std::lock_guard<std::mutex> lock(_map_mtx);

		//建立pageid和span的映射关系
		for (PageId i = 0; i < bigSpan->_n; i++)
		{
			//当前页的页号就是起始页+i
			PageId id = bigSpan->_Page_Id + i;
			_idSpanMap[id] = bigSpan;
		}
	}

	//这里为什么要切一块128的？ 切小会还回来合并成大内存，很小概率的内存碎片问题。
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
	//	//没有找到，一定是其他地方出问题了
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
	


	//这里换内存的话，如果是大于NPAGES的大块内存，
	if (span->_n >= NPAGES)
	{
		{
			//std::lock_guard<std::mutex> lock(_map_mtx);


			//_idSpanMap.erase(span->_Page_Id);

			_idSpanMap.erase(span->_Page_Id);

		}
		//释放内存，要算这个释放起始地址
		void* ptr = (void*)(span->_Page_Id << PAGE_SHIFT);
		SystemFree(ptr);
		delete span;
		return;
	}

	//加锁
	std::lock_guard<std::recursive_mutex> lock(_mtx);

	//检查前后空闲span的页，进行合并，解决内存碎片的问题

	//先向前合并
	while (1)
	{


		PageId preId = span->_Page_Id - 1;
		//auto ret = _idSpanMap.find(preId);

		//if (ret == _idSpanMap.end()) //说明这个地方前面没有span
		//{
		//	break;
		//}

		Span* preSpan = _idSpanMap.get(preId);
		if (preSpan == nullptr)
		{
			break;
		}



		////这里就找到了
		//Span* preSpan = ret->second;


		

		if (preSpan->_use_count != 0)//这里说前一个span存在，但是并不空闲，在使用中就结束向前合并
		{
			break;
		}

		//走到这里意味着前一个span存在，并且是空闲的，开始合并

		//如果两个页加起来大于128，就不进行合并
		if (preSpan->_n + span->_n >= NPAGES)
		{
			break;
		}



		//现在从对相应的_spanList中的span的List中解下来
		_spanList[preSpan->_n].Erase(preSpan);



		//这里合并把前一个页合并到后一个页，之后删除掉前一个页
		span->_Page_Id = preSpan->_Page_Id;
		span->_n = span->_n + preSpan->_n;

		{
			//std::lock_guard<std::mutex> lock(_map_mtx);

			//这里要更新页号和span之间的映射关系
			for (PageId i = 0; i < preSpan->_n; i++)
			{
				_idSpanMap[preSpan->_Page_Id + i] = span;
			}
		}
		delete preSpan;
	}


	//向后合并
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

		//超过128页，就不需要进行合并了
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
		
	//前后合并出大的span之后，插入到对应的链表当中去
	_spanList[span->_n].PushFront(span);
	
}