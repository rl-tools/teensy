/*
Memory strategy:
Teensy 4.1 has 3 main memory regions:
RAM1 _:(tightly coupled, fastest) 512kb: static allocations (global variables)
RAM2 DMAMEM: (relatively tightly coupled) 512kb: dynamic memory allocations
PSRAM EXTMEM: 16mb: global variables decorated with EXTMEM

Strategy: 
RAM1: code, otherwise nothing
RAM2: actor, actor_buffers
EXTMEM: off_policy_runner/replay_buffer
*/


#undef abs
#undef PI
#undef INPUT
#undef OUTPUT
#undef ARDUINO
#include <rl_tools.h>
#define TRAIN
#ifdef TRAIN
#define RL_TOOLS_DEPLOYMENT_ARDUINO
#define BENCHMARK
// #define RL_TOOLS_DISABLE_EVALUATION
#include </home/jonas/rl_tools/src/rl/environments/pendulum/sac/arm/training.h>
#else
#include <rl_tools/operations/arm.h>
#include <rl_tools/nn/layers/dense/operations_arm/dsp.h>
#include <rl_tools/nn/layers/dense/operations_arm/opt.h>
#include <rl_tools/nn_models/mlp/operations_generic.h>
// #include <data/test_rl_tools_nn_models_mlp_evaluation.h>
// #include <data/test_rl_tools_nn_models_mlp_persist_code.h>
namespace rlt = rl_tools;
#endif

using CONFIG = LOOP_CORE_CONFIG;


// // core state
// template<typename SPEC>
// using CONTAINER_TYPE = typename CONFIG::CONTAINER_TYPE_TAG::template type<SPEC>;
// typename CONFIG::NN::OPTIMIZER actor_optimizer, critic_optimizers[2];
// typename CONFIG::RNG rng;
// typename CONFIG::UI ui;
// EXTMEM rlt::rl::components::OffPolicyRunner<typename CONFIG::OFF_POLICY_RUNNER_SPEC> off_policy_runner;
// typename CONFIG::ENVIRONMENT envs[decltype(off_policy_runner)::N_ENVIRONMENTS];
// EXTMEM typename CONFIG::ACTOR_CRITIC_TYPE actor_critic;
// typename CONFIG::NN::ACTOR_TYPE::template Buffer<1> actor_deterministic_evaluation_buffers;
// rlt::rl::components::off_policy_runner::Batch<rlt::rl::components::off_policy_runner::BatchSpecification<typename decltype(off_policy_runner)::SPEC, CONFIG::ACTOR_CRITIC_TYPE::SPEC::PARAMETERS::CRITIC_BATCH_SIZE>> critic_batch;
// EXTMEM rlt::rl::algorithms::sac::CriticTrainingBuffers<typename CONFIG::ACTOR_CRITIC_SPEC> critic_training_buffers;
// CONTAINER_TYPE<rlt::matrix::Specification<T, TI, CONFIG::PARAMETERS::SAC_PARAMETERS::CRITIC_BATCH_SIZE, CONFIG::ENVIRONMENT::ACTION_DIM>> action_noise_critic[2];
// CONTAINER_TYPE<rlt::matrix::Specification<typename CONFIG::T, TI, 1, CONFIG::ENVIRONMENT::OBSERVATION_DIM>> observations_mean, observations_std;
// EXTMEM typename CONFIG::NN::CRITIC_TYPE::template Buffer<CONFIG::ACTOR_CRITIC_TYPE::SPEC::PARAMETERS::CRITIC_BATCH_SIZE> critic_buffers[2];
// rlt::rl::components::off_policy_runner::Batch<rlt::rl::components::off_policy_runner::BatchSpecification<typename decltype(off_policy_runner)::SPEC, CONFIG::ACTOR_CRITIC_TYPE::SPEC::PARAMETERS::ACTOR_BATCH_SIZE>> actor_batch;
// rlt::rl::algorithms::sac::ActorTrainingBuffers<typename CONFIG::ACTOR_CRITIC_TYPE::SPEC> actor_training_buffers;
// CONTAINER_TYPE<rlt::matrix::Specification<T, TI, CONFIG::PARAMETERS::SAC_PARAMETERS::CRITIC_BATCH_SIZE, CONFIG::ENVIRONMENT::ACTION_DIM>> action_noise_actor;
// EXTMEM typename CONFIG::NN::ACTOR_TYPE::template Buffer<CONFIG::ACTOR_CRITIC_TYPE::SPEC::PARAMETERS::ACTOR_BATCH_SIZE> actor_buffers[2];
// typename CONFIG::NN::ACTOR_TYPE::template Buffer<CONFIG::OFF_POLICY_RUNNER_SPEC::N_ENVIRONMENTS> actor_buffers_eval;

