#pragma once 

#include <iostream>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <strings.h>
#include <sys/sendfile.h>
#include <vector>
#include <unordered_map>
#include "Log.hpp"

#define BACKLOG 5

class Sock{
   public:
     //static成员函数是属于类的，而非对象的，也就是所有该类的对象共同拥有这一个成员函数，而不是普通的每个对象各自拥有一个成员函数
     static int Socket()
     {
         int sock = socket(AF_INET,SOCK_STREAM,0);
         if(sock < 0)
         {
           LOG(Fatal,"Socket create error");

           exit(SocketERR); 
         }
         return sock;
     }

     static void Bind(int sock,int port)
     {
         struct sockaddr_in local;
         bzero(&local,sizeof(local));
         //绑定初始化
         local.sin_family = AF_INET;
         local.sin_port   =  htons(port);
         local.sin_addr.s_addr = htonl(INADDR_ANY);

         if(bind(sock,(struct sockaddr*)&local,sizeof(local)) < 0 )
         {
           LOG(Fatal,"socket bind failed");
           exit(BindERR);
         }
     }

     static void Listen(int sock)
     {
        
       if(listen(sock,BACKLOG) < 0)
       {
         LOG(Fatal,"socket listen failed");
         exit(ListenERR);
       }
     }
     
     static void SetSockOpt(int sock)
     {

      int opt = 1;
      setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
     }

     static int Accept(int sock)
     {
       struct sockaddr_in peer;
       socklen_t len = sizeof(peer);
       int s = accept(sock,(struct sockaddr*)&peer,&len);
       if(s < 0)
       {
        LOG(Warning,"accept failed");
       }
       return s;
     }
     //作用：整体读取一行内容 
     static void GetLine(int sock,std::string &line)
     {
        //一般多行文本，是因为文本是以换行符作为分隔符将多行文本划分为多行
        //在这里要考虑回车换行的问题
        //\n ,\r ,\r\n  ->全部转化为\n
        //在此按字符读取
        char c = 'X';
        while(c != '\n')
        {
          ssize_t s = recv(sock,&c,1,0);//以阻塞方式去读取
          if(s > 0)
          {
            //读取成功
          if(c == '\r')
          {//如果读取到字符\r就去在检测一下下一个字符
             ssize_t ss = recv(sock,&c,1,MSG_PEEK); 
             if(ss > 0 && c == '\n')
             {//说明读取的字符是\r\n
              recv(sock,&c,1,0);

             }
             else 
             {
               //\r
               c = '\n';


             }

          }//if end

          //1.读取的是正常字符
          //2.读取的只是\n
          //3.\r\n \r -> \n
          if(c != '\n')
          line.push_back(c);
          }
        }
     }
};


