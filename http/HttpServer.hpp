#pragma once 
#include "Sock.hpp"
#include <pthread.h>
#include "Protocol.hpp"
#include "ThreadPool.hpp"
#define PORT 8081
class HttpServer{
  private:
    int port;//端口号
    int lsock; 
    ThreadPool *tp;
    static HttpServer *http_svr;
    static pthread_mutex_t lock;
    
    
  public:

    HttpServer(int _port=PORT)
      :port(_port)
      ,lsock(-1)
      ,tp(nullptr)
    {}
    static HttpServer* GetInstance(int sk)
    {
      if(nullptr == http_svr)
      {
        pthread_mutex_lock(&lock); //加锁
       if(nullptr == http_svr)
       {
        http_svr = new HttpServer(sk);
       }
        pthread_mutex_unlock(&lock);//解锁
      }
      return http_svr;
    }

    void Start()
    {
     //先用多线程版本
     for(;;)
     {
       int sock = Sock::Accept(lsock);
      if(sock < 0)
      {
        continue;
      }
      LOG(Notice,"get a new link..." );
      Task *tk = new Task(sock);
      tp->PushTask(tk);
      //demo
//      pthread_t tid;
//      int *sockp = new int(sock);
//      pthread_create(&tid,nullptr,Enter::Handerhttp,sockp);//多线程方案来处理请求
//
//      pthread_detach(tid);
//


     }

    }

    void HttpServerInit()//初始化服务器
    {
     lsock = Sock::Socket();//封装各种套接字
     Sock::SetSockOpt(lsock);    //将套接字设置为可复用状太
     Sock::Bind(lsock,port);//绑定
     Sock::Listen(lsock);        //监视
    
     tp = new ThreadPool(); //new一个线程池
     tp->InitThreadPool();  //初始化线程池
    }
 
    
    ~HttpServer()
    {
      if(lsock >= 0)
      {
        close(lsock);
      }

    }
    
};

HttpServer *HttpServer::http_svr = nullptr;//初始化static成员变量
pthread_mutex_t HttpServer::lock = PTHREAD_MUTEX_INITIALIZER; 
