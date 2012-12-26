#ifndef __IEEE754_H__
#define __IEEE754_H__

#include <cmath>
#include <limits>
#include <type_traits>

#include <iostream> // XXX: Not including this causes GCC to freak out about template substitutions.

#include "smallest_unsigned.hpp"

template<unsigned M, unsigned E, int B = (1 << (E - 1)) - 1 >
class IEEE754 {
	// XXX: This actually needs to be private, however I do not know how to make
	// The functions at std:: overrides able to access it without anything else does
	// (friending everything is very tedious and you cannot friend a namespace)
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

	private:
		/**
		 * Adjust previously calculated mantissa to new mantissa
		 */
		template <typename T >
		primitive adjusted_shift(T value, int shift_amount) {
			shift_amount -= M;

			if(shift_amount < 0)
				return value << -shift_amount;

			if(shift_amount > 0)
				return value >> shift_amount;

			return value;
		}

		/**
		 * Fills up exponent and mantissa from an unsigned value.
		 * Sign is left unchanged.
		 */
		template <typename T >
		void from_unsigned(T unsigned_value) {
			int log2 = std::log2(unsigned_value);

			exponent = unsigned_value ? B + log2 : 0;
			mantissa = adjusted_shift(unsigned_value, log2);
		}

	public:
		/**
		 * Default constructor, undefined value.
		 */
		IEEE754() = default;

		/**
		 * Default copy constructor
		 */
		IEEE754(const IEEE754 &) = default;

		/**
		 * Conversion from another IEEE floating point object
		 */
		template <unsigned OM, unsigned OE, int OB >
		IEEE754(const IEEE754<OM, OE, OB > &other_ieee754) {
			exponent = other_ieee754.exponent - OB + B;
			mantissa = adjusted_shift(other_ieee754.mantissa, OM);
		}

		/**
		 * Conversion from a floating point value
		 */
		template <
			typename T,
			typename = typename std::enable_if<std::is_floating_point<T >::value, T >::type
		>
		IEEE754(T floating_point) {
			int exp = 0;

			mantissa = (std::frexp(floating_point, &exp) + 0.5) * (1 << (M + 1));
			exponent = exp + B - 1;
		}

		/**
		 * Conversion from a signed integral type
		 */
		template <
			typename T,
			typename = typename std::enable_if<!std::is_floating_point<T >::value, T >::type,
			typename = typename std::enable_if< std::is_signed<T >::value, T >::type
		>
		IEEE754(T signed_integral) {
			sign = (signed_integral < 0);
			from_unsigned(std::abs(signed_integral));
		}

		/**
		 * Conversion from an unsigned integral type
		 */
		template <
			typename T,
			typename = typename std::enable_if<!std::is_floating_point<T >::value, T >::type,
			typename = typename std::enable_if<!std::is_signed<T >::value, T >::type,
			typename = typename std::enable_if< std::is_unsigned<T >::value, T >::type
		>
		IEEE754(T unsigned_integral) {
			sign = 0;
			from_unsigned(unsigned_integral);
		}

		/**
		 * Convert floating point to any arithmetic value.
		 */
		template <
			typename T,
			typename = typename std::enable_if<std::is_arithmetic<T >::value, T >::type
		>
		operator T() const {
			T result = 0;

			if(mantissa || exponent) {
				if(exponent == EXPONENT_MASK) {
					if(mantissa == 0)
						return std::numeric_limits<T >::infinity();

					return std::numeric_limits<T >::quiet_NaN();
				} else {
					// Denormal
					if(exponent == 0)
						result = (0 + mantissa / std::pow(2.0, M)) * std::pow(2.0, exponent - B + 1);
					else
						result = (1 + mantissa / std::pow(2.0, M)) * std::pow(2.0, exponent - B + 0);
				}
			}

			if(sign)
				result = -result;

			return result;
		}
};

#include "std_overrides.hpp"

#endif
