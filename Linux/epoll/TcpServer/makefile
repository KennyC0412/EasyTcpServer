objects = main.o CELLBuffer.o CELLClient.o CELLLog.o  CellServer.o CELLStream.o\
	 CELLTask.o CELLThread.o  NetEnvMan.o  Semaphore.o server.o CELLEpoll.o\
	epollServer.o CELLEpollServer.o

server : $(objects)
	g++ -o server  $(objects) -std=c++11 -pthread -g

main.o :preDefine.h MsgHeader.h
Alloc.o :  memorypool.hpp
CELLBuffer.o : preDefine.h MsgHeader.h
CELLClient.o : ObjectPool.hpp
CELLConfig.o : CELLLog.h
CELLLog.o : CELLTask.h
CELLMsgStream.o :
CellServer.o : CELLtimestamp.hpp
CELLStream.o : CELLLog.h
CELLTask.o : CELLThread.h
CELLThread.o : Semaphore.h
CELLtimestamp.o : 
memorypool.o : memorypool.hpp
NetEnvMan.o : preDefine.h
ObjectPool.o :ObjectPool.hpp
Semaphore.o :
CELLEpoll.o :CellServer.h CELLEpoll.h
epollServer.o : epollServer.h server.h
server.o : main.cpp CELLClient.h CELLThread.h
MyServer.o : epollServer.h MyServer.hpp
CELLEpollServer.o:

.PHONY : clean 
 clean : 
	 rm server $(objects)
