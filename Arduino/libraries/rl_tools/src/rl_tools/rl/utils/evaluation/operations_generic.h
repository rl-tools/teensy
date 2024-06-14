#include "../../../version.h"
#if (defined(RL_TOOLS_DISABLE_INCLUDE_GUARDS) || !defined(RL_TOOLS_RL_UTILS_EVALUATION_OPERATIONS_GENERIC_H)) && (RL_TOOLS_USE_THIS_VERSION == 1)
#pragma once
#define RL_TOOLS_RL_UTILS_EVALUATION_OPERATIONS_GENERIC_H
/*
 * This file relies on the environments methods hence it should be included after the operations of the environments that it will be used with
 */

#include "../../../math/operations_generic.h"
#include "../../environments/operations_generic.h"

#include "evaluation.h"

RL_TOOLS_NAMESPACE_WRAPPER_START
namespace rl_tools{
    namespace rl::utils::evaluation{
        template<typename TI, typename SPEC>
        void set_state(rl::utils::evaluation::NoData<SPEC>& data, TI episode_i, TI step_i, const typename SPEC::ENVIRONMENT::State state){}
        template<typename TI, typename SPEC>
        void set_state(rl::utils::evaluation::Data<SPEC>& data, TI episode_i, TI step_i, const typename SPEC::ENVIRONMENT::State state){
            data.states[episode_i][step_i] = state;
        }
        template<typename TI, typename SPEC, typename ACTION_SPEC>
        void set_action(rl::utils::evaluation::NoData<SPEC>& data, TI step_i, const Matrix<ACTION_SPEC>& actions){}
        template<typename TI, typename SPEC, typename ACTION_SPEC>
        void set_action(rl::utils::evaluation::Data<SPEC>& data, TI step_i, const Matrix<ACTION_SPEC>& actions){
            static_assert(ACTION_SPEC::ROWS == SPEC::N_EPISODES);
            static_assert(ACTION_SPEC::COLS == SPEC::ENVIRONMENT::ACTION_DIM);
            for (TI episode_i = 0; episode_i < SPEC::N_EPISODES; episode_i++){
                for (TI action_i = 0; action_i < SPEC::ENVIRONMENT::ACTION_DIM; action_i++) {
                    data.actions[episode_i][step_i][action_i] = get(actions, episode_i, action_i);
                }
            }
        }
        template<typename TI, typename SPEC>
        void set_reward(rl::utils::evaluation::NoData<SPEC>& data, TI episode_i, TI step_i, typename SPEC::T reward){}
        template<typename TI, typename SPEC>
        void set_reward(rl::utils::evaluation::Data<SPEC>& data, TI episode_i, TI step_i, typename SPEC::T reward){
            data.rewards[episode_i][step_i] = reward;
        }
        template<typename TI, typename SPEC>
        void set_terminated(rl::utils::evaluation::NoData<SPEC>& data, TI episode_i, TI step_i, bool terminated){}
        template<typename TI, typename SPEC>
        void set_terminated(rl::utils::evaluation::Data<SPEC>& data, TI episode_i, TI step_i, bool terminated){
            data.terminated[episode_i][step_i] = terminated;
        }
    }
    template<typename DEVICE, typename ENVIRONMENT, typename UI, typename POLICY, typename RNG, typename SPEC, template <typename> typename DATA, typename POLICY_EVALUATION_BUFFERS>
    void evaluate(DEVICE& device, ENVIRONMENT&, UI& ui, const POLICY& policy, rl::utils::evaluation::Result<SPEC>& results, DATA<SPEC>& data, POLICY_EVALUATION_BUFFERS& policy_evaluation_buffers, RNG &rng, bool deterministic = false){
        using T = typename POLICY::T;
        using TI = typename DEVICE::index_t;
        static_assert(ENVIRONMENT::OBSERVATION_DIM == POLICY::INPUT_DIM, "Observation and policy input dimensions must match");
        static_assert(ENVIRONMENT::ACTION_DIM == POLICY::OUTPUT_DIM || (2*ENVIRONMENT::ACTION_DIM == POLICY::OUTPUT_DIM), "Action and policy output dimensions must match");
        static constexpr bool STOCHASTIC_POLICY = POLICY::OUTPUT_DIM == 2*ENVIRONMENT::ACTION_DIM;
        results.returns_mean = 0;
        results.returns_std = 0;
        results.episode_length_mean = 0;
        results.episode_length_std = 0;

        MatrixStatic<matrix::Specification<T, TI, SPEC::N_EPISODES, ENVIRONMENT::ACTION_DIM * (STOCHASTIC_POLICY ? 2 : 1)>> actions_buffer_full;
        MatrixStatic<matrix::Specification<T, TI, SPEC::N_EPISODES, ENVIRONMENT::OBSERVATION_DIM>> observations;
        auto actions_buffer = view(device, actions_buffer_full, matrix::ViewSpec<SPEC::N_EPISODES, ENVIRONMENT::ACTION_DIM>{});

        ENVIRONMENT envs[SPEC::N_EPISODES];
        typename ENVIRONMENT::State states[SPEC::N_EPISODES];
        bool terminated[SPEC::N_EPISODES];

        for(TI env_i = 0; env_i < SPEC::N_EPISODES; env_i++){
            auto& env = envs[env_i];
            malloc(device, env);
            init(device, env);
            results.returns[env_i] = 0;
            results.episode_length[env_i] = 0;
            terminated[env_i] = false;
            auto& state = states[env_i];
            if(deterministic) {
                rl_tools::initial_state(device, env, state);
            }
            else{
                sample_initial_state(device, env, state, rng);
            }
        }
        for(TI step_i = 0; step_i < SPEC::STEP_LIMIT; step_i++) {
            for(TI env_i = 0; env_i < SPEC::N_EPISODES; env_i++) {
                auto observation = row(device, observations, env_i);
                auto& state = states[env_i];
                rl::utils::evaluation::set_state(data, env_i, step_i, states[env_i]);
                auto& env = envs[env_i];
                observe(device, env, state, observation, rng);
            }
            constexpr TI BATCH_SIZE = POLICY_EVALUATION_BUFFERS::BATCH_SIZE;
            constexpr TI NUM_FORWARD_PASSES = SPEC::N_EPISODES / BATCH_SIZE;
            if constexpr(NUM_FORWARD_PASSES > 0){
                for(TI forward_pass_i = 0; forward_pass_i < NUM_FORWARD_PASSES; forward_pass_i++){
                    auto observations_chunk = view(device, observations, matrix::ViewSpec<BATCH_SIZE, ENVIRONMENT::OBSERVATION_DIM>{}, forward_pass_i*BATCH_SIZE, 0);
                    auto actions_buffer_chunk = view(device, actions_buffer_full, matrix::ViewSpec<BATCH_SIZE, ENVIRONMENT::ACTION_DIM * (STOCHASTIC_POLICY ? 2 : 1)>{}, forward_pass_i*BATCH_SIZE, 0);
//                    sample(device, policy_evaluation_buffers, rng);
                    evaluate(device, policy, observations_chunk, actions_buffer_chunk, policy_evaluation_buffers, rng, nn::Mode<nn::mode::Inference>{});
                }
            }
            if constexpr(SPEC::N_EPISODES % BATCH_SIZE != 0){
                auto observations_chunk = view(device, observations, matrix::ViewSpec<SPEC::N_EPISODES % BATCH_SIZE, ENVIRONMENT::OBSERVATION_DIM>{}, NUM_FORWARD_PASSES*BATCH_SIZE, 0);
                auto actions_buffer_chunk = view(device, actions_buffer_full, matrix::ViewSpec<SPEC::N_EPISODES % BATCH_SIZE, ENVIRONMENT::ACTION_DIM * (STOCHASTIC_POLICY ? 2 : 1)>{}, NUM_FORWARD_PASSES*BATCH_SIZE, 0);
//                sample(device, policy_evaluation_buffers, rng);
                evaluate(device, policy, observations_chunk, actions_buffer_chunk, policy_evaluation_buffers, rng, nn::Mode<nn::mode::Inference>{});
            }
            if constexpr(STOCHASTIC_POLICY){ // todo: This is a special case for SAC, will be uneccessary once (https://github.com/rl-tools/rl-tools/blob/72a59eabf4038502c3be86a4f772bd72526bdcc8/TODO.md?plain=1#L22) is implemented
                for(TI env_i = 0; env_i < SPEC::N_EPISODES; env_i++) {
                    for (TI action_i = 0; action_i < ENVIRONMENT::ACTION_DIM; action_i++) {
                        set(actions_buffer, env_i, action_i, math::tanh<T>(device.math, get(actions_buffer, env_i, action_i)));
                    }
                }
            }
            rl::utils::evaluation::set_action(data, step_i, actions_buffer);
            for(TI env_i = 0; env_i < SPEC::N_EPISODES; env_i++) {
                if(step_i > 0){
                    if(terminated[env_i]){
                        continue;
                    }
                }
                auto& env = envs[env_i];
                typename ENVIRONMENT::State next_state;
                auto& state = states[env_i];
                auto action = row(device, actions_buffer, env_i);
                T dt = step(device, env, state, action, next_state, rng);
                if(env_i == 0 && !terminated[env_i]){ // only render the first environment
                    set_state(device, env, ui, state);
                    set_action(device, env, ui, action);
                    render(device, env, ui);
                }
                T r = reward(device, env, state, action, next_state, rng);
                rl::utils::evaluation::set_reward(data, env_i, step_i, r);
                bool terminated_flag = rl_tools::terminated(device, env, next_state, rng);
                terminated_flag = terminated_flag || terminated[env_i];
                terminated[env_i] = terminated_flag;
                rl::utils::evaluation::set_terminated(data, env_i, step_i, terminated_flag);
                if(!terminated_flag){
                    results.returns[env_i] += r;
                    results.episode_length[env_i] += 1;
                }
                states[env_i] = next_state;
            }
        }
        for(TI env_i = 0; env_i < SPEC::N_EPISODES; env_i++) {
            auto &env = envs[env_i];
            free(device, env);
        }
        for(TI env_i = 0; env_i < SPEC::N_EPISODES; env_i++){
            results.returns[env_i] = results.returns[env_i];
            results.returns_mean += results.returns[env_i];
            results.returns_std += results.returns[env_i]*results.returns[env_i];
            results.episode_length[env_i] = results.episode_length[env_i];
            results.episode_length_mean += results.episode_length[env_i];
            results.episode_length_std += results.episode_length[env_i]*results.episode_length[env_i];
        }
        results.returns_mean /= SPEC::N_EPISODES;
        results.returns_std = math::sqrt(device.math, math::max(device.math, (T)0, results.returns_std/SPEC::N_EPISODES - results.returns_mean*results.returns_mean));
        results.episode_length_mean /= SPEC::N_EPISODES;
        results.episode_length_std = math::sqrt(device.math, math::max(device.math, (T)0, results.episode_length_std/SPEC::N_EPISODES - results.episode_length_mean*results.episode_length_mean));
    }
    template<typename DEVICE, typename ENVIRONMENT, typename UI, typename POLICY, typename RNG, typename SPEC, typename POLICY_EVALUATION_BUFFERS>
    void evaluate(DEVICE& device, ENVIRONMENT& env, UI& ui, const POLICY& policy, rl::utils::evaluation::Result<SPEC>& results, POLICY_EVALUATION_BUFFERS& policy_evaluation_buffers, RNG &rng, bool deterministic = false){
        rl::utils::evaluation::NoData<SPEC> data;
        evaluate(device, env, ui, policy, results, data, policy_evaluation_buffers, rng, deterministic);
    }
}
RL_TOOLS_NAMESPACE_WRAPPER_END

#endif
