#include "../../../../../version.h"
#if (defined(RL_TOOLS_DISABLE_INCLUDE_GUARDS) || !defined(RL_TOOLS_RL_ALGORITHMS_SAC_LOOP_CORE_OPERATIONS_GENERIC_H)) && (RL_TOOLS_USE_THIS_VERSION == 1)
#pragma once
#define RL_TOOLS_RL_ALGORITHMS_SAC_LOOP_CORE_OPERATIONS_GENERIC_H

#include "../../../../../nn/optimizers/adam/instance/operations_generic.h"
#include "../../../../../nn/layers/sample_and_squash/operations_generic.h"
#include "../../../../../nn_models/mlp/operations_generic.h"
#include "../../../../../nn_models/sequential/operations_generic.h"
#include "../../../../../nn_models/random_uniform/operations_generic.h"
#include "../../../../../rl/algorithms/sac/operations_generic.h"
#include "../../../../../nn/optimizers/adam/operations_generic.h"
#include "../../../../../rl/components/off_policy_runner/operations_generic.h"

#include "config.h"

RL_TOOLS_NAMESPACE_WRAPPER_START
namespace rl_tools{
    template <typename DEVICE, typename T_CONFIG>
    void malloc(DEVICE& device, rl::algorithms::sac::loop::core::State<T_CONFIG>& ts, typename T_CONFIG::TI seed = 0){
        malloc(device, ts.actor_critic);
        malloc(device, ts.off_policy_runner);
        malloc(device, ts.critic_batch);
        malloc(device, ts.critic_training_buffers[0]);
        malloc(device, ts.critic_training_buffers[1]);
        malloc(device, ts.action_noise_critic);
        malloc(device, ts.critic_buffer);
        malloc(device, ts.actor_batch);
        malloc(device, ts.actor_training_buffers);
        malloc(device, ts.action_noise_actor);
        malloc(device, ts.actor_buffer_eval);
        malloc(device, ts.actor_buffer);
        for(auto& env: ts.envs){
            rl_tools::malloc(device, env);
        }
        ts.allocated = true;
    }
    template <typename DEVICE, typename T_CONFIG>
    void init(DEVICE& device, rl::algorithms::sac::loop::core::State<T_CONFIG>& ts, typename T_CONFIG::TI seed = 0){
        utils::assert_exit(device, ts.allocated, "State not allocated");
        using CONFIG = T_CONFIG;
        using T = typename CONFIG::T;
        using TI = typename DEVICE::index_t;

        ts.rng = random::default_engine(typename DEVICE::SPEC::RANDOM{}, seed);

        init(device, ts.actor_critic, ts.rng);

        for(TI env_i = 0; env_i < CONFIG::CORE_PARAMETERS::N_ENVIRONMENTS; env_i ++){
            rl_tools::init(device, ts.envs[env_i], ts.env_parameters[env_i]);
        }

        init(device, ts.off_policy_runner, ts.envs, ts.env_parameters);

        ts.step = 0;
    }
    template <typename DEVICE_SOURCE, typename DEVICE_TARGET, typename T_CONFIG_SOURCE, typename T_CONFIG_TARGET>
    void copy(DEVICE_SOURCE& device_source, DEVICE_TARGET& device_target, rl::algorithms::sac::loop::core::State<T_CONFIG_SOURCE>& source, rl::algorithms::sac::loop::core::State<T_CONFIG_TARGET>& target){
        copy(device_source, device_target, source.actor_critic, target.actor_critic);
        copy(device_source, device_target, source.off_policy_runner, target.off_policy_runner);
        copy(device_source, device_target, source.critic_batch, target.critic_batch);
        copy(device_source, device_target, source.critic_training_buffers[0], target.critic_training_buffers[0]);
        copy(device_source, device_target, source.critic_training_buffers[1], target.critic_training_buffers[1]);
        copy(device_source, device_target, source.critic_buffer, target.critic_buffer);
        copy(device_source, device_target, source.actor_batch, target.actor_batch);
        copy(device_source, device_target, source.actor_training_buffers, target.actor_training_buffers);
        copy(device_source, device_target, source.actor_buffer_eval, target.actor_buffer_eval);
        copy(device_source, device_target, source.actor_buffer, target.actor_buffer);
//        target.rng = source.rng;
        target.off_policy_runner.parameters.exploration_noise = source.off_policy_runner.parameters.exploration_noise;
        target.step = source.step;
    }

