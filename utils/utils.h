/*
 * utils.h
 *
 *  Created on: 2014-8-27
 *      Author: tiancai
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <sys/stat.h>
#include <string>
#include <vector>
#include <tuple>
#include <utility>

namespace os
{

class OSError : public std::exception
{
public:
	explicit OSError(const std::string& information) : info(information) { }
	virtual ~OSError() noexcept { }
	virtual const char* what() const noexcept { return info.c_str(); }
protected:
	std::string info;
};

std::string strerror(const std::string& s = "");

enum ListType{LIST_ALL, LIST_FILE, LIST_DIR};
std::vector<std::string> listdir(
		const std::string& path = ".", ListType type = LIST_ALL) throw(OSError);

void mkdir(const std::string& path, mode_t mode = S_IRWXU) throw(OSError);

void makedirs(const std::string& path, mode_t mode = S_IRWXU, bool exists_ok = true) throw(OSError);

void rmdir(const std::string& path) throw(OSError);

void removedirs(const std::string& path) throw(OSError);

void chdir(const std::string& path) throw(OSError);

void remove(const std::string& path) throw(OSError);

void rename(const std::string& oldname, const std::string& newname) throw(OSError);

std::string getcwd() throw(OSError);

namespace path
{

bool isfile(const std::string& path) noexcept;

bool isdir(const std::string& path) noexcept;

bool isabs(const std::string& path) noexcept;

std::string join(const std::string& path) noexcept;

template<typename... Args>
std::string join(const std::string& path1,
		const std::string& path2, Args&&... args) noexcept
{
	if (path1.empty() || isabs(path2)) return join(path2, std::forward<Args>(args)...);
	if (path1.back() == '/')
		return join(path1+path2, std::forward<Args>(args)...);
	else
		return join(path1+'/'+path2, std::forward<Args>(args)...);
}

std::tuple<std::string, std::string> split(const std::string& path) noexcept;

std::tuple<std::string, std::string> splitext(const std::string& path) noexcept;

std::string normpath(const std::string& path) noexcept;

std::string dirname(const std::string& path) noexcept;

std::string basename(const std::string& path) noexcept;

bool exists(const std::string& path) noexcept;

}
}



#endif /* UTILS_H_ */
