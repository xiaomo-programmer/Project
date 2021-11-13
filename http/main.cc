#include "HttpServer.hpp"
#include "Log.hpp"

static void Usage(std::string _proc)
{
  std::cout << "Usage: \n\t" ;
  std::cout <<"method one:" <<_proc << "port\n\t";
  std::cout <<"method one:" <<_proc << std::endl;;
}

int main(int argc,char *argv[])
{
    
  if(argc != 1 && argc!=2)
  {
      Usage(argv[0]);
      return 1;
  }
  HttpServer *ser = nullptr;
  if(argc == 1)
  {
  ser = HttpServer::GetInstance(8081);
  }
  else 
  {
  ser = HttpServer::GetInstance(atoi(argv[1]));
  }
  daemon(1,1);
  ser->HttpServerInit();
  ser->Start();

  return 0;
}
