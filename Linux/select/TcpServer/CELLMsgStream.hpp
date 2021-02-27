#ifndef  _CELL_MSG_STREAM_H_
#define _CELL_MSG_STREAM_H_

#include "CELLStream.h"
#include "MsgHeader.h"

//������Ϣ�����ֽ���
class CELLRecvStream:public CELLStream
{
public:
	CELLRecvStream(DataHeader* dh):CELLStream((char *)dh,dh->dataLength){
		push(dh->dataLength);
	}

	uint16_t getNetCMD(){
		uint16_t cmd = CMD_ERROR;
		Read<uint16_t>(cmd);
		return cmd;
	}
private:

};

//д����Ϣ�����ֽ���
class CELLSendStream :public CELLStream
{
public:
	CELLSendStream(char* data, int size, bool bDelete = false) :CELLStream(data, size, bDelete) {
		//Ԥ�����Ϣ�ײ�λ��
		Write<uint16_t>(0);
	}

	CELLSendStream(int size = 1024) :CELLStream(size) {
		//Ԥ�����Ϣ�ײ�λ��
		Write<uint16_t>(0);
	}

	void setNetCMD(uint16_t cmd) {
		Write<uint16_t>(cmd);
	}

	void finish()
	{
		int pos = writeLength();
		setWritePos(0);
		Write<uint16_t>(pos);
		setWritePos(pos);
	}
private:

};
#endif // ! _CELL_MSG_STREAM_H_
