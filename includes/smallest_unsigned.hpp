#ifndef __SMALLEST_UNSIGNED_H__
#define __SMALLEST_UNSIGNED_H__

#if defined(_MSC_VER) && _MSC_VER < 1600
typedef __int8 int8_t;
typedef __int16 int16_t;
typedef __int32 int32_t;
typedef __int64 int64_t;

typedef unsigned __int8 uint8_t;
typedef unsigned __int16 uint16_t;
typedef unsigned __int32 uint32_t;
typedef unsigned __int64 uint64_t;
#else
#include <stdint.h>
#endif

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
