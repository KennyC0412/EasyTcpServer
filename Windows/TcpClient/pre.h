#pragma once
#pragma comment(lib,"ws2_32.lib")
#ifndef  _PRE_H_
#define _PRE_H_

namespace net {
	const short PORT = 8888;
}
const int BACKLOG = 4;
//const char IP{} = "192.168.206.128";
namespace net {
	const char IP[] = "192.168.206.128";
}


#endif // ! _PRE_H_
