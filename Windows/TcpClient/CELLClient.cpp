#include "CELLClient.h"


	int CELLClient::push(DataHeaderPtr& dh) {
		int sendLen = dh->dataLength;
		const char* data = reinterpret_cast<const char*>(dh.get());
		if (_sendBuff.push(data, sendLen)) {
			return sendLen;
		}
	}

	void CELLClient::pop_front_Msg()
	{
		if (hasMsg()) {
			_recvBuff.pop(front_Msg()->dataLength);
		}
	}

	int CELLClient::sendData() {
		rstDtSend();
		return _sendBuff.sendData(_sockfd);
	}

	int CELLClient::recvData()
	{
		return _recvBuff.recvData(_sockfd);
	}

	bool CELLClient::hasMsg()
	{
		return _recvBuff.hasMsg();
	}

	DataHeader* CELLClient::front_Msg()
	{
		return reinterpret_cast<DataHeader*>(_recvBuff.data());
	}

	//���������
	bool CELLClient::checkHeart(time_t t) {
			dtHeart += t;
			return dtHeart >= CLIENT_HEART_DEAD_TIME?true : false;
	}

	//��ʱ���ͼ��
	void CELLClient::checkSend(time_t t) {
		dtSend += t;
		if (dtSend >= CLIENT_SEND_BUFF_TIME) {
			//������������
			sendData();
			rstDtSend();
		}
	}

