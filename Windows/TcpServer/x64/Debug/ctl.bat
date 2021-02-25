@echo off
::服务端IP地址
::@set strIP="any"
::服务端端口
::@set nPort=8888
::消息处理线程数量
::@set nThread=2
::@set nClient=2
set cmd="strIP=any"
set cmd=%cmd% nPort=8888
set cmd=%cmd% nThread=2
set cmd=%cmd% nClient=2

TcpServer  %cmd%

@pause

