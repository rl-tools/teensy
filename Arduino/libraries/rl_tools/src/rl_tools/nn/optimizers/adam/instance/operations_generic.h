#include "../../../../version.h"
#if (defined(RL_TOOLS_DISABLE_INCLUDE_GUARDS) || !defined(RL_TOOLS_NN_OPTIMIZERS_ADAM_INSTANCE_OPERATIONS_GENERIC_H)) && (RL_TOOLS_USE_THIS_VERSION == 1)
#pragma once
#define RL_TOOLS_NN_OPTIMIZERS_ADAM_INSTANCE_OPERATIONS_GENERIC_H

#include "../adam.h"
#include "../../../../nn/layers/dense/layer.h"
#include "../../../../nn/parameters/operations_generic.h"
#include "../../../../utils/polyak/operations_generic.h"

RL_TOOLS_NAMESPACE_WRAPPER_START
namespace rl_tools{
    template <typename DEVICE, typename SPEC>
    void malloc(DEVICE& device, nn::parameters::Adam::instance<SPEC>& p){
        malloc(device, (nn::parameters::Gradient::instance<SPEC>&) p);
        malloc(device, p.gradient_first_order_moment);
        malloc(device, p.gradient_second_order_moment);
    }
    template <typename DEVICE, typename SPEC>
    void free(DEVICE& device, nn::parameters::Adam::instance<SPEC>& p){
        free(device, (nn::parameters::Gradient::instance<SPEC>&) p);
        free(device, p.gradient_first_order_moment);
        free(device, p.gradient_second_order_moment);
    }
    template<typename DEVICE, typename SPEC, typename PARAMETERS>
    void update(DEVICE& device, nn::parameters::Adam::instance<SPEC>& parameter, nn::optimizers::Adam<PARAMETERS>& optimizer) {
        utils::polyak::update(device, parameter.gradient, parameter.gradient_first_order_moment, optimizer.parameters.beta_1);
        utils::polyak::update_squared(device, parameter.gradient, parameter.gradient_second_order_moment, optimizer.parameters.beta_2);
        gradient_descent(device, parameter, optimizer);
    }

    template<typename DEVICE, typename SPEC, typename PARAMETER_SPEC>
    void gradient_descent(DEVICE& device, nn::parameters::Adam::instance<PARAMETER_SPEC>& parameter, nn::optimizers::Adam<SPEC>& optimizer){
        using TI = typename DEVICE::index_t;
        using T = typename PARAMETER_SPEC::CONTAINER::T;
        for(TI row_i = 0; row_i < PARAMETER_SPEC::CONTAINER::ROWS; row_i++) {
            for(TI col_i = 0; col_i < PARAMETER_SPEC::CONTAINER::COLS; col_i++) {
                T pre_sqrt_term = get(parameter.gradient_second_order_moment, row_i, col_i) * optimizer.second_order_moment_bias_correction;
                pre_sqrt_term = math::max(device.math, pre_sqrt_term, (T)optimizer.parameters.epsilon_sqrt);
                T parameter_update = optimizer.parameters.alpha * optimizer.first_order_moment_bias_correction * get(parameter.gradient_first_order_moment, row_i, col_i) / (math::sqrt(device.math, pre_sqrt_term) + optimizer.parameters.epsilon);
                if constexpr(utils::typing::is_same_v<typename PARAMETER_SPEC::CATEGORY_TAG, nn::parameters::categories::Biases> && SPEC::ENABLE_BIAS_LR_FACTOR){
                    parameter_update *= optimizer.parameters.bias_lr_factor;
                }
                if constexpr(utils::typing::is_same_v<typename PARAMETER_SPEC::CATEGORY_TAG, nn::parameters::categories::Weights>){
                    if constexpr(utils::typing::is_same_v<typename PARAMETER_SPEC::GROUP_TAG, nn::parameters::groups::Normal> && SPEC::ENABLE_WEIGHT_DECAY){
                        parameter_update += get(parameter.parameters, row_i, col_i) * optimizer.parameters.weight_decay / 2;
                    }
                    if constexpr(utils::typing::is_same_v<typename PARAMETER_SPEC::GROUP_TAG, nn::parameters::groups::Input> && SPEC::ENABLE_WEIGHT_DECAY){
                        parameter_update += get(parameter.parameters, row_i, col_i) * optimizer.parameters.weight_decay_input / 2;
                    }
                    if constexpr(utils::typing::is_same_v<typename PARAMETER_SPEC::GROUP_TAG, nn::parameters::groups::Output> && SPEC::ENABLE_WEIGHT_DECAY){
                        parameter_update += get(parameter.parameters, row_i, col_i) * optimizer.parameters.weight_decay_output / 2;
                    }
                }
                increment(parameter.parameters, row_i, col_i, -parameter_update);
            }
        }
    }
    template<typename DEVICE, typename SPEC, typename PARAMETER_SPEC>
    void _reset_optimizer_state(DEVICE& device, nn::parameters::Adam::instance<PARAMETER_SPEC>& parameter, nn::optimizers::Adam<SPEC>& optimizer){
        set_all(device, parameter.gradient_first_order_moment, 0);
        set_all(device, parameter.gradient_second_order_moment, 0);
    }

    template<typename SOURCE_DEVICE, typename TARGET_DEVICE, typename SOURCE_SPEC, typename TARGET_SPEC>
    void copy(SOURCE_DEVICE& source_device, TARGET_DEVICE& target_device, const  nn::parameters::Adam::instance<SOURCE_SPEC>& source, nn::parameters::Adam::instance<TARGET_SPEC>& target){
        copy(source_device, target_device, (nn::parameters::Gradient::instance<SOURCE_SPEC>&) source, (nn::parameters::Gradient::instance<TARGET_SPEC>&) target);
        copy(source_device, target_device, source.gradient_first_order_moment , target.gradient_first_order_moment);
        copy(source_device, target_device, source.gradient_second_order_moment, target.gradient_second_order_moment);
    }
    template<typename DEVICE, typename SPEC_1, typename SPEC_2>
    typename SPEC_1::T abs_diff(DEVICE& device, const nn::parameters::Adam::instance<SPEC_1>& p1, const nn::parameters::Adam::instance<SPEC_2>& p2){
        typename SPEC_1::T acc = 0;
        acc += abs_diff(device, (nn::parameters::Gradient::instance<SPEC_1>&) p1, (nn::parameters::Gradient::instance<SPEC_2>&) p2);
        acc += abs_diff(device, p1.gradient_first_order_moment, p2.gradient_first_order_moment);
        acc += abs_diff(device, p1.gradient_second_order_moment, p2.gradient_second_order_moment);
        return acc;
    }
}
RL_TOOLS_NAMESPACE_WRAPPER_END

#endif