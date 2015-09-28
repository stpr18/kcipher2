#pragma once
#include <cstdint>
#include <array>
#include <memory>
#include <utility>
#include <iostream>
#include "AesSbox.h"

#define USE_KCIPHER2_AES_SBOX

class KCipher2
{
	friend std::ostream& operator<<(std::ostream& os, const KCipher2& data);

	// --- static member ---
public:
	using InitKey = std::array<uint32_t, 4>;
	using InitVec = std::array<uint32_t, 4>;
	using KeyStream = std::pair<uint32_t, uint32_t>;

private:
	using Rcon = std::array<uint32_t, 2>;
	using Amul = std::array<uint32_t, 256>;
	using Ttable = std::array<uint32_t, 256>;

	using Key = std::array<uint32_t, 12>;
	using RegA = std::array<uint32_t, 5>;
	using RegB = std::array<uint32_t, 11>;
	using Reg = std::array<uint32_t, 2>;

	static const Rcon rcon;
	static const Amul amul[4];
#ifdef USE_KCIPHER2_AES_SBOX
	static const AesSbox sbox;
#else
	static const Ttable table[4];
#endif

	struct State {
		RegA reg_a;
		RegB reg_b;
		Reg reg_r;
		Reg reg_l;

		State() = default;
		State(const uint32_t k[], const uint32_t iv[]);
		~State() = default;

		uint32_t zl() const;
		uint32_t zh() const;

		bool cl1() const
		{
			return (reg_a[2] & 0x40000000) != 0;
		}

		bool cl2() const
		{
			return (reg_a[2] & 0x80000000) != 0;
		}
	};
	
	static uint32_t alpha_mul(size_t i, uint32_t x)
	{
		return (x << 8) ^ amul[i][x >> 24];
	}

	static uint32_t sub(uint32_t x);

	template <bool IsInit>
	static void next(State *old_state, State *new_state);

	// ------
	std::unique_ptr<State> current_state_;
	std::unique_ptr<State> next_state_;

public:
	KCipher2() = default;
	KCipher2(const InitKey ik, const InitVec iv)
	{
		load_initial_key(ik, iv);
	}
	KCipher2(const uint32_t ik[], const uint32_t iv[])
	{
		load_initial_key(ik, iv);
	}

	~KCipher2() = default;

	void load_initial_key(const InitKey ik, const InitVec iv);
	void load_initial_key(const uint32_t ik[], const uint32_t iv[]);
	void next();
	void get_stream(KeyStream & stream);
	KeyStream get_stream();

};

std::ostream& operator<<(std::ostream& os, const KCipher2& data);
