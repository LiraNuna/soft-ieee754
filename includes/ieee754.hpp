#ifndef __IEEE754_H__
#define __IEEE754_H__

#include <cmath>
#include <limits>
#include <algorithm>
#include <type_traits>

#include "std_overrides.hpp"
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

		primitive mantissa : M;
		primitive exponent : E;
		primitive sign : 1;

	private:
		/**
		 * Shift that allows negative values
		 */
		template <
			typename T,
			typename RT = typename std::conditional<(sizeof(T) > sizeof(primitive)), T, primitive >::type
		>
		static RT shift(T value, int shift_amount) {
			if(shift_amount < 0)
				return value >> -shift_amount;
			if(shift_amount > 0)
				return value << shift_amount;

			return value;
		}

		/**
		 * Computes the real value of the mantissa.
		 * This adds the implicit 1.xxxx to the mantissa when needed
		 */
		primitive real_mantissa() const {
			return exponent ? mantissa | (1 << M) : mantissa;
		}

		/**
		 * Fills up exponent and mantissa from an unsigned value.
		 * Sign is left unchanged.
		 */
		template <typename T >
		void from_unsigned(T unsigned_value, int radix_point = 0) {
			int log2 = std::log2(unsigned_value);

			if(unsigned_value > ((1 << (M + 1)) - 1) << E) {
				exponent = EXPONENT_MASK;
				mantissa = 0;
			} else {
				exponent = unsigned_value ? log2 + radix_point + B : 0;
				mantissa = shift(unsigned_value, M - log2);
			}
		}

		/**
		 * Fills up sign, exponent and mantissa from an unsigned value.
		 */
		template <typename T >
		void from_signed(T signed_value, int radix_point = 0) {
			typename std::make_signed<T >::type forced_signed = signed_value;

			sign = (forced_signed < 0);
			from_unsigned<T >(std::abs(forced_signed), radix_point);
		}

		/**
		 * Retrieve the value of this float as an unsigned value
		 */
		template <typename T >
		T to_unsigned(int radix_point = 0) const {
			return shift<T >(real_mantissa(), exponent - radix_point - B - M);
		}

		/**
		 * Retrieve the value of this float as a signed value
		 */
		template <typename T >
		T to_signed(int radix_point = 0) const {
			return (to_unsigned<T >(radix_point) ^ -sign) + sign;
		}

	public:
		// -------------------------- Constructors -------------------------- //

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
			sign = other_ieee754.sign;
			exponent = other_ieee754.exponent ? other_ieee754.exponent - OB + B : 0;
			mantissa = shift(other_ieee754.mantissa, M - OM);
		}

		/**
		 * Conversion from a floating point value
		 */
		template <
			typename T,
			typename = typename std::enable_if<std::is_floating_point<T >::value, T >::type
		>
		IEEE754(T floating_point) {
			sign = std::signbit(floating_point);

			if(std::isnormal(floating_point)) {
				int exp = 0;
				int man = (std::frexp(floating_point, &exp) + 0.5) * (1 << (M + 1));

				exp += B - 1;
				if(exp < EXPONENT_MASK) {
					exponent = exp;
					mantissa = man;
				} else {
					exponent = EXPONENT_MASK;
					mantissa = 0;
				}
			} else {
				exponent = floating_point == T() ? 0 : EXPONENT_MASK;
				mantissa = std::isnan(floating_point);
			}
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
			from_signed(signed_integral);
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

		// ------------------------- Cast operators ------------------------- //

		/**
		 * Convert to another floating point value
		 */
		template <
			typename T,
			typename = typename std::enable_if<std::is_floating_point<T >::value, T >::type
		>
		operator T() const {
			T result;
			if(exponent != EXPONENT_MASK) {
				result = std::ldexp(real_mantissa() / T(1 << M), exponent - B);
			} else {
				if(mantissa)
					result = std::numeric_limits<T >::quiet_NaN();
				else
					result = std::numeric_limits<T >::infinity();
			}

			return std::copysign(result, -sign);
		}

		/**
		 * Convert to a signed integer
		 */
		template <
			typename T,
			typename = typename std::enable_if<!std::is_floating_point<T >::value, T >::type,
			typename = typename std::enable_if< std::is_signed<T >::value, T >::type
		>
		operator T() const {
			return to_signed<T >();
		}

		/**
		 * Convert to an unsigned integer
		 */
		template <
			typename T,
			typename = typename std::enable_if<!std::is_floating_point<T >::value, T >::type,
			typename = typename std::enable_if<!std::is_signed<T >::value, T >::type,
			typename = typename std::enable_if< std::is_unsigned<T >::value, T >::type
		>
		operator T() const {
			return to_unsigned<T >();
		}
};


#endif
