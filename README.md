# EasyTcpServer
TCP服务器和客户端

1.0:使用recv send等函数进行单次收发

1.1:服务器和客户端通过while循环，一对一交互

1.2:将发送的报文进行结构化和封装，可以通过一次收发对信息进行处理

1.3：使用select模型升级服务器和客户端，使得服务器可以处理多个客户端的连接，timeval参数设置后成为非阻塞select，增加处理其他事务的能力

1.4：使用thread修改客户端，分离出一个线程用来输入命令。增加Linux版本客户端
