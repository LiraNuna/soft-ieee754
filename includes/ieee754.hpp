#ifndef __IEEE754_H__
#define __IEEE754_H__

#include <cmath>
#include <limits>

#include "smallest_unsigned.hpp"

template<unsigned M, unsigned E, int B = (1 << (E - 1)) - 1 >
class IEEE754 {
	// XXX: This actually needs to be private, however I do not know how to make
	// The functions at std:: overrides able to access it without anything else does
	public:
		enum {
			MANTISSA_MASK = (1UL << M) - 1,
			EXPONENT_MASK = (1UL << E) - 1,

			BITS = 1 + E + M,
		};

		typedef typename smallest_unsigned<BITS >::type primitive;

		union {
			struct {
				primitive mantissa : M;
				primitive exponent : E;
				primitive sign : 1;
			};

			primitive all;
		};

	public:
		IEEE754() { }

		IEEE754(float f) {
			// Must have at least 32bits to hold a float,
			// but we need to expand for larger types
			typename smallest_unsigned<(M > 32) ? M : 32 >::type i = *(uint32_t*)&f;
			int e = (i >> 23) & 0xFF;

			sign = i >> 31;

			if(e == 0xFF)
				exponent = EXPONENT_MASK;
			else
				exponent =  e + B - 127;

			if(M > 23)
				mantissa = (i & 0x7FFFFF) << std::max<unsigned >(0, M - 23);
			else
				mantissa = (i & 0x7FFFFF) >> std::max<unsigned >(0, 23 - M);
		}

		template <typename T>
		operator T() const {
			T result = 0;

			if(mantissa || exponent) {
				if(exponent == EXPONENT_MASK) {
					if(mantissa != 0)
						return std::numeric_limits<T >::quiet_NaN();
					else
						return std::numeric_limits<T >::infinity();
				} else {
					// Denormal
					if(exponent == 0)
						result = (0 + mantissa / std::pow(2, M)) * std::pow(2.0, exponent - B + 1);
					else
						result = (1 + mantissa / std::pow(2, M)) * std::pow(2.0, exponent - B + 0);
				}
			}

			if(sign)
				result *= -1;

			return result;
		}
};

#include "std_overrides.hpp"

#endif
