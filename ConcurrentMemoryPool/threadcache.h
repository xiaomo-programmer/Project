#pragma once

#include "Common.h"

class ThreadCache  //һ��ӳ��freeList�Ĺ�ϣͰ
{
public:
	// �����ڴ����
	void* Allocate(size_t size);

	//�ͷ��ڴ����
	void Deallocate(void* ptr,size_t size);

	//�����Ļ����л�ȡ����
	void* FetchFromCentralCache(size_t index, size_t size);

	//�ͷŶ��󣬵������������ʱ�������ڴ�������Ļ���
	void ListTooLong(FreeList& _List, size_t size);

private:
	FreeList _freeList[NFREELISTS];  //[]����Ĵ�Сȡ���������������С
};

//map<int, ThreadCache> idchar; //����̷߳���idchar��Ҫ����
//TLS  thread local storage ��ÿ���߳̿�����һ�������Լ���ȫ�ֵĿռ�
static __declspec(thread) ThreadCache* tls_threadcache = nullptr;//ʹ�þ�̬������ȱ�ݵ�,����ֻ����threadcache��һ����ַ