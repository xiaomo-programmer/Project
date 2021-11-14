#pragma once
//此文件里放一些公共的函数、头文件、函数等
#include <iostream>

//项目当中尽量不要把std空间展开
#include <vector>
#include <time.h>
#include <assert.h>
#include <algorithm>
#include <map>
#include <unordered_map>
#include <mutex>

#ifdef _WIN32
#include <windows.h>
#else
//brk,mmap
#endif

#include <Thread>
using std::cout;
using std::endl;

//这里不用宏定义的原因：不方便调试
static const size_t MAX_BYTES = 64 * 1024;  //64k 16page
static const size_t NFREELISTS = 184;      
static const size_t NPAGES = 129;
static const size_t PAGE_SHIFT = 12;


#ifdef _WIN32
typedef int ADDRES_INT; //4字节

#else  //64位下
typedef unsigned long long  ADDRES_INT; //八字节

#endif


#ifdef _WIN32
typedef size_t PageId;
#else// 64位下
typedef unsigned long long  ADDRES_INT; 
#endif

//static size_t Index(size_t size)   //static除了改变生命周期，还改变连接属性，只在当前文件可见
//{
//	return ((size + (8 - 1)) >> 3) - 1;
//}


//哈希的位置
//size_t Index(size_t size) 
//{
//	if (size % 8 == 0)
//	{
//		return size / 8 - 1;
//	}
//	else
//	{
//		return size / 8;
//	}
//}

/**********************************************************************/



//内存对象大小的映射和对齐
class SizeClass
{
public:
	// 控制在12%左右的内碎片浪费
	// [1,128]                8byte对齐   		freelist[0,16)
	// [129,1024]             16byte对齐		freelist[16,72)
	// [1025,8*1024]		  128byte对齐		freelist[72,128)
	// [8*1024+1,64*1024]     1024byte对齐  	freelist[128,184)

	//[1,8]    +7  = [8,15]
	//[9,16]   +7  = [16,23]
	static inline size_t _Roundup(size_t bytes, size_t aglin)
	{
		return ((bytes + aglin - 1) & ~(aglin - 1));

	}

	// 对齐大小计算，浪费大概在1%-12%左右
	static inline size_t Roundup(size_t byte)
	{
		

		if (byte <= 128)
		{
			return _Roundup(byte, 8);

		}
		else if (byte <= 1024)
		{
			return _Roundup(byte, 16);
		}
		else if (byte <= 8192)
		{
			return _Roundup(byte, 128);
		}
		else if (byte <= 65536)
		{
			return _Roundup(byte, 1024);
		}
		else
		{
			//此项目定义的一页为4KB，也就是2^12byte
			return _Roundup(byte, 1 << PAGE_SHIFT);
		}

		return -1;
	}



	//计算在第几个桶子
	static inline size_t _Index(size_t bytes, size_t align_shift)
	{
		return ((bytes + (1 << align_shift) - 1) >> align_shift) - 1;
	}


	//映射的自由链表桶的位置
	static inline size_t Index(size_t byte)
	{
		assert(byte <= MAX_BYTES); //大于MAX_BYTES就从pagecache去申请了


		static int group_array[4] = { 16, 56, 56, 56 };//每个区间自由链表桶的个数
		if (byte <= 128)
		{
			return _Index(byte, 3);
		}
		else if (byte <= 1024)
		{
			return _Index(byte-128, 4) + group_array[0];
		}
		else if (byte <= 8 * 1024)
		{
			return _Index(byte - 1024, 7) + group_array[0] + group_array[1];
		}
		else if (byte <= 64 * 1024)
		{
			return _Index(byte - 8*1024, 7) + group_array[0] + group_array[1] + group_array[2];
		}

		assert(false);

		return -1;
	}

	//一次从中心缓存获取的个数
	static size_t NumMoveSize(size_t size)
	{
		if (size == 0)
		{
			return 0;
		}

		//[2,512]  一次批量移动多少个对象（慢启动）的上限值
		//小对象一次移动上限高
		//大对象一次移动上限低

		int num = MAX_BYTES / size;
		if (num < 2)
			num = 2;

		if (num > 512)
			num = 512;


		return num;
	}


