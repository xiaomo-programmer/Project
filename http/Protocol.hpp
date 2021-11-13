#pragma once 
#include "Log.hpp"
#include "Sock.hpp"
#include "Util.hpp"


#define WEBROOT "wwwroot" //web根目录
#define HOMEPAGE "index.html"
#define VERSION "HTTP/1.0"



static std::string CodeToDesc(int code)
{
  std::string desc;
  switch(code)
  {
    case 200:
      desc = "OK";
      break;
    case 404:
      desc = "404 Not Found";
      break;
    default:
      break;

  }
  return desc;
}
static std::string suffixToDesc(const std::string& suffix)
{
  if(suffix == ".html" || suffix == ".htm")
  {
    return "text/html";
  }
  else if(suffix == ".js")
  {
    return "application/x-javascript";
  }
  else if(suffix == ".css")
  {
    return "text/css";
  }
  else if(suffix == ".jpg")
  {
    return "image/jpeg";
  }
  else 
  
    return "text/html";
  

}

//请求 
class HttpRequest{
   private:
       std::string request_line;         //请求行
       std::vector<std::string> request_hander;       //请求报头 这里设置成vector是读取一行，就把目标行插入到vector，对应解析方便
       std::string blank;                //空行
       std::string request_body;         //请求正文 
   private:
       std::string method;               //请求方法
       std::string uri;
       std::string version;              //请求版本号

       std::string path;                 //uri中的路径
       std::string query_string;         //uri中的参数


       std::unordered_map<std::string,std::string> hander_kv; //基于map的哈希表,用一个key值直接找到他的value值             

       ssize_t content_length;

       bool cgi;

       ssize_t file_size;

       std::string suffix;//后缀文件类型
   public:
  HttpRequest():blank("\n"),content_length(-1),path(WEBROOT),cgi(false),suffix("text/html")       //构造函数
  {

  }
  void SetRequestline(const std::string &line)
  {
    request_line = line;
  }
  
  void InsertHeaderLine(const std::string &line)
  {
    request_hander.push_back(line);
    //打印日志
    LOG(Notice,line);
  }
  void RequestlineParse()
  {
    //把请求行 解析成 method uri version 
    Util::StringParse(request_line,method,uri,version);
    //打印日志
//    LOG(Notice,request_line);
//    LOG(Notice,method);
//    LOG(Notice,uri);
//    LOG(Notice,version);

  }

  void RequestHanderParse()
  {
    for(auto it = request_hander.begin();it!=request_hander.end();it++)
    {
      std::string k,v;
      Util::MakeStringToKV(*it,k,v);
      LOG(Notice,k);
      LOG(Notice,v);
      if(k == "Content-Length")
      {
        content_length = Util::StringToInt(v);
      }
      hander_kv.insert({k,v});
    }
  }


  bool IsNeedRecvBody()
  {
    //必须满足两个条件：1.请求方法是post方法，2.content-length不能为0
    
    //post可能有大小写  strcasecmp:忽略大小写比较字符串
    if(strcasecmp(method.c_str(),"POST") == 0 && content_length > 0)
    {
      //post有正文，使用的方式是cgi技术
      cgi = true;
      return true;
       // auto it = hander_kv.find("Content-Length");
       // if(it !=hander_kv.end())
       // {
       //   
       // }
    }
    return false;
  }

  ssize_t GetContentLength()
  {
    return content_length;
  }

  void SetRequestBody(const std::string & body)
  { 
    request_body = body;
  }

  bool IsMethodLegal()
  {
    if(strcasecmp(method.c_str(),"POST")==0 || strcasecmp(method.c_str(),"GET")==0)
    {
    return true; 
    }
    return false;
  }

  bool IsGet()
  {
    bool ret = strcasecmp(method.c_str(),"GET") == 0 ? true : false;
    return ret;
  }

  
  bool IsPost()
  {
    bool ret = strcasecmp(method.c_str(),"POST") == 0 ? true : false;
    return ret;
  }