    template <typename DEVICE, typename T_CONFIG>
    void free(DEVICE& device, rl::algorithms::sac::loop::core::State<T_CONFIG>& ts){
        free(device, ts.actor_critic);
        free(device, ts.off_policy_runner);
        free(device, ts.critic_batch);
        free(device, ts.critic_training_buffers[0]);
        free(device, ts.critic_training_buffers[1]);
        free(device, ts.action_noise_critic);
        free(device, ts.critic_buffer);
        free(device, ts.actor_batch);
        free(device, ts.actor_training_buffers);
        free(device, ts.action_noise_actor);
        free(device, ts.actor_buffer_eval);
        free(device, ts.actor_buffer);
        for(auto& env: ts.envs){
            rl_tools::free(device, env);
        }
    }

    template <typename DEVICE, typename T_CONFIG>
    bool step(DEVICE& device, rl::algorithms::sac::loop::core::State<T_CONFIG>& ts){
        using CONFIG = T_CONFIG;
        set_step(device, device.logger, ts.step);
        bool finished = false;
        if(ts.step >= CONFIG::CORE_PARAMETERS::N_WARMUP_STEPS){
            step<1>(device, ts.off_policy_runner, get_actor(ts), ts.actor_buffer_eval, ts.rng);
        }
        else{
            typename CONFIG::EXPLORATION_POLICY exploration_policy;
            typename CONFIG::EXPLORATION_POLICY::template Buffer<> exploration_policy_buffer;
            step<0>(device, ts.off_policy_runner, exploration_policy, exploration_policy_buffer, ts.rng);
        }
        bool train_critic_flag = ts.step >= CONFIG::CORE_PARAMETERS::N_WARMUP_STEPS_CRITIC && ts.step % CONFIG::CORE_PARAMETERS::SAC_PARAMETERS::CRITIC_TRAINING_INTERVAL == 0;
        bool update_critic_targets_flag = ts.step >= CONFIG::CORE_PARAMETERS::N_WARMUP_STEPS_CRITIC && ts.step % CONFIG::CORE_PARAMETERS::SAC_PARAMETERS::CRITIC_TARGET_UPDATE_INTERVAL == 0;
        bool train_actor_flag = ts.step >= CONFIG::CORE_PARAMETERS::N_WARMUP_STEPS_ACTOR && ts.step % CONFIG::CORE_PARAMETERS::SAC_PARAMETERS::ACTOR_TRAINING_INTERVAL == 0;
        if(CONFIG::CORE_PARAMETERS::SHARED_BATCH && (train_critic_flag || train_actor_flag)){
            gather_batch(device, ts.off_policy_runner, ts.critic_batch, ts.rng);
            randn(device, ts.action_noise_critic, ts.rng);
        }
        if(train_critic_flag){
            for(int critic_i = 0; critic_i < 2; critic_i++){
                if constexpr(!CONFIG::CORE_PARAMETERS::SHARED_BATCH) {
                    gather_batch(device, ts.off_policy_runner, ts.critic_batch, ts.rng);
                    randn(device, ts.action_noise_critic, ts.rng);
                }
                train_critic(device, ts.actor_critic, critic_i == 0 ? ts.actor_critic.critic_1 : ts.actor_critic.critic_2, ts.critic_batch, ts.critic_optimizers[critic_i], ts.actor_buffer, ts.critic_buffer, ts.critic_training_buffers[critic_i], ts.action_noise_critic, ts.rng);
            }
        }
        if(update_critic_targets_flag){
            update_critic_targets(device, ts.actor_critic);
        }
        if(train_actor_flag){
            randn(device, ts.action_noise_actor, ts.rng);
            if constexpr(CONFIG::CORE_PARAMETERS::SHARED_BATCH){
                train_actor(device, ts.actor_critic, ts.critic_batch, ts.actor_optimizer, ts.actor_buffer, ts.critic_buffer, ts.actor_training_buffers, ts.action_noise_actor, ts.rng);
            }
            else{
                gather_batch(device, ts.off_policy_runner, ts.actor_batch, ts.rng);
                train_actor(device, ts.actor_critic, ts.actor_batch, ts.actor_optimizer, ts.actor_buffer, ts.critic_buffer, ts.actor_training_buffers, ts.action_noise_actor, ts.rng);
            }
        }

        ts.step++;
        if(ts.step > CONFIG::CORE_PARAMETERS::STEP_LIMIT){
            return true;
        }
        else{
            return finished;
        }
    }
}
RL_TOOLS_NAMESPACE_WRAPPER_END


#endif
