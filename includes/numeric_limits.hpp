#ifndef __IEEE754_NUMERIC_LIMITS_H__
#define __IEEE754_NUMERIC_LIMITS_H__

namespace std {
	/**
	 * Specialization of std::numeric_limits for any bit format provided.
	 */
	template <unsigned M, unsigned E, int B >
	struct numeric_limits<IEEE754<M, E, B > > {
		static const bool is_specialized = true;

		static IEEE754<M, E, B > min() { return IEEE754<M, E, B >::fomBits(+1, +1); }
		static IEEE754<M, E, B > max() { return IEEE754<M, E, B >::fromBits(-2, -2); }

		static const int digits = M + 1;
		static const int digits10 = M * M_LOG10E / M_LOG2E;
		static const bool is_signed = true;
		static const bool is_integer = false;
		static const bool is_exact = false;
		static const int radix = 2;

		static IEEE754<M, E, B > epsilon() { return IEEE754<M, E, B >::fromBits(0, B - M); }
		static IEEE754<M, E, B > round_error() { return 0.5f; }

		static const int min_exponent = -B + 2;
		static const int min_exponent10 = min_exponent * M_LOG10E / M_LOG2E;
		static const int max_exponent =  B + 1;
		static const int max_exponent10 = max_exponent * M_LOG10E / M_LOG2E;

		static const bool has_infinity = true;
		static const bool has_quiet_NaN = true;
		static const bool has_signaling_NaN = false;
		static const float_denorm_style has_denorm = denorm_present;
		static const bool has_denorm_loss = false;

		static IEEE754<M, E, B > infinity() { return IEEE754<M, E, B >::fromBits(0, -1); }
		static IEEE754<M, E, B > quiet_NaN() { return IEEE754<M, E, B >::fromBits(1, -1); }
		static IEEE754<M, E, B > signaling_NaN() { return IEEE754<M, E, B >::fromBits(1, -1); }
		static IEEE754<M, E, B > denorm_min() { return IEEE754<M, E, B >::fromBits(1, 0); }

		static const bool is_iec559 = has_infinity && has_quiet_NaN && has_denorm == denorm_present;
		static const bool is_bounded = true;
		static const bool is_modulo = false;

		static const bool traps = false;
		static const bool tinyness_before = false;
		static const float_round_style round_style = round_to_nearest;
	};
}

#endif
