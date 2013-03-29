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
		typedef typename std::make_signed<primitive >::type signed_primitive;

		primitive mantissa : M;
		primitive exponent : E;
		primitive sign : 1;

	private:
		/**
		 * Build a float from components
		 */
		inline static IEEE754 from_components(primitive sign, primitive exponent, primitive mantissa) {
			IEEE754 result;
			result.sign = sign;
			result.exponent	= exponent;
			result.mantissa	= mantissa;
			return result;
		}

		template<typename T >
		inline static IEEE754 renormalize(T unnormalized, int radix_point) {
			IEEE754 result;
			result.from_signed(unnormalized, radix_point);
			return result;
		}

		/**
		 * Returns nan with optional sign and mantissa
		 */
		inline static IEEE754 nan(primitive sign = 0, primitive mantissa = 1) {
			return from_components(sign, EXPONENT_MASK, mantissa);
		}

		/**
		 * Returns Infinity
		 */
		inline static IEEE754 inf(primitive sign = 0) {
			return from_components(sign, EXPONENT_MASK, 0);
		}

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

			if(unsigned_value > shift<T >((1 << (M + 1)) - 1, E - radix_point)) {
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
				int man = std::abs(std::frexp(floating_point, &exp) + 0.5) * (1 << (M + 1));

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

		// --------------------------- Arithmetic --------------------------- //

		// Unary

		friend IEEE754 operator + (const IEEE754 &value) {
			return value;
		}

		friend IEEE754 operator - (const IEEE754 &value) {
			return from_components(
				value.sign ^ 1,
				value.exponent, value.mantissa);
		}

		// Binary

		friend IEEE754 operator + (const IEEE754 &lhs, const IEEE754 &rhs) {
			if(std::isunordered(lhs, rhs))
				return nan();

			int exp = std::min(lhs.exponent - B, rhs.exponent - B) - M;
			return renormalize(
				lhs.to_signed<signed_primitive >(exp) +
				rhs.to_signed<signed_primitive >(exp), exp);
		}

		friend IEEE754 operator - (const IEEE754 &lhs, const IEEE754 &rhs) {
			if(std::isunordered(lhs, rhs))
				return nan();

			int exp = std::min(lhs.exponent - B, rhs.exponent - B) - M;
			return renormalize(
				lhs.to_signed<signed_primitive >(exp) -
				rhs.to_signed<signed_primitive >(exp), exp);
		}

		friend IEEE754 operator * (const IEEE754 &lhs, const IEEE754 &rhs) {
			if(std::isunordered(lhs, rhs))
				return nan();

			primitive sign = lhs.sign ^ rhs.sign;
			primitive exponent = lhs.exponent + rhs.exponent - B;
			primitive product  = (lhs.real_mantissa() * rhs.real_mantissa()) >> M;

			primitive overflow = (product >> M) > 1;

			exponent += overflow;
			if(exponent >= EXPONENT_MASK)
				return inf(sign);

			return from_components(sign, exponent, product >> overflow);
		}

		friend IEEE754 operator / (const IEEE754 &lhs, const IEEE754 &rhs) {
			if(std::isunordered(lhs, rhs))
				return nan();

			primitive sign = lhs.sign ^ rhs.sign;
			primitive exponent = lhs.exponent - rhs.exponent + B;
			primitive quotient = (lhs.real_mantissa() << M) / rhs.real_mantissa();

			primitive underflow = (quotient >> M) < 1;
			exponent -= underflow;
			if(exponent >= EXPONENT_MASK)
				return nan(sign);

			return from_components(sign, exponent, quotient << underflow);
		}

		// Placement

		IEEE754& operator += (const IEEE754 &value) {
			return *this = *this + value;
		}

		IEEE754& operator -= (const IEEE754 &value) {
			return *this = *this - value;
		}

		IEEE754& operator *= (const IEEE754 &value) {
			return *this = *this * value;
		}

		IEEE754& operator /= (const IEEE754 &value) {
			return *this = *this / value;
		}

		// --------------------------- Comparison --------------------------- //

		friend bool operator == (const IEEE754 &lhs, const IEEE754 &rhs) {
			if(std::isunordered(lhs, rhs))
				return false;

			if(lhs.exponent == 0 && lhs.mantissa == 0
			&& rhs.exponent == 0 && rhs.mantissa == 0)
				return true;

			return (primitive&)lhs == (primitive&)rhs;
		}

		friend bool operator != (const IEEE754 &lhs, const IEEE754 &rhs) {
			if(std::isunordered(lhs, rhs))
				return false;

			if(lhs.exponent == 0 && lhs.mantissa == 0
			&& rhs.exponent == 0 && rhs.mantissa == 0)
				return false;

			return (primitive&)lhs != (primitive&)rhs;
		}
};

#endif
