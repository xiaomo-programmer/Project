#pragma once
#include "Common.h"
class CentralCache
{
public:
	static CentralCache* GetInstance()
	{
		return &centralCache;
	}
	// �����Ļ����ȡһ�������Ķ�����̻߳��� ,�˺�����һ������ֵ�������������ǵ�����centralcache���˼�������
	size_t FetchRangeObj(void*& start, void*& end, size_t n, size_t size);


	// ��SpanList����Page cache��ȡһ��span
	Span* GetOneSpan(SpanList& list, size_t size);


	// ��һ�������Ķ����ͷŵ�span���
	void ReleaseListToSpans(void* start, size_t size);


private:
	SpanList _spanList[NFREELISTS];  //�����뷽ʽӳ��

	//�����Ļ����У���threadͬʱ����ͬһ���ֽڵ�spanlistʱ��Ҫ���м���
	
private:
	CentralCache()
	{}

	CentralCache(const CentralCache&) = delete;
	static CentralCache centralCache; //����һ��ȫ�ֵ����Ļ������
};
