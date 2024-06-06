#include "../../version.h"
#if (defined(RL_TOOLS_DISABLE_INCLUDE_GUARDS) || !defined(RL_TOOLS_RL_ENVIRONMENTS_ENVIRONMENTS_H)) && (RL_TOOLS_USE_THIS_VERSION == 1)
#pragma once
#define RL_TOOLS_RL_ENVIRONMENTS_ENVIRONMENTS_H

RL_TOOLS_NAMESPACE_WRAPPER_START
namespace rl_tools::rl::environments{
    struct Environment{

    };
    struct DummyUI{};
}
RL_TOOLS_NAMESPACE_WRAPPER_END

#include "acrobot/acrobot.h"
#include "car/car.h"
//#include "mujoco/mujoco.h" // mujoco is not generic so needs to be included separately
#include "pendulum/pendulum.h"

#endif