#ifndef __SMALLEST_UNSIGNED_H__
#define __SMALLEST_UNSIGNED_H__

#include <stdint.h>

template<unsigned bits >
struct smallest_unsigned {
	typedef typename smallest_unsigned<bits + 1>::type type;
};

template<>
struct smallest_unsigned<8 > {
	typedef uint8_t type;
};

template<>
struct smallest_unsigned<16 > {
	typedef uint16_t type;
};

template<>
struct smallest_unsigned<32 > {
	typedef uint32_t type;
};

template<>
struct smallest_unsigned<64 > {
	typedef uint64_t type;
};

#endif
