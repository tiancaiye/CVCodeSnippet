/*
 * ir_server.cpp
 *
 *  Created on: 2014-8-25
 *      Author: tiancai
 */

#include "stdafx.h"

#include <thread>
#include <signal.h>
#include <pthread.h>

#include "ir_server.h"

using namespace std;

bool Server::init()
{
	if (!listener.open() || !listener.bind(hostaddr) || !listener.listen())
	{
		cerr << "init server failed!";
		return false;
	}
	return true;
}

void Server::register_service(const Sockaddr& addr, SrvFunc func)
{
	hostaddr = addr;
	service = func;
}

void iothread(Socket sk, Server::SrvFunc service)
{
	try
	{
		service(move(sk));
	}
	catch(...)
	{
		cerr << "Unexpected Service Error!" << endl;
	}
}

bool should_be_deleted(const pair<pthread_t, time_t>& t)
{
	if (pthread_kill(t.first, 0) != 0) // thread has already finished
		return true;
	if ((time(NULL) - t.second) > 15) // thread runs above the time limits
	{
		pthread_kill(t.first, SIGTERM);
		return true;
	}
	return false;
}

void Server::serve()
{
	while (1)
	{
		Socket sk = listener.accept();
		if (!sk.is_valid())
		{
			cerr << "invalid socket returned!" << endl;
			continue;
		}
		thread t(iothread, move(sk), service);
		t.detach();
	}
}
