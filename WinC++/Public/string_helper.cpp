#pragma once
#include "string_helper.h"
#include <string>
#include <stdarg.h>
#include <memory>
std::string format(char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	int size = _vsnprintf(nullptr, 0, fmt, args) + 1;
	std::unique_ptr<char> buf(new char[size]);
	size = _vsnprintf(buf.get(), size, fmt, args);
	va_end(args);
	return buf.get();
}