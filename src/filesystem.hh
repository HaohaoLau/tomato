#pragma once

#include <boost/filesystem.hpp>
#include "config.hh"

namespace fs = boost::filesystem;

/// Get the current executable name with path. Returns empty path if the name
/// cannot be found. May return absolute or relative paths.
#if defined(_WIN32)
#include <windows.h>
fs::path inline execname() {
	char buf[1024];
	DWORD ret = GetModuleFileName(NULL, buf, sizeof(buf));
	if (ret == 0 || ret == sizeof(buf)) return std::string();
	return buf;
}
#elif defined(__APPLE__)
#include <mach-o/dyld.h>
fs::path inline execname() {
	char buf[1024];
	uint32_t size = sizeof(buf);
	int ret = _NSGetExecutablePath(buf, &size);
	if (ret != 0) return fs::path();
	return buf;
}
#elif defined(sun) || defined(__sun)
#include <stdlib.h>
fs::path inline execname() {
	return getexecname();
}
#elif defined(__FreeBSD__)
#include <sys/sysctl.h>
fs::path inline execname() {
	int mib[4];
	mib[0] = CTL_KERN;
	mib[1] = KERN_PROC;
	mib[2] = KERN_PROC_PATHNAME;
	mib[3] = -1;
	char buf[1024];
	size_t maxchars = sizeof(buf) - 1;
	size_t size = maxchars;
	sysctl(mib, 4, buf, &size, NULL, 0);
	if (size == 0 || size >= maxchars) return fs::path();
	buf[size] = '\0';
	return buf;
}
#elif defined(__linux__)
#include <unistd.h>
fs::path inline execname() {
	char buf[1024];
	ssize_t maxchars = sizeof(buf) - 1;
	ssize_t size = readlink("/proc/self/exe", buf, sizeof(buf));
	if (size <= 0 || size >= maxchars) return fs::path();
	buf[size] = '\0';
	return buf;
}
#else
fs::path inline execname() {
	return fs::path();
}
#endif


/// Get a file from share data path
std::string inline getFilePath(fs::path file) {
	return (execname().parent_path().parent_path() / SHARED_DATA_DIR / file).string();
}
