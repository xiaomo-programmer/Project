#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <cstring>

void CalData(std::string& query_string)
{
  //假设字符串是 a=100&b=200
  //先以&提取出两部分
  std::string part1;
  std::string part2;
  std::size_t pos = query_string.find("&");
  int x = 0;
  int y = 0;
  if(pos!=std::string::npos)
  {
    part1 = query_string.substr(0,pos); //a=100;
    part2 = query_string.substr(pos+1); //b=200;
  }
  //然后分别提取100和200
  pos = part1.find("=");
  if(pos!=std::string::npos)
  {
   x = atoi(part1.substr(pos+1).c_str());
  }
  
  pos = part2.find("=");
  if(pos!=std::string::npos)
  {
   y = atoi(part2.substr(pos+1).c_str());
  }
  std::cout << "<html>" << std::endl;
  std::cout <<"<h1>" << x << "+" << y << "=" << x+y <<"</h1>"<< std::endl;
  std::cout << x << "-" << y << "=" << x-y << std::endl;
  std::cout << x << "*" << y << "=" << x*y << std::endl;
  std::cout << x << "/" << y << "=" << x/y << std::endl;
  std::cout << "</html>" << std::endl;
}

int main()
{
  std::cout << "cgi success run!" << std::endl;
  std::string method; //请求方法
  std::string query_string; //参数
  if(getenv("METHOD")) 
  {
    method = getenv("METHOD");
  }
  else 
  {
    return 1;
  }
  if(strcasecmp(method.c_str(),"GET") == 0)//请求方法为get方法
  {
    query_string = getenv("QUERY_STRING");

  }
  else if(strcasecmp(method.c_str(),"POST") == 0)//请求方法为POST方法
  {
    //让 cgi 通过 标准输入0 来读取
    //            标准输出1 来写入
  int  cl = atoi(getenv("CONTENT-LENGTH"));
  std::cout <<  "hello1: " << cl << std::endl;
    char c = 0;
    while(cl)
    {
      read(0,&c,1);
      query_string.push_back(c);
      cl--;
    }
  std::cout <<  "hello2: " << query_string << std::endl;
  
  }
  //走到这里，参数都在query_string里面
  //std::cout <<"use cgi #"<< query_string << std::endl;
  CalData(query_string);
  return 0;
}
