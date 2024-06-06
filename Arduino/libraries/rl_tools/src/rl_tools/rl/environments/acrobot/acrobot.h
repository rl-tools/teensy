#include "../../../version.h"
#if (defined(RL_TOOLS_DISABLE_INCLUDE_GUARDS) || !defined(RL_TOOLS_RL_ENVIRONMENTS_ACROBOT_ACROBOT_H)) && (RL_TOOLS_USE_THIS_VERSION == 1)
#pragma once
#define RL_TOOLS_RL_ENVIRONMENTS_ACROBOT_ACROBOT_H

#include "../environments.h"

RL_TOOLS_NAMESPACE_WRAPPER_START
namespace rl_tools::rl::environments::acrobot {
    template <typename T>
    struct DefaultParameters {
        static constexpr T DT = 0.2;
        static constexpr T LINK_LENGTH_1 = 1.0;  // [m]
        static constexpr T LINK_LENGTH_2 = 1.0;  // [m]
        static constexpr T LINK_MASS_1 = 1.0;  //: [kg] mass of link 1
        static constexpr T LINK_MASS_2 = 1.0;  //: [kg] mass of link 2
        static constexpr T LINK_COM_POS_1 = 0.5;  //: [m] position of the center of mass of link 1
        static constexpr T LINK_COM_POS_2 = 0.5;  //: [m] position of the center of mass of link 2
        static constexpr T LINK_MOI = 1.0;  //: moments of inertia for both links
        static constexpr T MAX_VEL_1 = 4 * math::PI<T>;
        static constexpr T MAX_VEL_2 = 9 * math::PI<T>;
        static constexpr T MIN_TORQUE = -1;
        static constexpr T MAX_TORQUE = +1;
        static constexpr T TORQUE_NOISE_MAX = 0.0;
        static constexpr T SCREEN_DIM = 500;
        static constexpr T VEL_PENALTY = 0.2;
    };
    template <typename T>
    struct EasyParameters: DefaultParameters<T> {
        static constexpr T min_torque = -10;
        static constexpr T max_torque = +10;
    };
    template <typename T_T, typename T_TI, typename T_PARAMETERS = DefaultParameters<T_T>>
    struct Specification{
        using T = T_T;
        using TI = T_TI;
        using PARAMETERS = T_PARAMETERS;
    };

    template <typename T, typename TI>
    struct State{
        static constexpr TI DIM = 4;
        T theta_0;
        T theta_1;
        T theta_0_dot;
        T theta_1_dot;
    };

}
RL_TOOLS_NAMESPACE_WRAPPER_END

RL_TOOLS_NAMESPACE_WRAPPER_START
namespace rl_tools::rl::environments{
    template <typename T_SPEC>
    struct Acrobot: Environment{
        using SPEC = T_SPEC;
        using T = typename SPEC::T;
        using TI = typename SPEC::TI;
        using State = acrobot::State<T, TI>;
        using PARAMETERS = typename SPEC::PARAMETERS;
        static constexpr TI OBSERVATION_DIM = 6;
        static constexpr TI OBSERVATION_DIM_PRIVILEGED = OBSERVATION_DIM;
        static constexpr TI ACTION_DIM = 1;
    };
    template <typename T_SPEC>
    struct AcrobotSwingup: Acrobot<T_SPEC>{};
}
RL_TOOLS_NAMESPACE_WRAPPER_END







#endif
