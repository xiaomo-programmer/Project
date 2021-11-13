#!/bin/bash 
bin=http_server
port=8888
id=$(pidof $bin)
function start()
{
  if [ -z "$id" ];then 
    ./$bin $port 
    echo "$id process is running end . . ."
  else 
    echo "$bin is running!"
  
  fi
}



function stop()
{
  if [ -z "$id" ];then 
     
    echo "$id process is not exists"
  else 
    kill -9 $id 
    echo "$bin is end!"
  
  fi
}


function status()
{
  if `pidof $bin > /dev/null`;then 
    echo "status: running"
  else 
    echo "status: dead"
  fi 
}

case $1 in
  "start" )
   start
    ;;
  "stop" )
   stop
   ;;
  "restart" )
    start 
    stop
    ;;
   "status" )
    status
     ;;
   
 * )
   echo "Usage :$0 start | stop | status"
   echo "default"
   ;;
esac
