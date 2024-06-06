#include "../../../../../version.h"
#if (defined(RL_TOOLS_DISABLE_INCLUDE_GUARDS) || !defined(RL_TOOLS_RL_ALGORITHMS_PPO_LOOP_CORE_OPERATIONS_GENERIC_H)) && (RL_TOOLS_USE_THIS_VERSION == 1)
#pragma once
#define RL_TOOLS_RL_ALGORITHMS_PPO_LOOP_CORE_OPERATIONS_GENERIC_H

#include "../../../../../nn/optimizers/adam/instance/operations_generic.h"
#include "../../../../../nn/layers/standardize/operations_generic.h"
#include "../../../../../nn_models/mlp_unconditional_stddev/operations_generic.h"
#include "../../../../../nn_models/sequential/operations_generic.h"
#include "../../../../../nn/optimizers/adam/operations_generic.h"
#include "../../../../../rl/algorithms/ppo/operations_generic.h"
#include "../../../../../rl/components/on_policy_runner/operations_generic.h"
#include "../../../../../rl/components/running_normalizer/operations_generic.h"

#include "config.h"

RL_TOOLS_NAMESPACE_WRAPPER_START
namespace rl_tools{
    template <typename DEVICE, typename T_CONFIG>
    void malloc(DEVICE& device, rl::algorithms::ppo::loop::core::State<T_CONFIG>& ts){
        malloc(device, ts.ppo);
        malloc(device, ts.ppo_buffers);
        malloc(device, ts.on_policy_runner_dataset);
        malloc(device, ts.on_policy_runner);
        malloc(device, ts.actor_eval_buffers);
        malloc(device, ts.actor_deterministic_evaluation_buffers);
        malloc(device, ts.actor_buffers);
        malloc(device, ts.critic_buffers);
        malloc(device, ts.critic_buffers_gae);
        malloc(device, ts.observation_normalizer);
        for(auto& env: ts.envs){
            malloc(device, env);
        }

    }
    template <typename DEVICE, typename T_CONFIG>
    void init(DEVICE& device, rl::algorithms::ppo::loop::core::State<T_CONFIG>& ts, typename T_CONFIG::TI seed = 0){
        using CONFIG = T_CONFIG;
        using T = typename CONFIG::T;

        ts.rng = random::default_engine(typename DEVICE::SPEC::RANDOM(), seed);

        for(auto& env: ts.envs){
            rl_tools::init(device, env);
        }

        init(device, ts.on_policy_runner, ts.envs, ts.rng);
        init(device, ts.observation_normalizer);
        init(device, ts.ppo, ts.actor_optimizer, ts.critic_optimizer, ts.rng);

        ts.step = 0;
    }

    template <typename DEVICE, typename T_CONFIG>
    void free(DEVICE& device, rl::algorithms::ppo::loop::core::State<T_CONFIG>& ts){
        free(device, ts.ppo);
        free(device, ts.ppo_buffers);
        free(device, ts.on_policy_runner_dataset);
        free(device, ts.on_policy_runner);
        free(device, ts.actor_eval_buffers);
        free(device, ts.actor_deterministic_evaluation_buffers);
        free(device, ts.actor_buffers);
        free(device, ts.critic_buffers);
        free(device, ts.critic_buffers_gae);
        free(device, ts.observation_normalizer);
        for(auto& env: ts.envs){
            free(device, env);
        }
    }

