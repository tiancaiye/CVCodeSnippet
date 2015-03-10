/*
 * srv_main.cpp
 *
 *  Created on: 2014-8-25
 *      Author: tiancai
 */
#include "stdafx.h"
#include "ir_server.h"

using namespace std;
using namespace cv;

void service(Socket csock)
{
	uint32_t clen;
	if (csock.recv(&clen, sizeof(clen), MSG_WAITALL) != sizeof(clen))
	{
		cerr << "recv msg head error!" << endl;
		return;
	}
	clen = ntohl(clen);

	vector<char> buf(clen);
	if (csock.recv(buf.data(), clen, MSG_WAITALL) != clen)
	{
		cerr << "recv img content error!" << endl;
		return;
	}
	Mat im = imdecode(buf, CV_LOAD_IMAGE_COLOR);
	//namedWindow("Server");
	//imshow("Server", im);
	//waitKey();
	cout << "service end!" << endl;
	while(1) ;
}

int main(int argc, char** argv)
{
	Sockaddr addr("localhost", 8000);
 	Server srv;
 	srv.register_service(addr, service);
 	if (srv.init())
 	{
 		srv.serve();
 	}
	return 0;
}
