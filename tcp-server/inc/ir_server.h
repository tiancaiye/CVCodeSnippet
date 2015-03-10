/*
 * ir_server.h
 *
 *  Created on: 2014-8-25
 *      Author: tiancai
 */

#ifndef IR_SERVER_HPP_
#define IR_SERVER_HPP_

#include <ir_comm.h>

class Server
{
public:
	typedef void (*SrvFunc)(Socket);

	void register_service(const Sockaddr& addr, SrvFunc func);
	bool init();
	void serve();

private:
	Socket listener;
	SrvFunc service;
	Sockaddr hostaddr;
};



#endif /* IR_SERVER_HPP_ */
