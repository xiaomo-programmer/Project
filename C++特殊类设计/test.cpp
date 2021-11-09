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
//	// 1.只声明,不实现。因为实现可能会很麻烦，而你本身不需要
//	// 2.声明成私有
//	//HeapOnly(const HeapOnly&);
//
//		// or
//
//		// C++11 
//		HeapOnly(const HeapOnly&) = delete;
//};
//只能在栈上开辟的对象类 
//推荐这个 
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

//屏蔽new
//缺陷：防止不了静态区的对象  全局对象和静态对象
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
	//HeapOnly::CreateObject();//在堆上
	// StackOnly st = StackOnly::CreateObject(); //在栈上

	//StackOnly* st1 = new StackOnly;
	static StackOnly st2;
	StackOnly st3;
	return 0;
}