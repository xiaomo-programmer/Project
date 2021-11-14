//#include "objectPool.h"
//#include <string>
//#include "ConcurrentAlloc.h"
//
//void func1()
//{
//	for (size_t i = 0; i < 10; i++)
//	{
//		ConcurrentAlloc(8);
//	}
//}
//
//
//void func2()
//{
//	for (size_t i = 0; i < 20; i++)
//	{
//		ConcurrentAlloc(17);
//	}
//}
//
//void TestThreads()
//{
//	std::thread t1(func1);
//	std::thread t2(func2);
//
//	t1.join();
//	t2.join();
//}
//
//
//void TestSzieClass()
//{
//	cout << SizeClass::Index(1025) << endl;
//	cout << SizeClass::Index(1024) << endl;
//}
//
//
//void TestConcurrentAlloc()
//{
//	void* ptr1 = ConcurrentAlloc(5);
//	void* ptr2 = ConcurrentAlloc(8);
//	void* ptr3 = ConcurrentAlloc(8);
//
//	ConcurrentFree(ptr1);
//	ConcurrentFree(ptr2);
//	ConcurrentFree(ptr3);
//
//}
//
//void TestBigMemory()
//{
//	void* ptr1 = ConcurrentAlloc(129 *4* 1024);
//	ConcurrentFree(ptr1);
//}
//

//int main()
//{
//	//TestObjectPool();
//	//TestThreads();
//	//TestSzieClass();
//	TestConcurrentAlloc();
//	//TestBigMemory();
//
//	
//	return 0;
//}



