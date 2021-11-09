#include <iostream>
#include <memory>

using namespace std;
//
//class HeapOnly
//{
//public:
//	static HeapOnly* CreateObject()
//	{
//		return new HeapOnly;
//	}
//private:
//	HeapOnly() {}
//
//	// C++98
//	// 1.ֻ����,��ʵ�֡���Ϊʵ�ֿ��ܻ���鷳�����㱾����Ҫ
//	// 2.������˽��
//	//HeapOnly(const HeapOnly&);
//
//		// or
//
//		// C++11 
//		HeapOnly(const HeapOnly&) = delete;
//};
//ֻ����ջ�Ͽ��ٵĶ����� 
//�Ƽ���� 
//class StackOnly
//{
//public:
//	static StackOnly CreateObject()
//	{
//		return StackOnly();
//	}
//private:
//	StackOnly() {}
//};

//����new
//ȱ�ݣ���ֹ���˾�̬���Ķ���  ȫ�ֶ���;�̬����
class StackOnly
{
public:
	StackOnly() {}
//private:
	void* operator new(size_t size) =  delete;
	void operator delete(void* p) = delete;
};

int main()
{
	//HeapOnly::CreateObject();//�ڶ���
	// StackOnly st = StackOnly::CreateObject(); //��ջ��

	//StackOnly* st1 = new StackOnly;
	static StackOnly st2;
	StackOnly st3;
	return 0;
}