  void UriParse()
  {
    //GET方法uri存在
    std::size_t pos = uri.find('?');
    if(pos == std::string::npos )
    {
      path += uri;
    }
    else 
    { //有参数 ,也以cgi方式去运行
      path += uri.substr(0,pos);        
      query_string = uri.substr(pos+1);
      cgi = true;
    }

  }
    
  void SetUriEqPath()
  {
      path += uri;
  }

  void IsAddHomePage()
  {  //path有两种情况 1./ 2./a/b/c.html  或者post方法/s
    if(path[path.size()-1]=='/')
    {
      path+=HOMEPAGE;
    }
    
  }
  std::string GetPath()
  {
    return path;
  }

  void SetPath(std::string _path)
  {
    path = _path;
  }

  void SetCgi()
  {
    cgi = true;
  }

  bool IsCgi()
  {
    return cgi;
  }

  void SetFileSize(ssize_t s)
  {
    file_size = s;
  }

  ssize_t GetFileSize()
  {
    return file_size;
  }
  
  std::string GetQueryString()
  {
    return query_string;
  }

  std::string Getbody()
  {
    return request_body;
  }

  std::string GetMethod()
  {
    return method;
  }

  std::string MakeSuffix() //找文件后缀类型
  {
    std::string suffix;
    std::size_t pos = path.rfind(".");
    if(pos != std::string::npos)
    {
      suffix = path.substr(pos); 
    }
  return suffix;

  }
  

  ~HttpRequest()                        //析构函数
  {}


};
//响应
class HttpResponse{
      private:
        std::string status_line; //状态行
        std::vector<std::string> response_hander; //响应报头
        std::string blank;       //空行
        std::string response_body;   //响应正文

      public:
        HttpResponse():blank("\r\n")
        {}
        void SetStatusLine(const std::string &sline)
        {
          status_line = sline;
        }
        std::string GetStatusLine()
        {
          return status_line;

        }
        const std::vector<std::string>& GetRsphander()
        {
          response_hander.push_back(blank);
          return response_hander;
        }

        void AddHander(const std::string& ct)//把content-type插入到响应报头
        {
        response_hander.push_back(ct);
        }
        ~HttpResponse()
        {}
}; 

class EndPoint{  //EndPoint只负责读和写，读取信息设置进request、response，由request或者response来完成解析，即只根据协议来进行文本处理
     private:
     int sock;
     HttpRequest req;
     HttpResponse rsp;

     private:
     void GetRequestLine()      //读取请求行
     {
       std::string line;
       Sock::GetLine(sock,line);//获取行
       req.SetRequestline(line);//把获取行设置进request
       req.RequestlineParse();  //解析行
     }
     void SetResponseStatusLine(int code)     //读取状态行
     {
          std::string status_line;
          status_line = VERSION;   //协议版本
          status_line += " ";
          status_line += std::to_string(code); //状态码
          status_line += " ";  
          status_line += CodeToDesc(code);     //状态码描述
          status_line += "\r\n";
      
          rsp.SetStatusLine(status_line);
     }

     void GetRequestHeader()   //读取报头
     {
       //这里如果没用报头，但一定有空行
       std::string line;
       do 
       {
         line.clear(); //清空
         Sock::GetLine(sock,line);//获取行
         req.InsertHeaderLine(line);//把获取的行插入到vector中


       }while(!line.empty());//在读到空行时，一直读取，就把报头读完了

       //解析
       req.RequestHanderParse();
     }

     void GetRequestBody()
     {
       ssize_t len = req.GetContentLength();
       char c;
       std::string body;
       while(len)
       {
         ssize_t s = recv(sock,&c,1,0);
         body.push_back(c);
         len--;
       }
       req.SetRequestBody(body);//把正文设置今request

     }

     void SetResponseHanderLine()
     {
       std::string suffix = req.MakeSuffix();//制作后缀   
       std::string content_type = "Content-Type: ";
       content_type += suffixToDesc(suffix);
       content_type += "\r\n";
       rsp.AddHander(content_type);
      
     }
     public:
     EndPoint(int _sock)
       :sock(_sock)
     {}

