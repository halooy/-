LFLAG =-L/usr/lib/x86_64-linux-gnu -lmysqlclient -ljsoncpp -lpthread
main:main.cpp db.hpp
	g++ -std=c++11 $^ -o $@ $(LFLAG)
