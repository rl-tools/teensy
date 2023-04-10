#undef abs
#undef PI
#include <layer_in_c.h>
#define LAYER_IN_C_DEPLOYMENT_ARDUINO
#include </home/jonas/phd/projects/rl_for_control/layer-in-c/tests/src/rl/algorithms/td3/arm/full_training.cpp>
#include <data/test_layer_in_c_nn_models_mlp_evaluation.h>
#include <data/test_layer_in_c_nn_models_mlp_persist_code.h>


void main_train(){
  Serial.print("pre train\n");
  unsigned long start = millis();
  train();
  auto end = millis();
  Serial.printf("post train, took: %lu\n", end - start);
  for(int i = 0; i < N_EVALUATIONS; i++){
    Serial.printf("Evaluation[%d]: %f \n", i, evaluation_returns[i]);
  }
}
void main_test_timing(){
  unsigned long start = millis();
  int current_interval = 0;
  int interval = 10000;
  while(true){
    unsigned long current = millis();
    if(current_interval * interval < current - start){
      current_interval++;
      Serial.printf("Elapsed: %lums\n", current - start);
    }
  }
}

void main_evaluate_correctness(){
  using DEVICE = lic::devices::DefaultARM;
  DEVICE device;
  using T = float;
  using TI = typename DEVICE::index_t;
  int iterations = 1000;
  constexpr TI BATCH_SIZE = input::CONTAINER_SPEC::ROWS;
  decltype(mlp_1::mlp)::template Buffers<BATCH_SIZE, lic::MatrixDynamicTag> buffers;

  lic::MatrixDynamic<lic::matrix::Specification<T, TI, BATCH_SIZE, decltype(mlp_1::mlp)::OUTPUT_DIM>> output;
  lic::malloc(device, buffers);
  lic::malloc(device, output);
  for(int iteration_i = 0; iteration_i < iterations; iteration_i++){
    lic::evaluate(device, mlp_1::mlp, input::container, output);
  }
  for(int batch_i = 0; batch_i < BATCH_SIZE; batch_i++){
    for(int col_i = 0; col_i < decltype(mlp_1::mlp)::OUTPUT_DIM; col_i++){
        Serial.printf("%+5.5f ", get(output, batch_i, col_i));
    }
    Serial.printf("\n");
  }
  lic::free(device, buffers);
  lic::free(device, output);

}

void main_evaluate_benchmark(){
  using DEVICE = lic::devices::DefaultARM;
  DEVICE device;
  int num_runs = 10000;
  decltype(mlp_1::mlp)::template Buffers<1, lic::MatrixDynamicTag> buffers;
  using T = float;
  using TI = typename DEVICE::index_t;
  lic::MatrixDynamic<lic::matrix::Specification<T, TI, 1, decltype(mlp_1::mlp)::OUTPUT_DIM>> output;
  lic::malloc(device, buffers);
  lic::malloc(device, output);
  auto input_sample = lic::view(device, input::container, lic::matrix::ViewSpec<1, mlp_1::SPEC::INPUT_DIM>{}, 0, 0);
  while(true){
    unsigned long start = millis();
    for(int iteration_i = 0; iteration_i < num_runs; iteration_i++){
      lic::evaluate(device, mlp_1::mlp, input_sample, output);
    }
    unsigned long end = millis();
    Serial.printf("Time taken: %lu ms\n", end - start);
    Serial.printf("Frequency: %f Hz\n", num_runs / ((end - start) / 1000.0));
      for(int batch_i = 0; batch_i < 1; batch_i++){
        for(int col_i = 0; col_i < decltype(mlp_1::mlp)::OUTPUT_DIM; col_i++){
            Serial.printf("%+5.5f ", get(output, batch_i, col_i));
        }
        Serial.printf("\n");
      }
  }

  
  
  lic::free(device, buffers);
  lic::free(device, output);
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.print("setup\n");

}

void loop() {
  main_evaluate_correctness();
  main_evaluate_benchmark();
}