     void RecvRequest()
     {
       //1获取完整的http请求
       //2分析http请求
       //按行读取请求 在util中编写按行请求
       //在短链接的情况下，请求是以行为基本单位
       GetRequestLine();//第一行读取完毕

       //读取报头 空行也读取完毕
       GetRequestHeader();

       //是否需要读取正文
       if(req.IsNeedRecvBody())//为真读取，为假不读取
       {
         GetRequestBody();          
       }
       //已经读完了所有请求

     }
     void MakeResponse()
     {
       //开始分析
       //只处理GET 和 POST方法 
       int code = 200;
       ssize_t size = 0; //file_size
       std::string path;
       if(!req.IsMethodLegal())//判断请求方法是否合法
       {
         //如果不合法，响应时需要状态码 ，属于客户端的错误 错误码以4开头
         LOG(Warning,"method is not legal!");
         code = 404;
         goto end;
       }
       //分析url ，get方法需要分析，并且有两种：有参数和无参数
       if(req.IsGet())
       { //判断是否为get方法
         req.UriParse();  //进行解析
       }
       else 
       { //这里就是POST方法 直接让uri变成path
         req.SetUriEqPath();
       }
       //path分为两种情况 1、最后一个元素是有效的 2.后面就跟了个/
       req.IsAddHomePage(); //是否添加首页


       //此时会有如下三种情况
       //如果GET方法没有参数 需要访问的资源在 path中
       //如果GET方法有参数   需要访问的资源在 path中，但是传递资源在query_string
       //如果POST方法 访问的资源 路径放在path中 && 参数在body正文中
       path = req.GetPath();
       LOG(Notice,path);        
       struct stat st;
       if(stat(path.c_str(),&st) < 0) //小于0说明文件不存在
       {
         LOG(Warning,"html is not exist! 404");
         code = 404;
         goto end;
       }
       else //说明文件存在 
       { 
         if(S_ISDIR(st.st_mode)) //如果是一个目录,并且存在
         {
           //走到这里，末尾绝对不是/ 而是/a/b/c/index.html
           path += "/";
           req.SetPath(path); 
           req.IsAddHomePage(); //添加分隔符HOMEPAGE 
         }
         else //它不是目录
         {
           //它可能是一个可执行程序，并不是把这个可执行程序拿下来，可能是把这个可执行程序跑完，把结果拿回来，这种技术叫做cgi技术


           if((st.st_mode & S_IXUSR) ||\
               (st.st_mode & S_IXGRP) ||\
               (st.st_mode & S_IXOTH))   //如果三者中有任何一个人拥有可执行权限
           {
             //cgi
             req.SetCgi();
           }
           else  
           { //正常的网页请求
             //do nothing
           }  

         }
        if(!req.IsCgi()) //如果不是cgi方式
        {
           req.SetFileSize(st.st_size); //获取文件的大小
        
        }
       }
        

end:
       //制作response
       SetResponseStatusLine(code);//响应行
       SetResponseHanderLine();//响应报头
     }
     void EXecNonCgi(const std::string& path) //非cgi方式
     {
       ssize_t size = req.GetFileSize();
      int fd = open(path.c_str(),O_RDONLY); //这个文件一定存在，有可能没有读权限的情况，但是情况不存在
      if(fd < 0) //判断是否合法
      {
        LOG(Error,"path is not exists bug!!!");
        return;
      }
      sendfile(sock,fd,nullptr,size); //函数在两个文件描写叙述符之间直接传递数据(全然在内核中操作，传送)，从而避免了内核缓冲区数据和用户缓冲区数据之间的拷贝，操作效率非常高，被称之为零拷贝
      close(fd);

     }

