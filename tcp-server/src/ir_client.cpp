/*
 * ir_client.cpp
 *
 *  Created on: 2014-8-25
 *      Author: tiancai
 */
#include "stdafx.h"
#include "ir_comm.h"
#include "ir_client.h"

using namespace std;
using namespace cv;



int main(int argc, char** argv)
{
	Socket ck;
	ck.open();
	Sockaddr addr("127.0.0.1", 8000);
	if (!ck.connect_retry(addr, 30))
	{
		cerr << "connect failed!" << endl;
	}

	Mat im = imread("/home/tiancai/dataset/ASD/Imgs/0_2_2580.jpg");
	namedWindow("client window check");
	imshow("client window check", im);

	if (im.empty()) cout << "imread error!" << endl;
	vector<uchar> buf;
	imencode(".jpg", im, buf);

	uint32_t ss = buf.size();
	ss = htonl(ss);

	if (ck.send(&ss, sizeof(ss)) != sizeof(ss))
		cerr << "send failed" << endl;
	if (ck.send(buf.data(), buf.size()) != buf.size())
		cerr << "send data failed" << endl;


	return 0;
}
