#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <array>
using namespace std;

//c++11首先对类型转换进行了扩展

class A
{
public:
	 A(int a)
		:_a(a)//单参数的构造函数
	{

	}
private:
	int _a;
};

class Point
{
public:
	 Point(int x = 0, int y = 0)//多参数的构造函数
		:_x(x), _y(y)
	{

	}

private:
	int _x;
	int _y;
};

//int main()
//{
//	//C++98
//	A a(1);
//	A a2 = 2;//单参数的类支持隐式类型转换(相近类型)
//	string s = "mzp";
//	vector<string> v;
//	v.push_back(s);
//	v.push_back("qzy");
//
//
//	//C++11 支持多参数类的隐式类型转换
//
//	Point p1={ 1, 2 };  //隐式类型转换(相近类型)  先拿1，2构造一个临时对象，在拿这个临时对象拷贝构造，在编译器优化  优化后，直接拿1，2构造
//	Point p2{ 1, 2 };
//   decltype(it) copy2;
//	//auto 推导是把双刃剑，如果不知道返回对象的类型，会降低可读性
//	//auto 不能做参数类型，不能做返回值类型
//	//auto 需要编译时推到类型，语法层的概念
//	//auto 底层实现角度，编译成函数指令需要先建立栈帧
//}

//int main()
//{
//	
//		// 内置类型变量
//		int x1 = { 10 };
//		int x2{ 10 };
//		int x3 = 1 + 2;
//		int x4 = { 1 + 2 };
//		int x5{ 1 + 2 };
//		// 数组
//		int arr1[5] {1, 2, 3, 4, 5};
//		int arr2[]{1, 2, 3, 4, 5};
//
//		// 动态数组，在C++98中不支持
//		int* arr3 = new int[5]{1, 2, 3, 4, 5};
//
//		// 标准容器
//		vector<int> v{ 1, 2, 3, 4, 5 };
//		map<int, int> m{ { 1, 1 }, { 2, 2, }, { 3, 3 }, { 4, 4 } };
//	
//
//	return 0;
//}



//范围for 底层原理：迭代器
//int main()
//{
//	vector<int> v{ 1, 2, 3, 4, 5 };
//	for (auto e : v)
//	{
//		cout << e << endl;
//	}
//	int a[] = { 1, 2, 3, 4 };
//	for (auto e : a)
//	{
//		cout << e << endl;
//	}
//}


//final 可以给函数加final ，表示这个函数不能被继承 override






int main()
{
	array<int, 100> a1;
	array<int, 1000> a2;

	cout << sizeof(a1) << endl;
	cout << sizeof(a2) << endl;
	//vector在堆上，也能用resize也能达到上面的作用
	//array其实没啥用
	//default 可以生成 -》 一般针对的是构造函数
	//delete  不让生成 -》 一般针对的是拷贝构造和赋值
	//c++ 98 防拷贝/防复制技术 -》1.只声明不实现，原因：如果不声明也不实现，编译器会默认生成，得声明一下（防止编译器默认生成）
	//                            2.声明成私有（防止别人在类外面定义）
	//c++11 提供delete ，让防拷贝更加简单更容易理解
}