@echo off
::�����IP��ַ
::@set strIP="any"
::����˶˿�
::@set nPort=8888
::��Ϣ�����߳�����
::@set nThread=2
::@set nClient=2
set cmd="strIP=any"
set cmd=%cmd% nPort=8888
set cmd=%cmd% nThread=2
set cmd=%cmd% nClient=2

TcpServer  %cmd%

@pause

