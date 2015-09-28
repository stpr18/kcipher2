#pragma once
#include <cstdint>
#ifdef _MSC_VER
#include <intrin.h>
#endif

#ifdef _MSC_VER
#pragma intrinsic(_rotl8, _rotr8, _rotl, _rotr)
#endif

namespace util
{
	inline uint8_t rotl8(uint8_t value, unsigned char shift) {
#ifdef _MSC_VER
		return _rotl8(value, shift);
#else
		return (value << shift) | (value >> (8 - shift));
#endif
	}

	inline uint8_t rotr8(uint8_t value, unsigned char shift) {
#ifdef _MSC_VER
		return _rotr8(value, shift);
#else
		return (value >> shift) | (value << (8 - shift));
#endif
	}

	inline uint32_t rotl32(uint32_t value, unsigned char shift) {
#ifdef _MSC_VER
		return _rotl(value, shift);
#else
		return (value << shift) | (value >> (32 - shift));
#endif
	}

	inline uint32_t rotr32(uint32_t value, unsigned char shift) {
#ifdef _MSC_VER
		return _rotr(value, shift);
#else
		return (value >> shift) | (value << (32 - shift));
#endif
	}
}
