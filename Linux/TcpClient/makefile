#target = /home/csr/code/EasyTcpServer/Linux/TcpClient/*.*
.PHONY:client
client: client.cpp processor.cpp pre.h cmdThread.cpp
	g++ -o client client.cpp processor.cpp pre.h cmdThread.cpp -std=c++11\
	 -pthread


