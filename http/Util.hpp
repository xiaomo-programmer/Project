#pragma once 


#include "Sock.hpp"
#include "Log.hpp"


class Util{

  public:
    static void StringParse(std::string &request_line,std::string &method,std::string &uri,std::string &version)
    {
          std::stringstream ss(request_line);
          ss >> method >> uri >> version;

    }
    static void MakeStringToKV(std::string line,std::string &k,std::string &v)
    {
      std::size_t pos = line.find(": ");
      if(pos != std::string::npos)//找到了
      {
        k = line.substr(0,pos); //k
        v = line.substr(pos+2); //v
      }
    }
    static ssize_t  StringToInt(std::string &v)
    {
      //stoi(v)
      std::stringstream ss(v);
      ssize_t i = 0;
        ss >> i;
        return i;
    }
};
