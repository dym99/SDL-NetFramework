#pragma once
#include <cstdio>
#ifdef _DEBUG
	#define DEBUG_LOG(FMT, ...) printf(FMT, __VA_ARGS__)
#else
	#define DEBUG_LOG(FMT, ...) __noop
#endif