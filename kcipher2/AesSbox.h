#pragma once
#include <array>
#include <cstdint>
#include "util.h"
#include "Gf256.h"

class AesSbox
{
private:
	using AesGf256 = Gf256 < (0x80 | 0x1b) >;
	const AesGf256 gf_;
	std::array<uint8_t, 256> sbox_;
	//std::array<uint8_t, 256> inv_sbox_;

public:

	AesSbox()
	{
		uint8_t x = 0;
		uint8_t value;
		do {
			value = at(x);
			sbox_[x] = value;
			//inv_sbox_[value] = x;
		} while (++x != 0);
	}

	~AesSbox() = default;

	uint8_t at(uint8_t x) const
	{
		uint8_t sft;
		x = sft = gf_.inv(x);
		for (int i = 0; i < 4; ++i) {
			sft = util::rotl8(sft, 1);
			x ^= sft;
		}
		x ^= 0x63; // 0b01100011 
		return x;
	}

	uint8_t operator[](uint8_t x) const
	{
		return sbox_[x];
	}

	//uint8_t inv(uint8_t x) const
	//{
	//	return inv_sbox_[x];
	//}

	uint8_t rcon(uint8_t x) const
	{
		if (x == 0)
			return 0;
		uint8_t c = 1;
		for (; x != 1; --x)
			c = gf_.mul(c, 2);
		return c;
	}

	uint8_t mul(uint8_t a, uint8_t b) const
	{
		return gf_.mul(a, b);
	}

	uint8_t inv(uint8_t a) const
	{
		return gf_.inv(a);
	}
};