    template <typename DEVICE, typename T_CONFIG>
    bool step(DEVICE& device, rl::algorithms::ppo::loop::core::State<T_CONFIG>& ts){
        using CONFIG = T_CONFIG;
        using TI = typename DEVICE::index_t;
        set_step(device, device.logger, ts.step);
        bool finished = false;

        if(T_CONFIG::CORE_PARAMETERS::NORMALIZE_OBSERVATIONS && ts.step == 0){
            for(TI observation_normalization_warmup_step_i = 0; observation_normalization_warmup_step_i < T_CONFIG::OBSERVATION_NORMALIZATION_WARMUP_STEPS; observation_normalization_warmup_step_i++) {
                collect(device, ts.on_policy_runner_dataset, ts.on_policy_runner, ts.ppo.actor, ts.actor_eval_buffers, ts.rng);
                update(device, ts.observation_normalizer, ts.on_policy_runner_dataset.observations);
            }
            std::cout << "Observation means: " << std::endl;
            print(device, ts.observation_normalizer.mean);
            std::cout << "Observation std: " << std::endl;
            print(device, ts.observation_normalizer.std);
            init(device, ts.on_policy_runner, ts.envs, ts.rng); // reinitializing the on_policy_runner to reset the episode counters
            set_statistics(device, ts.ppo.actor.content, ts.observation_normalizer.mean, ts.observation_normalizer.std);
            set_statistics(device, ts.ppo.critic.content, ts.observation_normalizer.mean, ts.observation_normalizer.std);
        }
        collect(device, ts.on_policy_runner_dataset, ts.on_policy_runner, ts.ppo.actor, ts.actor_eval_buffers, ts.rng);
        if(T_CONFIG::CORE_PARAMETERS::NORMALIZE_OBSERVATIONS && T_CONFIG::CORE_PARAMETERS::NORMALIZE_OBSERVATIONS_CONTINUOUSLY){
            update(device, ts.observation_normalizer, ts.on_policy_runner_dataset.all_observations);
            set_statistics(device, ts.ppo.actor.content, ts.observation_normalizer.mean, ts.observation_normalizer.std);
        }
        evaluate(device, ts.ppo.critic, ts.on_policy_runner_dataset.all_observations, ts.on_policy_runner_dataset.all_values, ts.critic_buffers_gae, ts.rng);
        estimate_generalized_advantages(device, ts.on_policy_runner_dataset, typename CONFIG::PPO_TYPE::SPEC::PARAMETERS{});
        train(device, ts.ppo, ts.on_policy_runner_dataset, ts.actor_optimizer, ts.critic_optimizer, ts.ppo_buffers, ts.actor_buffers, ts.critic_buffers, ts.rng);

//        log(device, device.logger, "log_std: ", get(ts.ppo.actor.log_std.parameters, 0, 0));

        ts.step++;
        if(ts.step > CONFIG::CORE_PARAMETERS::STEP_LIMIT){
            return true;
        }
        else{
            return finished;
        }
    }
    template <typename DEVICE, typename PARAMETERS, typename utils::typing::enable_if<utils::typing::is_same_v<typename PARAMETERS::TAG, rl::algorithms::ppo::loop::core::ParametersTag>>::type* = nullptr>
    void log(DEVICE& device, PARAMETERS){
        log(device, device.logger, "STEP_LIMIT: ", PARAMETERS::STEP_LIMIT);
        log(device, device.logger, "ACTOR_HIDDEN_DIM: ", PARAMETERS::ACTOR_HIDDEN_DIM);
        log(device, device.logger, "ACTOR_NUM_LAYERS: ", PARAMETERS::ACTOR_NUM_LAYERS);
        log(device, device.logger, "ACTOR_ACTIVATION_FUNCTION: ", PARAMETERS::ACTOR_ACTIVATION_FUNCTION);
        log(device, device.logger, "CRITIC_HIDDEN_DIM: ", PARAMETERS::CRITIC_HIDDEN_DIM);
        log(device, device.logger, "CRITIC_NUM_LAYERS: ", PARAMETERS::CRITIC_NUM_LAYERS);
        log(device, device.logger, "CRITIC_ACTIVATION_FUNCTION: ", PARAMETERS::CRITIC_ACTIVATION_FUNCTION);
        log(device, device.logger, "EPISODE_STEP_LIMIT: ", PARAMETERS::EPISODE_STEP_LIMIT);
        log(device, device.logger, "N_ENVIRONMENTS: ", PARAMETERS::N_ENVIRONMENTS);
        log(device, device.logger, "ON_POLICY_RUNNER_STEPS_PER_ENV: ", PARAMETERS::ON_POLICY_RUNNER_STEPS_PER_ENV);
        log(device, device.logger, "BATCH_SIZE: ", PARAMETERS::BATCH_SIZE);
    }
    template <typename DEVICE, typename CONFIG, typename utils::typing::enable_if<utils::typing::is_same_v<typename CONFIG::TAG, rl::algorithms::ppo::loop::core::ConfigTag>>::type* = nullptr>
    void log(DEVICE& device, CONFIG){
        log(device, typename CONFIG::CORE_PARAMETERS{});
//        log(device, typename CONFIG::NEXT{});
    }



}
RL_TOOLS_NAMESPACE_WRAPPER_END


#endif