	// 计算一次向系统获取几个页
	//单个对象可能为 8byte
	//单个对象也肯能为64KB 为16个页
	static size_t NumMovePage(size_t size)
	{
		size_t num = NumMoveSize(size);
		size_t npage = num * size;
		npage = npage >> 12; //除以4K，右移12位


		if (npage == 0)
			npage = 1;

		return npage;
	}
};

/**********************************************************************/


//管理一个跨度的大块内存(以页为单位)，其中一个作用是把大内存分成小份

struct Span                    //span有两种情况：1.span空间还没有分配 2.分配出去了但又还回来了
{
	PageId _Page_Id = 0; //页号
	size_t _n = 0;        //页数


	Span* _next = nullptr;
	Span* _prev = nullptr;



	void* _List = nullptr; //大块内存切成小块，链接起来，原因：这样回收回来的内存方便链接
	size_t _use_count = 0; //使用计数 == 0，说明所有对像都回来了
	size_t _objsize  = 0;   //切出来的单个对象的大小
};

class SpanList //管理大块内存的双向链表
{
public:
	SpanList()
	{
		_head = new Span;
		_head->_next = _head;
		_head->_prev = _head;
	}


	Span* Begin()
	{
		return _head->_next;
	}

	Span* end()
	{
		return _head;
	}

	void Insert(Span* cur, Span* newspan) //在pos位置之前插入一个节点
	{
		Span* prev = cur->_prev;     //这里不是我们自己开辟出来的空间

		//Insert(prev newspan cur)
		prev->_next = newspan;
		newspan->_next = cur;
		
		newspan->_prev = prev;
		cur->_prev = newspan;


	}



	void Erase(Span* cur)
	{
		assert(cur != _head);
		Span* prev = cur->_prev;
		Span* next = cur->_next;

		prev->_next = next;
		next->_prev = prev;

		//这里不需要自己delete释放掉，因为这里只是说span不在centralcache了，并没有说空间释放了
	}


	bool Empty()
	{
		return _head == _head->_next;
	}

	void PushFront(Span* span)
	{
		Insert(Begin(), span);
	}

	Span* PopFront()
	{
		Span* span = Begin();
		Erase(span);

		return span;
	}

	/*void lock()
	{
		_mtx.lock();
	}

	void unlock()
	{
		_mtx.unlock();
	}*/
private:
	Span* _head;

public:

	std::mutex _mtx;
};




inline static void* SystemAlloc(size_t kpage)
{
#ifdef _WIN32
	void* ptr = VirtualAlloc(0, kpage*(1 << PAGE_SHIFT),MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
#else
	// brk mmap等
#endif
	if (ptr == nullptr)
		throw std::bad_alloc();
	return ptr;
}
inline static void SystemFree(void* ptr)
{
#ifdef _WIN32
	VirtualFree(ptr, 0, MEM_RELEASE);
#else
	// sbrk unmmap等
#endif
}










/*********************************************************************************************************************/

//取头4或8个字节   
inline void*& NextObj(void* obj)//把obj强转成void**再解引用，32位下是4字节，64位下就是8字节
{
	return *((void**)obj);
}


//自由链表
class FreeList
{
public:
	//把多个内存对象链接到自由链表上
	void PushRange(void* start,void* end,int n)
	{
		NextObj(end) = _head;
		_head = start;

		//自由链表上挂了多少个内存对象
		_size = _size + n;
	}

	//把多给对象pop出threadcache的自由链表
	void PopRange(void*& start,void*& end,int n)
	{
		
		start = _head;
		for (int i = 0; i < n; i++)
		{
			end = _head;
			_head = NextObj(_head);
			
		}
		NextObj(end) = nullptr;
		_size = _size - n;
	}

	//头插法
	void Push(void* obj)
	{
		NextObj(obj) = _head;
		_head = obj;
		_size += 1;
	}

	//头删法
	void* Pop()
	{
		void* obj = _head;
		_head = NextObj(_head);
		_size -= 1;


		return obj;
	}

	bool Empty()
	{
		return _head == nullptr;
	}

	size_t MAX_SIZE()
	{
		return _max_size;
	}

	void SetMaxSize(size_t i)
	{
		_max_size = i;
	}

	size_t Size()
	{
		return _size;
	}

private:
	void* _head = nullptr;
	size_t _max_size = 1;
	size_t _size = 0;
};


/************************************************************************************************************************/