#ifndef __IEEE754_H__
#define __IEEE754_H__

#include <cmath>
#include <limits>

#include "smallest_unsigned.hpp"

template<unsigned M, unsigned E, int B = (1 << (E - 1)) - 1 >
struct IEEE754 {
	private:
		enum {
			MANTISSA_MASK = (1UL << M) - 1,
			EXPONENT_MASK = (1UL << E) - 1,

			BITS = 1 + E + M,
		};

		typedef typename smallest_unsigned<BITS >::type primitive;

		primitive mantissa : M;
		primitive exponent : E;
		primitive sign : 1;

	public:
		static IEEE754<M, E, B > fromBits(const primitive &bytes) {
			IEEE754<M, E, B > result;
			result.mantissa	= (bytes >> 0) & MANTISSA_MASK;
			result.exponent	= (bytes >> M) & EXPONENT_MASK;
			result.sign = (bytes >> (M + E)) & 1;
			return result;
		}

		static IEEE754<M, E, B > fromBits(const primitive &mantissa, const primitive &exponent) {
			IEEE754<M, E, B > result;
			result.mantissa	= mantissa;
			result.exponent	= exponent;
			result.sign = 0;
			return result;
		}

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
						result = (0 + mantissa / std::pow(2, M)) * std::pow(2, exponent - B + 1);
					else
						result = (1 + mantissa / std::pow(2, M)) * std::pow(2, exponent - B + 0);
				}
			}

			if(sign)
				result *= -1;

			return result;
		}
};

#include "numeric_limits.hpp"

#endif
