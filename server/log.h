#pragma once

#include <string>

namespace Log
{
	void error(const char *__fmt, ...);
	void warning(const char *__fmt, ...);
	void info(const char *__fmt, ...);

    extern std::string logPath;
}

