/*
 * ir_comm.cpp
 *
 *  Created on: 2014-8-25
 *      Author: tiancai
 */

#include "stdafx.h"
#include "ir_comm.h"

using namespace std;

Sockaddr::Sockaddr()
{
	init_sa();
}

Sockaddr::Sockaddr(const string& addr, int port)
{
	init_sa();
	set_addr(addr);
	set_port(port);
}

void Sockaddr::Sockaddr::init_sa()
{
	memset(&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = INADDR_ANY;
	sa.sin_port = htons(8000);
}

string Sockaddr::get_addr()
{
	char addr[INET_ADDRSTRLEN];
	if (inet_ntop(AF_INET, &(sa.sin_addr.s_addr), addr, INET_ADDRSTRLEN) != NULL)
		return addr;
	else
		return "";
}

void Sockaddr::set_addr(const string& addr)
{
	if (addr == "localhost")
		sa.sin_addr.s_addr = INADDR_ANY;
	else
		sa.sin_addr.s_addr = inet_addr(addr.c_str());
}

int Sockaddr::get_port()
{
	return ntohs(sa.sin_port);
}

void Sockaddr::set_port(int port)
{
	sa.sin_port = htons(port);
}

Socket::Socket()
{
	sockfd = -1;
}

Socket::Socket(Socket&& tmpsk)
{
	sockfd = tmpsk.sockfd;
	tmpsk.sockfd = -1;
}

Socket::~Socket()
{
	close();
}

bool Socket::open()
{
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		perror("socket()");
		return false;
	}
	return true;
}

bool Socket::close()
{
	if (::close(sockfd) < 0)
		return false;

	sockfd = -1;
	return true;
}

bool Socket::is_valid()
{
	return sockfd > 0 ? true :false;
}

bool Socket::bind(const Sockaddr& addr)
{
	if (::bind(sockfd, (sockaddr*)&(addr.sa), sizeof(addr.sa)) < 0)
	{
		perror("bind()");
		return false;
	}
	return true;
}

bool Socket::connect(const Sockaddr& addr)
{
	if (::connect(sockfd, (sockaddr*)&(addr.sa), sizeof(addr.sa)) < 0)
	{
		perror("connect()");
		return false;
	}
	return true;
}

bool Socket::connect_retry(const Sockaddr& addr, int maxsleep)
{
	for (int nsec = 1; nsec <= maxsleep; nsec <<= 1)
	{
		if (connect(addr)) return true;
		if (nsec <= maxsleep/2) sleep(nsec);
	}
	return false;
}

bool Socket::listen(int qlen)
{
	if (::listen(sockfd, qlen) < 0)
	{
		perror("listen()");
		return false;
	}
	return true;
}

Socket Socket::accept(Sockaddr* paddr)
{
	sockaddr_in sa;
	socklen_t len = sizeof(sa);

	int clfd = ::accept(sockfd, (sockaddr*)&sa, &len);
	if (clfd < 0)
	{
		perror("accept()");
		return Socket(-1);
	}

	if (paddr != NULL) paddr->sa = sa;
	return Socket(clfd);
}

size_t Socket::send(const void *buf, size_t nbytes, int flags)
{
	size_t n = ::send(sockfd, buf, nbytes, flags);
	if (n < 0)
	{
		perror("send()");
		return -1;
	}
	return n;
}

size_t Socket::recv(void* buf, size_t nbytes, int flags)
{
	size_t n = ::recv(sockfd, buf, nbytes, flags);
	if (n < 0)
	{
		perror("recv()");
		return -1;
	}
	return n;
}











































