#pragma once

#include "Common.h"

class ThreadCache  //一个映射freeList的哈希桶
{
public:
	// 申请内存对象
	void* Allocate(size_t size);

	//释放内存对象
	void Deallocate(void* ptr,size_t size);

	//从中心缓存中获取对象
	void* FetchFromCentralCache(size_t index, size_t size);

	//释放对象，当自由链表过长时，回收内存对象到中心缓存
	void ListTooLong(FreeList& _List, size_t size);

private:
	FreeList _freeList[NFREELISTS];  //[]里面的大小取决于想解决的问题大小
};

//map<int, ThreadCache> idchar; //多个线程访问idchar需要加锁
//TLS  thread local storage 给每个线程开辟了一个属于自己的全局的空间
static __declspec(thread) ThreadCache* tls_threadcache = nullptr;//使用静态的是有缺陷的,这里只存了threadcache的一个地址