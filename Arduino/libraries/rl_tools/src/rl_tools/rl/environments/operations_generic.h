#include "../../version.h"
#if (defined(RL_TOOLS_DISABLE_INCLUDE_GUARDS) || !defined(RL_TOOLS_RL_ENVIRONMENTS_OPERATIONS_GENERIC_H)) && (RL_TOOLS_USE_THIS_VERSION == 1)
#pragma once
#define RL_TOOLS_RL_ENVIRONMENTS_OPERATIONS_GENERIC_H
//#include "pendulum/operations_generic.h"
#include "environments.h"


RL_TOOLS_NAMESPACE_WRAPPER_START
namespace rl_tools{
    template <typename DEVICE>
    void malloc(DEVICE&, rl::environments::Environment&){};
    template <typename DEVICE>
    void init(DEVICE&, rl::environments::Environment&){};
    template <typename DEVICE, typename ENV>
    void init(DEVICE&, ENV&, rl::environments::DummyUI){};
    template <typename DEVICE, typename ENV>
    void render(DEVICE&, ENV&, rl::environments::DummyUI){};
    template <typename DEVICE, typename ENV, typename STATE>
    void set_state(DEVICE&, ENV&, rl::environments::DummyUI, STATE&){};
    template <typename DEVICE, typename ENV, typename STATE, typename ACTION>
    void set_state(DEVICE&, ENV&, rl::environments::DummyUI, STATE&, ACTION&){};
    template <typename DEVICE, typename ENV, typename ACTION>
    void set_action(DEVICE&, ENV&, rl::environments::DummyUI, ACTION&){};
    template <typename DEVICE, typename ENV>
    auto get_ui(DEVICE&, ENV&){return "";}
    template <typename DEVICE, typename ENV, typename STATE>
    auto serialize_json(DEVICE&, ENV&, STATE&){return "{}";};
    template <typename DEVICE>
    void free(DEVICE&, rl::environments::Environment&){};
}
RL_TOOLS_NAMESPACE_WRAPPER_END
#endif