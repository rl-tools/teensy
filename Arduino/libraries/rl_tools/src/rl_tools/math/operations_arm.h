#include "../version.h"
#if (defined(RL_TOOLS_DISABLE_INCLUDE_GUARDS) || !defined(RL_TOOLS_MATH_OPERATIONS_ARM_H)) && (RL_TOOLS_USE_THIS_VERSION == 1)
#pragma once
#define RL_TOOLS_MATH_OPERATIONS_ARM_H

#include "operations_generic.h"

#include "../devices/arm.h"

#include <cmath>
//#include <algorithm>


RL_TOOLS_NAMESPACE_WRAPPER_START
namespace rl_tools::math {

    template<typename T>
    T sqrt(const devices::math::ARM&, const T x) {
        return std::sqrt(x);
    }
    template<typename T>
    T tanh(const devices::math::ARM&, const T x) {
        return std::tanh(x);
    }
    template<typename T>
    T exp(const devices::math::ARM&, const T x) {
        return std::exp(x);
    }
    template<typename T>
    T sin(const devices::math::ARM&, const T x) {
        return std::sin(x);
    }
    template<typename T>
    T cos(const devices::math::ARM&, const T x) {
        return std::cos(x);
    }
    template<typename T>
    T acos(const devices::math::ARM&, const T x) {
        return std::acos(x);
    }
    template<typename TX, typename TY>
    auto pow(const devices::math::ARM&, const TX x, const TY y) {
        return std::pow(x, y);
    }
    template<typename T>
    auto log(const devices::math::ARM&, const T x) {
        return std::log(x);
    }
    template<typename T>
    auto floor(const devices::math::ARM&, const T x) {
        return std::floor(x);
    }
    template<typename T>
    auto is_nan(const devices::math::ARM&, const T x) {
        return isnan(x);
    }
    template<typename T>
    auto is_finite(const devices::math::ARM&, const T x) {
        return std::isfinite(x);
    }
    template<typename T>
    T clamp(const devices::math::ARM&, T x, T min, T max){
//        return std::clamp(x, min, max);
        return x < min ? min : (x > max ? max : x);
    }
    template<typename T>
    T min(const devices::math::ARM&, T x, T y){
//        return std::min(x, y);
        return x < y ? x : y;
    }
    template<typename T>
    T max(const devices::math::ARM&, T x, T y){
//        return std::max(x, y);
        return x > y ? x : y;
    }
    template<typename T>
    T abs(const devices::math::ARM&, T x){
        return std::abs(x);
    }
    template<typename T>
    T nan(const devices::math::ARM&){
        return std::numeric_limits<T>::quiet_NaN();
    }
    template<typename T>
    T fast_tanh(const devices::math::ARM& dev, T x) {
        x = clamp(dev, x, -(T)3.0f, (T)3.0);
        T x_squared = x * x;
        return x * (27 + x_squared) / (27 + 9 * x_squared);
    }
    template<typename T>
    T atan2(const devices::math::ARM& dev, T a, T b) {
        return std::atan2(a, b);
    }
}
RL_TOOLS_NAMESPACE_WRAPPER_END
#endif
