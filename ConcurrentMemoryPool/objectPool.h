//#pragma once
//
//#include "Common.h"
//
//
//
////template<size_t size> //针对某一个具体的大小，只管申请内存
////class ObjectPool
////{
////
////};
//
//
//
//
//
//
//
//
//template<class T>  //对象池，我不知道你是什么类型，你直接传类型就可
//class ObjectPool   //针对某一个具体的类型，申请完内存了，还会调用一下构造函数进行初始化一下
//{
//public:
//	~ObjectPool()
//	{
//		//...当前内存池不支持析构
//	}
//
//	void*& NextObj(void* obj)
//	{
//		return *((void**)obj);
//	}
//
//	T* New()
//	{
//		T* obj = nullptr;
//		if (_freeList)   //第一次进来，freelist是空的
//		{
//			//如果freeList不为空,在自由链表里取内存，为二次利用
//			/*obj = (T*)_freeList;
//			_freeList =(void*)*((int*)_freeList);*/
//
//			obj = (T*)_freeList;
//			//_freeList =*((void**)_freeList);
//			_freeList = NextObj(_freeList);
//		}
//		else
//		{//第一次要来这里取内存
//			if (_leaveSize < sizeof(T) )   //如果切到最后，剩一点点就丢弃掉
//			{
//				_leaveSize = 1024 * 100;
//				_memory = (char*)malloc(_leaveSize);  //memory一直加可能会越界
//				if (_memory == nullptr)
//				{
//					/*exit(-1);
//					cout << "申请内存失败" endl;*/
//					throw std::bad_alloc();
//				}
//			}
//			//走到这有两种情况：1、_memory没有，去申请
//			//                  2、_memory有足够的空间
//
//
//			obj = (T*)_memory;
//			_memory = _memory + sizeof(T);
//			//这里内存池分配出来的空间没有初始化，因此需要定位new
//			
//			_leaveSize = _leaveSize - sizeof(T);
//			
//		}
//		new(obj)T; //这里可以显示的去调用它的构造函数
//		return obj;
//	}
//
//	void Delete(T* obj)
//	{
//		obj->~T();
//
//		//头插到freeList
//		/*(*((int*)obj)) = (int)_freeList;*/
//		//*((void**)obj) = _freeList;   //指针是一个地址，指针的类型的意义是方便解引用的时候看多大
//		NextObj(obj) = _freeList;      //这里换成void**解引用是void*的大小
//		                              //void*是4或者8
//		_freeList = obj;
//
//	}
//private:
//	char* _memory = nullptr;   //给缺省值，默认生成
//	int _leaveSize = 0;        //剩余的内存池大小
//	void* _freeList = nullptr; //自由链表
//};
//
//struct TreeNode
//{
//	int _val;
//	TreeNode* _left;
//	TreeNode* _right;
//
//	TreeNode()  //初始化
//		:_val(0)
//		, _left(nullptr)
//		, _right(nullptr)
//	{}
//};
//
//
//void TestObjectPool()
//{
//
//	//std::vector<TreeNode*> v1;
//	//for (size_t i = 0; i < 100; i++)
//	//{
//	//	TreeNode* node = TNPool.New();
//	//	cout << node << endl;
//
//	//	v1.push_back(node);
//
//	//	_sleep(500);
//	//}
//
//
//	//for (auto& e : v1)
//	//{
//	//	TNPool.Delete(v1.front());
//	//}
//
//	size_t begin1 = clock();
//	std::vector<TreeNode*> v1;
//	for (size_t i = 0; i < 100000; ++i)
//	{
//		v1.push_back(new TreeNode);
//	}
//
//	for (size_t i = 0; i < 100000; ++i)
//	{
//		delete v1[i];
//	}
//	v1.clear();
//	for (size_t i = 0; i < 100000; ++i)
//	{
//		v1.push_back(new TreeNode);
//	}
//	size_t end1 = clock();
//
//	ObjectPool<TreeNode> TNPool;
//
//	size_t begin2 = clock();
//	std::vector<TreeNode*> v2;
//	for (size_t i = 0; i < 100000; ++i)
//	{
//	
//		v2.push_back(TNPool.New());
//	}
//	
//	for (size_t i = 0; i < 100000; ++i)
//	{
//
//		TNPool.Delete(v2[i]);
//	}
//	v2.clear();
//	for (size_t i = 0; i < 100000; ++i)
//	{
//		v2.push_back(TNPool.New());
//	}
//	size_t end2 = clock();
//
//
//	cout << end1 - begin1 << endl;
//	cout << end2 - begin2 << endl;
//
//	/*TreeNode* node1 = TNPool.New(); 
//	TreeNode* node2 = TNPool.New();
//	TreeNode* node3 = TNPool.New();
//	TreeNode* node4 = TNPool.New();
//	TreeNode* node5 = TNPool.New();
//
//	cout << node1 << endl; 
//	cout << node2 << endl;
//	cout << node3 << endl;
//	cout << node4 << endl;
//	cout << node5 << endl;
//	TNPool.Delete(node1);
//	TNPool.Delete(node2);
//	TNPool.Delete(node5);
//
//	TreeNode* node6 = TNPool.New();
//	TreeNode* node7 = TNPool.New();
//	TreeNode* node8 = TNPool.New();
//
//	cout << node6 << endl;
//	cout << node7 << endl;
//	cout << node8 << endl;*/
//
//
//}

