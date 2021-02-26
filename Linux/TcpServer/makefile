header=./*.h
source=./*.cpp
achieve=./*.hpp
objects = main.o CELLBuffer.o CELLClient.o CELLLog.o  CellServer.o CELLStream.o\
	 CELLTask.o CELLThread.o  NetEnvMan.o  Semaphore.o server.o 

server : $(objects)
	g++ -o server  $(objects) -std=c++11 -pthread

main.o :pre.h messageHeader.h
Alloc.o :  memorypool.hpp
CELLBuffer.o : pre.h messageHeader.h
CELLClient.o : ObjectPool.hpp
CELLConfig.o : CELLLog.h
CELLLog.o : CELLTask.h
CELLMsgStream.o :
CellServer.o : FDset.hpp CELLtimestamp.hpp
CELLStream.o : CELLLog.h
CELLTask.o : CELLThread.h
CELLThread.o : Semaphore.h
CELLtimestamp.o : 
memorypool.o : memorypool.hpp
NetEnvMan.o : pre.h
ObjectPool.o :ObjectPool.hpp
Semaphore.o :
server.o : main.cpp CELLClient.h CELLThread.h
FDset.o : pre.h FDset.hpp
MyServer.o : MyServer.hpp

.PHONY : clean 
 clean : 
	 rm server $(objects)



