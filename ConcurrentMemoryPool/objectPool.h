//#pragma once
//
//#include "Common.h"
//
//
//
////template<size_t size> //���ĳһ������Ĵ�С��ֻ�������ڴ�
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
//template<class T>  //����أ��Ҳ�֪������ʲô���ͣ���ֱ�Ӵ����;Ϳ�
//class ObjectPool   //���ĳһ����������ͣ��������ڴ��ˣ��������һ�¹��캯�����г�ʼ��һ��
//{
//public:
//	~ObjectPool()
//	{
//		//...��ǰ�ڴ�ز�֧������
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
//		if (_freeList)   //��һ�ν�����freelist�ǿյ�
//		{
//			//���freeList��Ϊ��,������������ȡ�ڴ棬Ϊ��������
//			/*obj = (T*)_freeList;
//			_freeList =(void*)*((int*)_freeList);*/
//
//			obj = (T*)_freeList;
//			//_freeList =*((void**)_freeList);
//			_freeList = NextObj(_freeList);
//		}
//		else
//		{//��һ��Ҫ������ȡ�ڴ�
//			if (_leaveSize < sizeof(T) )   //����е����ʣһ���Ͷ�����
//			{
//				_leaveSize = 1024 * 100;
//				_memory = (char*)malloc(_leaveSize);  //memoryһֱ�ӿ��ܻ�Խ��
//				if (_memory == nullptr)
//				{
//					/*exit(-1);
//					cout << "�����ڴ�ʧ��" endl;*/
//					throw std::bad_alloc();
//				}
//			}
//			//�ߵ��������������1��_memoryû�У�ȥ����
//			//                  2��_memory���㹻�Ŀռ�
//
//
//			obj = (T*)_memory;
//			_memory = _memory + sizeof(T);
//			//�����ڴ�ط�������Ŀռ�û�г�ʼ���������Ҫ��λnew
//			
//			_leaveSize = _leaveSize - sizeof(T);
//			
//		}
//		new(obj)T; //���������ʾ��ȥ�������Ĺ��캯��
//		return obj;
//	}
//
//	void Delete(T* obj)
//	{
//		obj->~T();
//
//		//ͷ�嵽freeList
//		/*(*((int*)obj)) = (int)_freeList;*/
//		//*((void**)obj) = _freeList;   //ָ����һ����ַ��ָ������͵������Ƿ�������õ�ʱ�򿴶��
//		NextObj(obj) = _freeList;      //���ﻻ��void**��������void*�Ĵ�С
//		                              //void*��4����8
//		_freeList = obj;
//
//	}
//private:
//	char* _memory = nullptr;   //��ȱʡֵ��Ĭ������
//	int _leaveSize = 0;        //ʣ����ڴ�ش�С
//	void* _freeList = nullptr; //��������
//};
//
//struct TreeNode
//{
//	int _val;
//	TreeNode* _left;
//	TreeNode* _right;
//
//	TreeNode()  //��ʼ��
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

