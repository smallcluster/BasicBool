#pragma once

#include <string>

using std::string;

#include "logger.hpp"

#if defined(_WIN32)
#define PLATFORM_WINDOWS 1
#elif defined(__linux__)
#define PLATFORM_LINUX 1
#endif

#define MULTI_CORES 0