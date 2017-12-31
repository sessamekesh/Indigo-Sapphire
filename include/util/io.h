#pragma once

#include <future>
#include <string>

namespace util
{
	std::future<std::string> readFileAsync(const char* path);}