// // evaluation state
// rlt::rl::utils::evaluation::Result<T, TI, LOOP_EVAL_CONFIG::PARAMETERS::NUM_EVALUATION_EPISODES> evaluation_results[LOOP_EVAL_CONFIG::PARAMETERS::N_EVALUATIONS];
// typename CONFIG::RNG rng_eval;
// typename CONFIG::ENVIRONMENT_EVALUATION env_eval;

// DEVICE device;
// TI step;
// TI seed = 0;
// void setup(){
//   Serial.begin(9600);
  
//   rlt::init(device);
//   rlt::init(device, device.logger);

//   // dynamically allocate actor_critic (for critic_1 and critic_2, others are static and hence no-op)
//   rlt::malloc(device, actor_critic);
//   rlt::malloc(device, actor_deterministic_evaluation_buffers);
//   rlt::malloc(device, actor_buffers[0]);
//   rlt::malloc(device, actor_buffers[1]);
//   rlt::malloc(device, actor_buffers_eval);

//   rng = rlt::random::default_engine(typename DEVICE::SPEC::RANDOM{}, seed);
//   rng_eval = rlt::random::default_engine(typename DEVICE::SPEC::RANDOM{}, seed);

//   rlt::init(device, actor_critic, rng);
  
//   rlt::init(device, envs[0], ui);
//   rlt::init(device, env_eval, ui);
//   rlt::init(device, off_policy_runner, envs);
  

//   off_policy_runner.parameters = rlt::rl::components::off_policy_runner::default_parameters<T>;


//   step = 0;
// }

// void loop(){
//   Serial.println(String("Step: ") + step);
//   rlt::set_step(device, device.logger, step);
//   bool finished = false;
//   TI evaluation_index = step / LOOP_EVAL_CONFIG::PARAMETERS::EVALUATION_INTERVAL;
//   if(step % LOOP_EVAL_CONFIG::PARAMETERS::EVALUATION_INTERVAL == 0 && evaluation_index < LOOP_EVAL_CONFIG::PARAMETERS::N_EVALUATIONS){
//     auto result = rlt::evaluate(device, env_eval, ui, actor_critic.actor, rlt::rl::utils::evaluation::Specification<LOOP_EVAL_CONFIG::PARAMETERS::NUM_EVALUATION_EPISODES, CONFIG::PARAMETERS::ENVIRONMENT_STEP_LIMIT>(), observations_mean, observations_std, actor_deterministic_evaluation_buffers, rng_eval, false);
//     rlt::log(device, device.logger, "Step: ", step, " Mean return: ", result.returns_mean);
//     rlt::add_scalar(device, device.logger, "evaluation/return/mean", result.returns_mean);
//     rlt::add_scalar(device, device.logger, "evaluation/return/std", result.returns_std);
//     evaluation_results[evaluation_index] = result;
//   }
//   rlt::step(device, off_policy_runner, actor_critic.actor, actor_buffers_eval, rng);
//   if(step > CONFIG::PARAMETERS::N_WARMUP_STEPS){
//       for(int critic_i = 0; critic_i < 2; critic_i++){
//           rlt::gather_batch(device, off_policy_runner, critic_batch, rng);
//           rlt::randn(device, action_noise_critic[critic_i], rng);
//           rlt::train_critic(device, actor_critic, critic_i == 0 ? actor_critic.critic_1 : actor_critic.critic_2, critic_batch, critic_optimizers[critic_i], actor_buffers[critic_i], critic_buffers[critic_i], critic_training_buffers, action_noise_critic[critic_i]);
//       }
//       if(step % 1 == 0){
//           {
//               rlt::gather_batch(device, off_policy_runner, actor_batch, rng);
//               rlt::randn(device, action_noise_actor, rng);
//               rlt::train_actor(device, actor_critic, actor_batch, actor_optimizer, actor_buffers[0], critic_buffers[0], actor_training_buffers, action_noise_actor);
//           }
//           rlt::update_critic_targets(device, actor_critic);
//       }
//   }
//   step++;
// }


DEVICE device;
EXTMEM LOOP_STATE ts;
TI seed = 0;
unsigned long start_time;
void setup(){
  Serial.begin(9600);
  rlt::malloc(device, ts);
  rlt::init(device, ts, 0);
  print_sizes(device, ts);
  start_time = millis();
}
void loop(){
  unsigned long now = millis();
  bool finished = rlt::step(device, ts);
  if(ts.step % 100 == 0){
    Serial.println(String("Elapsed[") + ts.step + String("]: ") + (now - start_time)/1000.0 + String("s"));
  }
  if(finished){
      Serial.println(String("Finished in: ") + (now - start_time)/1000.0 + String("s"));
      start_time = now;
      rlt::init(device, ts, seed++);
  }
}