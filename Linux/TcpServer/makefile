header=./*.h
source=./*.cpp
achieve=./*.hpp
server : $(header) $(source) $(achieve)
	g++ -o server $(header) $(source) $(achieve) -std=c++11 -pthread -g 

Alloc.o : memorypool.o
	g++ Alloc.hpp memorypool.hpp -std=c++11 -pthread -g
