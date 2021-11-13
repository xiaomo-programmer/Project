BIN=http_server
SRC=main.cc
CC=g++
FLAGS=-std=c++11 -lpthread #-DDEBUG  #命令行式宏定义

.PHONY:all
all:$(BIN) CGI

$(BIN):$(SRC)
	$(CC) -o $@ $^ $(FLAGS)
.PHONY:CGI 
CGI:
	cd ./cgi; make; cd ..
.PHONY:clean
clean:
	rm -f $(BIN)
	cd ./cgi; make clean; cd ..;\
	rm -rf output;
.PHONY:output 
output:
	make clean;\
		make;\
	mkdir -p output/wwwroot/cgi;\
	cp $(BIN) output;\
	cp wwwroot/* output/wwwroot -rf;\
	cp cgi/test_cgi output/wwwroot/cgi;\
	cp cgi/test.py output/wwwroot/cgi;\
	cp cgi/test.sh output/wwwroot/cgi;\
	cp crtl.sh output;	 

