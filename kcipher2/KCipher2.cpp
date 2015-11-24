#include <algorithm>
#include <iomanip>
#include <boost/io/ios_state.hpp>
#include "KCipher2.h"
#include "util.h"

template <size_t Size>
static std::ostream& operator<<(std::ostream& os, const std::array<uint32_t, Size>& data)
{
	boost::io::ios_flags_saver fs(os);
	os << std::hex << std::uppercase;
	for (uint32_t i : data)
		os << std::setfill('0') << std::setw(8) << i << " ";
	return os;
}

std::ostream& operator<<(std::ostream& os, const KCipher2& data)
{
	if (!data.current_state_) {
		os << "ik and iv are null" << std::endl;
	}
	else {
		os << "A:" << data.current_state_->reg_a << std::endl;
		os << "B:" << data.current_state_->reg_b << std::endl;
		os << "R:" << data.current_state_->reg_r << std::endl;
		os << "L:" << data.current_state_->reg_l << std::endl;
	}
	return os;
}

KCipher2::State::State(const uint32_t k[], const uint32_t iv[])
{
	reg_a = { k[4], k[3], k[2], k[1], k[0] };
	reg_b = { k[10], k[11], iv[0], iv[1], k[8], k[9], iv[2], iv[3], k[7], k[5], k[6] };
	reg_r = { 0, 0 };
	reg_l = { 0, 0 };
}

uint32_t KCipher2::State::zl() const
{
	return (reg_b[0] + reg_r[1]) ^ reg_r[0] ^ reg_a[4];
}

uint32_t KCipher2::State::zh() const
{
	return (reg_b[10] + reg_l[1]) ^ reg_l[0] ^ reg_a[0];
}

uint32_t KCipher2::sub(uint32_t x)
{
#ifdef USE_KCIPHER2_AES_SBOX
	const uint8_t result[] = {
		sbox[util::int32toq3(x)],
		sbox[util::int32toq2(x)],
		sbox[util::int32toq1(x)],
		sbox[util::int32toq0(x)]
	};
	const uint8_t mul2[] = {
		sbox.mul(result[0], 2),
		sbox.mul(result[1], 2),
		sbox.mul(result[2], 2),
		sbox.mul(result[3], 2)
	};
	return
		util::q3toint32(mul2[0] ^ result[3] ^ result[2] ^ mul2[1] ^ result[1]) ^
		util::q2toint32(mul2[1] ^ result[0] ^ result[3] ^ mul2[2] ^ result[2]) ^
		util::q1toint32(mul2[2] ^ result[1] ^ result[0] ^ mul2[3] ^ result[3]) ^
		util::q0toint32(mul2[3] ^ result[2] ^ result[1] ^ mul2[0] ^ result[0]);
#else
	return table[0][util::int32toq3(x)] ^ table[1][util::int32toq2(x)] ^ table[2][util::int32toq1(x)] ^ table[3][util::int32toq0(x)];
#endif
}

template <bool IsInit>
void KCipher2::next(State *old_state, State *new_state)
{
	std::rotate_copy(old_state->reg_a.begin(), old_state->reg_a.begin() + 1, old_state->reg_a.end(), new_state->reg_a.begin());
	new_state->reg_a[4] =
		old_state->reg_a[3] ^ alpha_mul(0, old_state->reg_a[0]);
	if (IsInit)
		new_state->reg_a[4] ^= old_state->zl();

	std::rotate_copy(old_state->reg_b.begin(), old_state->reg_b.begin() + 1, old_state->reg_b.end(), new_state->reg_b.begin());
	new_state->reg_b[10] =
		alpha_mul((old_state->cl1()) ? 1 : 2, old_state->reg_b[0]) ^
		((old_state->cl2()) ? alpha_mul(3, old_state->reg_b[8]) : old_state->reg_b[8]) ^
		old_state->reg_b[1] ^ old_state->reg_b[6];
	if (IsInit)
		new_state->reg_b[10] ^= old_state->zh();

	new_state->reg_r[0] = sub(old_state->reg_l[1] + old_state->reg_b[9]);
	new_state->reg_r[1] = sub(old_state->reg_r[0]);
	new_state->reg_l[0] = sub(old_state->reg_r[1] + old_state->reg_b[4]);
	new_state->reg_l[1] = sub(old_state->reg_l[0]);
}

void KCipher2::load_initial_key(const InitKey& ik, const InitVec& iv)
{
	load_initial_key(ik.data(), iv.data());
}

void KCipher2::load_initial_key(const uint32_t ik[], const uint32_t iv[])
{
	Key key;

	int i;
	for (i = 0; i < 4; ++i)
		key[i] = ik[i];
	for (; i < 12; ++i) {
		if (i % 4 == 0)
			key[i] = key[i - 4] ^ sub(util::rotl32(key[i - 1], 8)) ^ rcon[i / 4 - 1];
		else
			key[i] = key[i - 4] ^ key[i - 1];
	}

	current_state_.reset(new State(key.data(), iv));
	next_state_.reset(new State());

	for (int i = 0; i < 24; ++i) {
		next<true>(current_state_.get(), next_state_.get());
		current_state_.swap(next_state_);
	}
}

void KCipher2::next()
{
	next<false>(current_state_.get(), next_state_.get());
	current_state_.swap(next_state_);
}

void KCipher2::get_stream(KeyStream & stream)
{
	stream.first = current_state_->zh();
	stream.second = current_state_->zl();
}

KCipher2::KeyStream KCipher2::get_stream()
{
	return KeyStream(current_state_->zh(), current_state_->zl());
}

