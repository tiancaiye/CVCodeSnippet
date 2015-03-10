/*
 * ir_comm.h
 *
 *  Created on: 2014-8-25
 *      Author: tiancai
 */

#ifndef IR_COMM_HPP_
#define IR_COMM_HPP_

class Sockaddr
{
public:
	friend class Socket;
public:
	Sockaddr();
	Sockaddr(const std::string& addr, int port);

	std::string get_addr();
	int get_port();

	void set_addr(const std::string& addr);
	void set_port(int port);

protected:
	void init_sa();

public:
	sockaddr_in sa;
};

class Socket
{
public:
	Socket();
	Socket(Socket&& tmpsk);
	Socket(int fd) { sockfd = fd; }
	~Socket();

	bool open();
	bool close();
	bool is_valid();

	bool bind(const Sockaddr& addr);
	bool connect(const Sockaddr& addr);
	bool connect_retry(const Sockaddr& addr, int maxsleep = 60);
	bool listen(int qlen = 128);
	Socket accept(Sockaddr* paddr = NULL);
	size_t send(const void* buf, size_t nbytes, int flags = 0);
	size_t recv(void* buf, size_t nbytes, int flags = 0);

private:
	Socket(const Socket&) = delete;
	const Socket& operator=(const Socket&) = delete;

private:
	int sockfd;
};




#endif /* IR_COMM_HPP_ */
