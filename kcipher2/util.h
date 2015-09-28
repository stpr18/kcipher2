#pragma once
#include <array>
#include "builtin.h"

namespace util {
	inline uint8_t int32toq0(uint32_t x)
	{
		return x >> 24;
	}

	inline uint8_t int32toq1(uint32_t x)
	{
		return (x >> 16) & 0xff;
	}

	inline uint8_t int32toq2(uint32_t x)
	{
		return (x >> 8) & 0xff;
	}

	inline uint8_t int32toq3(uint32_t x)
	{
		return x & 0xff;
	}

	inline uint32_t q0toint32(uint8_t x)
	{
		return x << 24;
	}

	inline uint32_t q1toint32(uint8_t x)
	{
		return x << 16;
	}

	inline uint32_t q2toint32(uint8_t x)
	{
		return x << 8;
	}

	inline uint32_t q3toint32(uint8_t x)
	{
		return x;
	}
}

