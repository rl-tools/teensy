#include "../version.h"
#if (defined(RL_TOOLS_DISABLE_INCLUDE_GUARDS) || !defined(RL_TOOLS_UTILS_RANDOM_OPERATIONS_ARM_H)) && (RL_TOOLS_USE_THIS_VERSION == 1)
#pragma once
#define RL_TOOLS_UTILS_RANDOM_OPERATIONS_ARM_H


#include "../utils/generic/typing.h"
#include "operations_generic.h"

RL_TOOLS_NAMESPACE_WRAPPER_START
namespace rl_tools::random{
    devices::random::ARM::index_t default_engine(const devices::random::ARM& dev, devices::random::ARM::index_t seed = 1){
        return default_engine(devices::random::Generic<devices::math::ARM>{}, seed);
    };
    constexpr devices::random::ARM::index_t next_max(const devices::random::ARM& dev){
        return devices::random::ARM::MAX_INDEX;
    }
    template <typename TI, typename RNG>
    auto split(const devices::random::ARM& dev, TI split_id, RNG& rng){
        // this operation should not alter the state of rng
        return split(devices::random::Generic<devices::math::ARM>{}, split_id, rng);
    }
    template<typename RNG>
    void next(const devices::random::ARM& dev, RNG& rng){
        next(devices::random::Generic<devices::math::ARM>{}, rng);
    }

    template<typename T, typename RNG>
    T uniform_int_distribution(const devices::random::ARM& dev, T low, T high, RNG& rng){
        return uniform_int_distribution(devices::random::Generic<devices::math::ARM>{}, low, high, rng);
    }
    template<typename T, typename RNG>
    T uniform_real_distribution(const devices::random::ARM& dev, T low, T high, RNG& rng){
        return uniform_real_distribution(devices::random::Generic<devices::math::ARM>{}, low, high, rng);
    }
    namespace normal_distribution{
        template<typename T, typename RNG>
        T sample(const devices::random::ARM& dev, T mean, T std, RNG& rng){
            return sample(devices::random::Generic<devices::math::ARM>{}, mean, std, rng);
        }
    }
}
RL_TOOLS_NAMESPACE_WRAPPER_END

#endif
