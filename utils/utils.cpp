/*
 * utils.cpp
 *
 *  Created on: 2014-8-27
 *      Author: tiancai
 */

#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <stack>

#include "utils.h"

using namespace std;

namespace os
{

string strerror(const string& s)
{
	if (s != "")
		return s + " : " + ::strerror(errno);
	else
		return ::strerror(errno);
}

vector<string> listdir(const string& path, ListType type) throw(OSError)
{
	vector<string> res;
	DIR* dp = ::opendir(path.c_str());
	if (dp == NULL)
		throw OSError(strerror(path));

	dirent* pitem;
	while ((pitem = ::readdir(dp)) != NULL)
	{
		if ((type == LIST_ALL) || (type == LIST_FILE && pitem->d_type == DT_REG)
				|| (type == LIST_DIR && pitem->d_type == DT_DIR))
			if (pitem->d_name[0] != '.')
				res.push_back(pitem->d_name);
	}
	::closedir(dp);
	return res;
}

void mkdir(const string& path, mode_t mode) throw(OSError)
{
	if (::mkdir(path.c_str(), mode) < 0)
		throw OSError(strerror(path));
}

void makedirs(const string& path, mode_t mode, bool exists_ok) throw(OSError)
{
	if (path == "") return;
	if (path::exists(path))
	{
		if (exists_ok) return;
		else throw OSError(path + " already exists!");
	}
	auto npath = path::normpath(path);
	makedirs(path::dirname(npath), mode);
	mkdir(npath, mode);
}

void rmdir(const string& path) throw(OSError)
{
	if (::rmdir(path.c_str()))
		throw OSError(strerror(path));
}

void removedirs(const string& path) throw(OSError)
{
	if (path == "") return;
	auto npath = path::normpath(path);
	rmdir(npath);
	removedirs(path::dirname(npath));
}

void chdir(const string& path) throw(OSError)
{
	if (::chdir(path.c_str()) < 0)
		throw OSError(strerror(path));
}

void remove(const string& path) throw(OSError)
{
	if (::remove(path.c_str()) < 0)
		throw OSError(strerror(path));
}

void rename(const string& oldname, const string& newname) throw(OSError)
{
	if (::rename(oldname.c_str(), newname.c_str()) < 0)
		throw OSError(strerror(oldname));
}

string getcwd() throw(OSError)
{
	char buf[PATH_MAX];
	if (::getcwd(buf, PATH_MAX) == NULL)
		throw OSError(strerror());
	return buf;
}

namespace path
{

bool isfile(const string& path) noexcept
{
	struct stat buf;
	if (::stat(path.c_str(), &buf) < 0)
		return false;
	return S_ISREG(buf.st_mode);
}

bool isdir(const string& path) noexcept
{
	struct stat buf;
	if (::stat(path.c_str(), &buf) < 0)
		return false;
	return S_ISDIR(buf.st_mode);
}

bool isabs(const string& path) noexcept
{
	if (path.empty()) return false;
	return path[0] == '/';
}

string join(const string& path) noexcept
{
	return path;
}

tuple<string, string> split(const string& path) noexcept
{
	size_t pos = path.find_last_of('/');
	if (pos == string::npos)
		return make_tuple("", path);
	if (pos == 0)
		return make_tuple("/", path.substr(pos+1));
	return make_tuple(path.substr(0, pos), path.substr(pos+1));
}

tuple<string, string> splitext(const string& path) noexcept
{
	size_t pos = path.find_last_of('.');
	if (pos == string::npos || pos == 0 || path[pos-1] == '/')
		return make_tuple(path, "");
	return make_tuple(path.substr(0, pos), path.substr(pos));
}

string normpath(const string& path) noexcept
{
	if (path.empty()) return path;
	vector<string> items;
	size_t pos = 0;
	while (pos < path.size())
	{
		while (pos < path.size() && path[pos] == '/') ++pos;
		int begin = pos;
		while (pos < path.size() && path[pos] != '/') ++pos;
		int end = pos;
		string item = path.substr(begin, end - begin);
		if (item == "." || item == "")
			continue;
		if (item == "..")
			items.pop_back();
		else
			items.push_back(item);
	}
	string res;
	for (size_t i = 0; i < items.size(); ++i)
		res += '/' + items[i];
	if (res.empty()) res = "/";

	if (path[0] == '/')
		return res;
	else
		return res.substr(1);
}

string dirname(const string& path) noexcept
{
	return get<0>(split(path));
}

string basename(const string& path) noexcept
{
	return get<1>(split(path));
}

string abspath(const string& path) noexcept
{
	return normpath(join(getcwd(), path));
}

bool exists(const string& path) noexcept
{
	struct stat buf;
	if (::stat(path.c_str(), &buf) < 0)
		return false;
	return true;
}

}
}









