#pragma once 

#include <iostream>
#include <string>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <sstream>
//日志级别
#define Notice  1

#define Warning 2

#define Error   3

#define Fatal   4

enum ERR{
  SocketERR=1,
  BindERR,
  ListenERR,
  ArgERR

};

#define LOG(level,message) \
     Log(#level,message,__FILE__,__LINE__);



//日志
void Log(std::string level,std::string message, std::string filename, ssize_t line)
{
  struct timeval curr;
  gettimeofday(&curr,nullptr);
//重定向
//  int fd =-1;
//  if((fd = open("LOG.txt",O_RDWR|O_CREAT|O_TRUNC))== -1)
//  {
//    return ;
//  }
//  dup2(fd,1);

 // std::cout<<"[ " << level << " ]"<<"[ "<< message <<" ]" << "[ " <<"[ "<<curr.tv_sec<<" ]"<<"[ "<< filename << " ]" <<"[ " << line << " ]"<< std::endl;
//  close(fd);
}



