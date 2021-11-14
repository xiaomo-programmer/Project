#pragma once
#include "Common.h"
class CentralCache
{
public:
	static CentralCache* GetInstance()
	{
		return &centralCache;
	}
	// 从中心缓存获取一定数量的对象给线程缓存 ,此函数有一个返回值，它告诉了我们到底是centralcache给了几个对象
	size_t FetchRangeObj(void*& start, void*& end, size_t n, size_t size);


	// 从SpanList或者Page cache获取一个span
	Span* GetOneSpan(SpanList& list, size_t size);


	// 将一定数量的对象释放到span跨度
	void ReleaseListToSpans(void* start, size_t size);


private:
	SpanList _spanList[NFREELISTS];  //按对齐方式映射

	//在中心缓存中，当thread同时访问同一个字节的spanlist时，要进行加锁
	
private:
	CentralCache()
	{}

	CentralCache(const CentralCache&) = delete;
	static CentralCache centralCache; //定义一个全局的中心缓存对象
};
