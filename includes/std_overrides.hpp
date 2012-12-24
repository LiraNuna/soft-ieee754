#ifndef __IEEE754_STD_OVERRIDES_H__
#define __IEEE754_STD_OVERRIDES_H__

namespace std {
	/**
	 * Specialization of std::numeric_limits for any bit format provided.
	 */
	template <unsigned M, unsigned E, int B >
	struct numeric_limits<IEEE754<M, E, B > > {
		public:
			static const bool is_specialized = true;

			static IEEE754<M, E, B > min()			{ return from_components(+1, +1); }
			static IEEE754<M, E, B > max()			{ return from_components(-2, -2); }

			static const int digits = M + 1;
			static const int digits10 = M * M_LOG10E / M_LOG2E;
			static const bool is_signed = true;
			static const bool is_integer = false;
			static const bool is_exact = false;
			static const int radix = 2;

			static IEEE754<M, E, B > epsilon()		{ return from_components(B - M, 0); }
			static IEEE754<M, E, B > round_error()	{ return 0.5f; }

			static const int min_exponent = -B + 2;
			static const int max_exponent =  B + 1;
			static const int min_exponent10 = min_exponent * M_LOG10E / M_LOG2E;
			static const int max_exponent10 = max_exponent * M_LOG10E / M_LOG2E;

			static const bool has_infinity = true;
			static const bool has_quiet_NaN = true;
			static const bool has_signaling_NaN = false;
			static const float_denorm_style has_denorm = denorm_present;
			static const bool has_denorm_loss = false;

			static IEEE754<M, E, B > infinity()		{ return from_components(-1,  0); }
			static IEEE754<M, E, B > quiet_NaN()	{ return from_components(-1, +1); }
			static IEEE754<M, E, B > signaling_NaN(){ return from_components(-1, +1); }
			static IEEE754<M, E, B > denorm_min()	{ return from_components( 0, +1); }

			static const bool is_iec559 = has_infinity && has_quiet_NaN && has_denorm == denorm_present;
			static const bool is_bounded = true;
			static const bool is_modulo = false;

			static const bool traps = false;
			static const bool tinyness_before = false;
			static const float_round_style round_style = round_to_nearest;

		private:
			inline static IEEE754<M, E, B > from_components(typename IEEE754<M, E, B >::primitive exponent, typename IEEE754<M, E, B >::primitive mantissa) {
				IEEE754<M, E, B > result;
				result.mantissa	= mantissa;
				result.exponent	= exponent;
				result.sign = 0;
				return result;
			}
	};

	// --------------------------- Classification --------------------------- //

	/*
	 * Categorizes the given floating point value
	 */
	template <unsigned M, unsigned E, int B >
	int fpclassify(const IEEE754<M, E, B > &arg) {
		// Zero exponent can be either zero or a subnormal (denormal)
		if(arg.exponent == 0) {
			if(arg.mantissa == 0)
				return FP_ZERO;

			return FP_SUBNORMAL;
		}

		// Exponent of mask can be NaN or Infinite
		if(arg.exponent == IEEE754<M, E, B >::EXPONENT_MASK) {
			if(arg.mantissa == 0)
				return FP_INFINITE;

			return FP_NAN;
		}

		// Any other value is considered a normal value
		return FP_NORMAL;
	}

	/*
	 * Checks if the given number has finite value
	 */
	template <unsigned M, unsigned E, int B >
	bool isfinite(const IEEE754<M, E, B > &arg) {
		return (arg.exponent != IEEE754<M, E, B >::EXPONENT_MASK);
	}

	/*
	 * Checks if the given number is infinite
	 */
	template <unsigned M, unsigned E, int B >
	bool isinf(const IEEE754<M, E, B > &arg) {
		return (arg.exponent == IEEE754<M, E, B >::EXPONENT_MASK)
			&& arg.mantissa == 0;
	}

	/**
	 * Checks if the given number is NaN
	 */
	template <unsigned M, unsigned E, int B >
	bool isnan(const IEEE754<M, E, B > &arg) {
		return (arg.exponent == IEEE754<M, E, B >::EXPONENT_MASK)
			&& arg.mantissa != 0;
	}

	/*
	 * Checks if the given number is normal
	 */
	template <unsigned M, unsigned E, int B >
	bool isnormal(const IEEE754<M, E, B > &arg) {
		return (arg.exponent != 0)
			|| (arg.exponent != IEEE754<M, E, B >::EXPONENT_MASK);
	}

	/*
	 * Checks if the given number is negative
	 */
	template <unsigned M, unsigned E, int B >
	bool signbit(const IEEE754<M, E, B > &arg) {
		return arg.sign != 0;
	}

	/**
	 * Checks if two floating-point values are unordered
	 */
	template <unsigned M, unsigned E, int B >
	bool isunordered(const IEEE754<M, E, B > &arg1, const IEEE754<M, E, B > &arg2) {
		return isnan(arg1)
			|| isnan(arg2);
	}
}

#endif
