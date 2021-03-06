#pragma once

#include <array>
#include <cstddef>
#include <type_traits>

#include "config.hpp"

namespace synthizer {

/*
 * Basic sample type.
 */
using AudioSample = float;

/*
 * vector of 4 audio samples, which can alias like char does.
 */
typedef AudioSample AudioSample4 __attribute__((may_alias, vector_size(16), aligned(16)));

/*
 * Some types, i.e. SIMD extensions, may not have a simple way to be zero initialized.
 * In particular, Clang and GCC don't document the interaction with C++ initializer lists.
 * Since the only operation that varies is the zeroing operation for most of this library,we deal with that here.
 */
template<typename T>
T zeroValue();

template<>
inline float zeroValue<float>() { return 0.0f; }
template<>
inline double zeroValue<double>() { return 0.0; }
template<>
inline AudioSample4 zeroValue<AudioSample4>() { return { 0, 0, 0, 0 }; }


/*
 * Some basic SFINAE helpers.
 */
template<unsigned int n, typename T=void>
using PowerOfTwo = std::enable_if_t<( ( n & (n-1) ) == 0 && n != 0), T>;
template<unsigned int n, unsigned int mul, typename T=void>
using MultipleOf = typename std::enable_if_t< (n%mul == 0 && n != 0), T>;
// We really need Clang to get concepts, but until then this combines requirements.
template<typename T, typename... T2>
class AllHelper {
	public:
	using type = T;
};

template<typename T, typename... ARGS>
using All = typename AllHelper<T, ARGS...>::type;

/*
 * This is terribly, terribly, terribly slow; use only at compile time.
 * */
constexpr unsigned int nextPowerOfTwo(unsigned int input) {
	unsigned int ret = 1;
	while(ret < input) ret <<= 1;
	return ret;
}

constexpr unsigned int nextMultipleOf(unsigned int value, unsigned int multiplier) {
	if (value % multiplier == 0) return value;
	else return (value / multiplier + 1) * multiplier;
}

}
