/*
 * reorder_dataset.cpp
 *
 *  Created on: 2014-8-27
 *      Author: tiancai
 */
#include <cstdio>
#include <iostream>
#include <cmath>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include "utils.h"

using namespace std;
using namespace os;
using namespace os::path;

int MAX_FILES_PER_DIR = 10000;
int FILE_NAME_LEN;
int DIR_NAME_LEN = 5;
bool APPEND = false;

string srcdir;
string desdir;
vector<string> names;

int get_num_len(unsigned int num)
{
	int len = 1;
	while (num /= 10)
		len++;
	return len;
}

string format_num_name(unsigned int num, unsigned int len)
{
	string res = to_string(num);
	return string(len - res.size(), '0') + res;
}

bool init_args(int argc, char** argv)
{
	string helpinfo = "usage : reorder_dataset -s srcdir -d desdir [-n maxfile -l dirnamelen]";
	int ch;
	while ((ch = getopt(argc, argv, "s:d:n:l:a")) != -1)
	{
		switch (ch)
		{
		case 's':
			srcdir = optarg;
			break;
		case 'd':
			desdir = optarg;
			break;
		case 'n':
			MAX_FILES_PER_DIR = atoi(optarg);
			break;
		case 'l':
			DIR_NAME_LEN = atoi(optarg);
			break;
		case 'a':
			APPEND = true;
			cout << "Notice : when -a is used, -n and -l is decided automatically" << endl;
			break;
		default:
			cout << helpinfo << endl;
			return false;
		}
	}

	if (srcdir == "" || desdir == "")
	{
		cout << helpinfo << endl;
		return false;
	}

	srcdir = normpath(srcdir);
	desdir = normpath(desdir);

	if (!exists(srcdir)) return false;
	names = listdir(srcdir, LIST_FILE);

	if (APPEND)
	{
		if (!exists(desdir)) return false;
		vector<string> res = listdir(desdir, LIST_DIR);
		if (!res.empty())
		{
			DIR_NAME_LEN = res[0].size();
			res = listdir(join(desdir, res[0]), LIST_FILE);
			if (!res.empty())
			{
				FILE_NAME_LEN = res[0].find_first_of('.');
				MAX_FILES_PER_DIR = pow(10, FILE_NAME_LEN);
			}
		}
	}
	FILE_NAME_LEN = get_num_len(MAX_FILES_PER_DIR-1);
	return true;
}

void move_files(int fnum, int dnum)
{
	string curdir = join(desdir, format_num_name(dnum, DIR_NAME_LEN));
	for (size_t i = 0; i < names.size(); ++i, ++fnum)
	{
		if (fnum == MAX_FILES_PER_DIR)
		{
			fnum = 0;
			dnum++;
		}
		if (fnum == 0)
		{
			curdir = join(desdir, format_num_name(dnum, DIR_NAME_LEN));
			if (!exists(curdir))
				mkdir(curdir);
		}
		string srcname = join(srcdir, names[i]);
		string desname = join(curdir, format_num_name(fnum, FILE_NAME_LEN)+get<1>(splitext(names[i])));
		rename(srcname, desname);
	}
}

void find_fdnum(int& fnum, int& dnum)
{
	fnum = 0; dnum = 0;
	vector<string> res = listdir(desdir, LIST_DIR);
	if (res.empty()) return;
	for (size_t i = 0; i < res.size(); ++i)
		dnum = max(dnum, atoi(res[i].c_str()));

	string curdir = join(desdir, format_num_name(dnum, DIR_NAME_LEN));
	res = listdir(curdir, LIST_FILE);
	fnum = res.size();
}

int main(int argc, char** argv)
{
	if (!init_args(argc, argv)) return 1;

	int fnum, dnum;
	if (APPEND)
		find_fdnum(fnum, dnum);

	move_files(fnum, dnum);
	return 0;

}
