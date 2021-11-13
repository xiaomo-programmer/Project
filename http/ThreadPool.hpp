#pragma once 


#include <iostream>
#include <queue>
#include <pthread.h>
#include "Protocol.hpp"

typedef void (*hander_t)(int);

class Task 
{
      private:
        int sock;
        hander_t hander;

      public:
        Task(int sk)
          :sock(sk)
          ,hander(Enter::Handerhttp)
        {}
        void Run()
        {
          hander(sock);
        }
        ~Task()
        {}
};

class ThreadPool
{
  private:
    std::queue<Task*> q; //任务队列
    int num; //线程池中线程的个数
    pthread_mutex_t lock;
    pthread_cond_t cond;
  public:
    ThreadPool(int n = 8)
      :num(n)
    {}

    void LockQueue()
    {
      pthread_mutex_lock(&lock);
    }
    void UnLockQueue()
    {
      pthread_mutex_unlock(&lock);
    }

    void ThreadWait()
    {
      pthread_cond_wait(&cond,&lock);
    }

    void ThreadWakeUp()
    {
      pthread_cond_signal(&cond);
    }
    bool IsEmpty()
    {
      return q.size() == 0 ? true : false;
    }
    static void* Routine(void* args)
    {
      ThreadPool *tp = (ThreadPool*)args;

      tp->LockQueue();
      while(tp->IsEmpty())
      {    //如果tp为空，线程进行休眠
        tp->ThreadWait();

      }

      Task *tk = tp->PopTask();
      tp->UnLockQueue();

      tk->Run();
      //TODO
      delete tk;
    }
    void InitThreadPool() //初始化
    {
      pthread_mutex_init(&lock,nullptr);
      pthread_cond_init(&cond,nullptr);
      pthread_t tid;
      for(int i =0;i < num;i++)
      {
        pthread_create(&tid,nullptr,Routine,this);
        pthread_detach(tid);
      }
    }

    void PushTask(Task *tk) //添加任务
    {
      LockQueue();
      q.push(tk);
      UnLockQueue();
      ThreadWakeUp(); //有任务之后唤醒线程 
    }
    
    

    Task* PopTask()
    {
      Task *tk = q.front();
      q.pop();
      return tk;
    }
    ~ThreadPool()
    {
      pthread_mutex_destroy(&lock);
      pthread_cond_destroy(&cond);

    }
};