     void ExecCgi()
     {
     // std::string arg;
     // if(req.IsGet())
     // {
     //   arg = req.GetQueryString();//参数在query_string中
     // }
     // else 
     // {
     //   arg = req.Getbody();
     // }
      std::string content_length_env;
      std::string path = req.GetPath(); //获取路径
      std::string method = req.GetMethod(); 
      std::string method_env = "METHOD=";
      method_env += method;
      LOG(Notice,method);

      std::string query_string;
      std::string query_string_env;

      std::string body;
      int pipe_in[2] ={0};    
      int pipe_out[2] ={0};   

      //在被调用进程的角度 
      pipe(pipe_in);
      pipe(pipe_out);

      putenv((char*)method_env.c_str());//把环境变量给父进程
    
      pid_t id = fork(); //创建子进程
      if(id == 0) //子进程
      { //child
          close(pipe_in[1]);  //pipe_in要读取，所以要关闭写
          close(pipe_out[0]); //pipe_out要写入 所以要关闭读


          dup2(pipe_in[0],0); //重定向
          dup2(pipe_out[1],1);

          //两种方式
          //1.通过环境变量把参数传递给子进程 -> GET方法 -> query_string 
          if(req.IsGet()) //如果请求方法是get方法
          {
            query_string = req.GetQueryString();
            query_string_env = "QUERY_STRING=";
            query_string_env += query_string;
          putenv((char*)query_string_env.c_str());
          }
          else if(req.IsPost())
          {
          content_length_env = "CONTENT-LENGTH=";
          content_length_env += std::to_string(req.GetContentLength());
          LOG(Notice,content_length_env);
          putenv((char*)content_length_env.c_str());
          }
          else 
          {
          }
          //子进程进行程序替换 exec
          //执行程序
          execl(path.c_str(),path.c_str(),nullptr); 
          exit(0); //如果失败，直接终止
      }
    
      
          close(pipe_in[0]);  //pipe_in要写入，所以要关闭读
          close(pipe_out[1]); //pipe_out要读取 所以要关闭写
          //father      
          //2.通过管道的方式传递给子进程  -> POst方法 -> request_body 
          char c = 'X';
          if(req.IsPost()) //如果请求方法是post方法
          {
            body = req.Getbody();
            LOG(Notice,body);
            //把body字段写入管道里面
            int i = 0;
            for( i= 0;i < body.size();i++)
            {
            write(pipe_in[1],&body[i],1); 
            }
          }
            
            ssize_t s = 0;
            do 
            {
            s = read(pipe_out[0],&c,1);//父进程从管道里读取
            if(s > 0)
            { //cgi响应正文
              send(sock,&c,1,0); //这里不是一个字节一个字节的发送，而是拷贝到发送缓冲区里，合适的时候发送
            }
            }while(s > 0); //管道是文件，文锦描述符的生命周期随进程，子进程关闭，管道也关闭，此时必定能读到零值，循环退出
              
            //父进程完毕后要等待子进程
            waitpid(id,nullptr,0);//以阻塞的方式进行等待
     }

     void SendResponse()
     {
       std::string line = rsp.GetStatusLine();
       send(sock,line.c_str(),line.size(),0); //send本质是把数据拷贝到发送缓冲区，发送响应行
       const auto& hander = rsp.GetRsphander();
       auto it = hander.begin();
       for(;it!=hander.end();it++)                //发送响应报头
       {
         send(sock,it->c_str(),it->size(),0);
         
       }

       if(req.IsCgi()) //是否是cgi
       {
         //以cgi方式运行
         LOG(Notice,"use cgi model");
         ExecCgi();
       }
       else 
       { 
         LOG(Notice,"use non cgi model");
         //走到这里没有传递参数，请求一个url资源存在path中
         std::string path = req.GetPath(); //获取一个文件资源，保存在path中
         EXecNonCgi(path); //以非cgi方式去执行
       }

     }
     ~EndPoint()
     {
       if(sock >= 0)
       {
         close(sock);
       }
     }

};

class Enter{
  public:
    static void Handerhttp(int sock)
    {
     // int sock = *(int*)arg;



     //  delete  (int*)arg;

      //条件编译
      //#define DEBUG 方法一   方法二在Makefile种加一个-DTEST
#ifdef TEST 
      char request[10240];
      recv(sock,request,sizeof(request),0);

      //std::cout<< request << std::endl;


      close(sock);


#else
      // std::cout <<"other code"    << std::endl; 

      EndPoint *ep = new EndPoint(sock);
      ep->RecvRequest();//处理请求 读取请求和分析请求
      ep->MakeResponse(); //根据处理的结果制作一个响应
      ep->SendResponse();


        delete ep;
#endif 

     //nothing
    }

};




