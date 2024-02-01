#undef abs
#undef PI
#undef INPUT
#undef OUTPUT
#undef ARDUINO
#include <rl_tools.h>
#define TRAIN
#ifdef TRAIN
#define RL_TOOLS_DEPLOYMENT_ARDUINO
// #define RL_TOOLS_DISABLE_EVALUATION
#include </home/jonas/rl_tools/src/rl/environments/pendulum/sac/arm/training2.h>
#else
#include <rl_tools/operations/arm.h>
#include <rl_tools/nn/layers/dense/operations_arm/dsp.h>
#include <rl_tools/nn/layers/dense/operations_arm/opt.h>
#include <rl_tools/nn_models/mlp/operations_generic.h>
#include <data/test_rl_tools_nn_models_mlp_evaluation.h>
#include <data/test_rl_tools_nn_models_mlp_persist_code.h>
namespace rlt = rl_tools;
#endif

#ifdef TRAIN
// void main_train(){
//   Serial.print("pre train\n");
//   unsigned long start = millis();
//   train();
//   auto end = millis();
//   Serial.printf("post train, took: %lu\n", end - start);
// #ifndef RL_TOOLS_DISABLE_EVALUATION
//   for(int i = 0; i < LOOP_CONFIG::PARAMETERS::N_EVALUATIONS; i++){
//     Serial.printf("Evaluation[%d]: %f \n", i, ts.evaluation_returns[i]);
//   }
// #endif
// }
// #else
// void main_test_timing(){
//   unsigned long start = millis();
//   int current_interval = 0;
//   int interval = 10000;
//   while(true){
//     unsigned long current = millis();
//     if(current_interval * interval < current - start){
//       current_interval++;
//       Serial.printf("Elapsed: %lums\n", current - start);
//     }
//   }
// }

// template <typename DEVICE>
// void main_evaluate_correctness(){

//   DEVICE device;
//   using T = float;
//   using TI = typename DEVICE::index_t;
//   int iterations = 1000;
//   constexpr TI BATCH_SIZE = input::CONTAINER_SPEC::ROWS;
//   decltype(mlp_1::mlp)::template Buffers<BATCH_SIZE, rlt::MatrixDynamicTag> buffers;

//   rlt::MatrixDynamic<rlt::matrix::Specification<T, TI, BATCH_SIZE, decltype(mlp_1::mlp)::OUTPUT_DIM>> output;
//   rlt::malloc(device, buffers);
//   rlt::malloc(device, output);
//   for(int iteration_i = 0; iteration_i < iterations; iteration_i++){
//     rlt::evaluate(device, mlp_1::mlp, input::container, output);
//   }
//   for(int batch_i = 0; batch_i < BATCH_SIZE; batch_i++){
//     for(int col_i = 0; col_i < decltype(mlp_1::mlp)::OUTPUT_DIM; col_i++){
//         Serial.printf("%+5.5f ", get(output, batch_i, col_i));
//     }
//     Serial.printf("\n");
//   }
//   rlt::free(device, buffers);
//   rlt::free(device, output);

// }

// template <typename DEVICE>
// void main_evaluate_benchmark(){
//   DEVICE device;
//   int num_runs = 10000;
//   decltype(mlp_1::mlp)::template Buffers<1, rlt::MatrixDynamicTag> buffers;
//   using T = float;
//   using TI = typename DEVICE::index_t;
//   rlt::MatrixDynamic<rlt::matrix::Specification<T, TI, 1, decltype(mlp_1::mlp)::OUTPUT_DIM>> output;
//   rlt::malloc(device, buffers);
//   rlt::malloc(device, output);
//   auto input_sample = rlt::view(device, input::container, rlt::matrix::ViewSpec<1, mlp_1::SPEC::INPUT_DIM>{}, 0, 0);
//   while(true){
//     unsigned long start = millis();
//     for(int iteration_i = 0; iteration_i < num_runs; iteration_i++){
//       rlt::evaluate(device, mlp_1::mlp, input_sample, output);
//     }
//     unsigned long end = millis();
//     Serial.printf("Time taken: %lu ms\n", end - start);
//     Serial.printf("Frequency: %f Hz\n", num_runs / ((end - start) / 1000.0));
//       for(int batch_i = 0; batch_i < 1; batch_i++){
//         for(int col_i = 0; col_i < decltype(mlp_1::mlp)::OUTPUT_DIM; col_i++){
//             Serial.printf("%+5.5f ", get(output, batch_i, col_i));
//         }
//         Serial.printf("\n");
//       }
//   }

//   rlt::free(device, buffers);
//   rlt::free(device, output);
// }
#endif


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.print("setup\n");

}
EXTMEM LOOP_STATE ts;
void train2(){
    DEVICE device;

    rlt::malloc(device, ts);
    rlt::init(device, ts, 0);
    print_sizes(device, ts);
    while(!rlt::step(device, ts)){
        // Serial.println(String("step: ") + ts.step);
    }
    rlt::free(device, ts);
}

void loop() {
#ifdef TRAIN
  // main_train();
  train2();
#else
  using DEV_SPEC = rlt::devices::DefaultARMSpecification; 
  using DEVICE = rlt::devices::arm::OPT<DEV_SPEC>;
  main_evaluate_correctness<DEVICE>(); // disable this for "DSP" because it only supports BATCH_SIZE=1
  main_evaluate_benchmark<DEVICE>();

#endif
}