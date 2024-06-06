#include "../../../../version.h"
#if (defined(RL_TOOLS_DISABLE_INCLUDE_GUARDS) || !defined(RL_TOOLS_RL_LOOP_STEPS_SAVE_TRAJECTORIES_CONFIG_H)) && (RL_TOOLS_USE_THIS_VERSION == 1)
#pragma once
#define RL_TOOLS_RL_LOOP_STEPS_SAVE_TRAJECTORIES_CONFIG_H

#include "../../../../rl/components/off_policy_runner/operations_generic.h"

#include "../../../../rl/environments/environments.h"

#include "state.h"

RL_TOOLS_NAMESPACE_WRAPPER_START
namespace rl_tools::rl::loop::steps::save_trajectories{
    struct ParametersTag{};
    template <typename T, typename TI, typename NEXT>
    struct Parameters{
        using TAG = ParametersTag;
        static constexpr bool DETERMINISTIC_EVALUATION = true;
        static constexpr TI INTERVAL = 1000;
        static constexpr TI NUM_EPISODES = 10;
        static constexpr TI EPISODE_STEP_LIMIT = NEXT::CORE_PARAMETERS::EPISODE_STEP_LIMIT;
        static constexpr bool SAVE_TRAJECTORIES = true;
    };
    struct ConfigTag{};
    template<typename T_NEXT, typename T_PARAMETERS = Parameters<typename T_NEXT::T, typename T_NEXT::TI, T_NEXT>, typename T_UI = environments::DummyUI>
    struct Config: T_NEXT {
        using TAG = ConfigTag;
        using NEXT = T_NEXT;
        using SAVE_TRAJECTORIES_PARAMETERS = T_PARAMETERS;
        using UI = T_UI;
        using T = typename NEXT::T;
        using TI = typename NEXT::TI;
        using SAVE_TRAJECTORIES_SPEC = rl::utils::evaluation::Specification<T, TI, typename NEXT::ENVIRONMENT_EVALUATION, SAVE_TRAJECTORIES_PARAMETERS::NUM_EPISODES, SAVE_TRAJECTORIES_PARAMETERS::EPISODE_STEP_LIMIT>;
        template <typename CONFIG>
        using State = State<CONFIG>;
    };
}
RL_TOOLS_NAMESPACE_WRAPPER_END
#endif




