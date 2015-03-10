/*
 * bin_feat.h
 *
 *  Created on: 2014-9-1
 *      Author: tiancai
 */

#ifndef BIN_FEAT_H_
#define BIN_FEAT_H_

#include <array>
#include <functional>
#include <iostream>

template <size_t N>
class BinFeat
{
public:
	BinFeat()
	{
		for (size_t i = 0; i < bytes(); ++i)
			_data[i] = 0;
	}

	size_t size() const noexcept { return N; }
	size_t bytes() const noexcept { return N/8; }
	const char* data() const noexcept { return _data; }
	char* data() noexcept { return _data; }

	bool operator==(const BinFeat<N>& feat) const noexcept
	{
		for (size_t i = 0; i < bytes(); ++i)
		{
			if (_data[i] != feat._data[i])
				return false;
		}
		return true;
	}

	bool operator!=(const BinFeat<N>& feat) const noexcept
	{
		return !operator==(feat);
	}

	BinFeat<N>& operator&=(const BinFeat<N>& other) noexcept
	{
		for (size_t i = 0; i < bytes(); ++i)
		{
			_data[i] &= other._data[i];
		}
		return *this;
	}

	BinFeat<N>& operator|=(const BinFeat<N>& other) noexcept
	{
		for (size_t i = 0; i < bytes(); ++i)
		{
			_data[i] |= other._data[i];
		}
		return *this;
	}

	BinFeat<N>& operator^=(const BinFeat<N>& other) noexcept
	{
		for (size_t i = 0; i < bytes(); ++i)
		{
			_data[i] ^= other._data[i];
		}
		return *this;
	}

	BinFeat<N> operator~() const noexcept
	{
		BinFeat<N> res;
		for (size_t i = 0; i < bytes(); ++i)
		{
			res._data[i] = ~_data[i] ;
		}
		return res;
	}

	size_t count() const noexcept
	{
		const char table [256] =
		{
		0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
		1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
		1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
		2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
		1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
		2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
		2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
		3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
		1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
		2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
		2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
		3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
		2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
		3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
		3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
		4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8
		};
		size_t sum = 0;
		for (size_t i = 0; i < bytes(); ++i)
			sum += table[(unsigned char)(_data[i])];
		return sum;
	}

private:
	char _data[N/8];
};

template<size_t N>
size_t hamming_distance(const BinFeat<N>& lhs, const BinFeat<N>& rhs) noexcept
{
	return (lhs ^ rhs).count();
}

template<size_t N>
BinFeat<N> operator|(const BinFeat<N>& lhs, const BinFeat<N>& rhs) noexcept
{
	BinFeat<N> res;
	for (size_t i = 0; i < lhs.bytes(); ++i)
		res.data()[i] = lhs.data()[i] | rhs.data()[i];
	return res;
}

template<size_t N>
BinFeat<N> operator&(const BinFeat<N>& lhs, const BinFeat<N>& rhs) noexcept
{
	BinFeat<N> res;
	for (size_t i = 0; i < lhs.bytes(); ++i)
		res.data()[i] = lhs.data()[i] & rhs.data()[i];
	return res;
}

template<size_t N>
BinFeat<N> operator^(const BinFeat<N>& lhs, const BinFeat<N>& rhs) noexcept
{
	BinFeat<N> res;
	for (size_t i = 0; i < lhs.bytes(); ++i)
		res.data()[i] = lhs.data()[i] ^ rhs.data()[i];
	return res;
}

template<size_t N>
std::ostream& operator<<(std::ostream& output, const BinFeat<N>& feat)
{
	output.write(feat.data(), feat.bytes());
	return output;
}

template<size_t N>
std::istream& operator>>(std::istream& input, BinFeat<N>& feat)
{
	input.read(feat.data(), feat.bytes());
	return input;
}

namespace std
{
template<size_t N>
struct hash<BinFeat<N>> : public __hash_base<size_t, BinFeat<N>>
{
	size_t operator()(const BinFeat<N>& _val) const noexcept
	{
		return std::_Hash_impl::hash(_val.data(), _val.bytes());
	}
};
}
#endif /* BIN_FEAT_H_ */
