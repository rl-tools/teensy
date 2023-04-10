#undef abs
#undef PI
#include <layer_in_c.h>
#define LAYER_IN_C_DEPLOYMENT_ARDUINO
#include </home/jonas/phd/projects/rl_for_control/layer-in-c/tests/src/rl/algorithms/td3/arm/full_training.cpp>



void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.print("setup\n");

}

void loop() {
  {
    unsigned long start = millis();
    int current_interval = 0;
    int 
    while(true){
      unsigned long current = millis();
    }
  }
  Serial.print("pre train\n");
  unsigned long start = millis();
  train();
  auto end = millis();
  Serial.printf("post train, took: %lu\n", end - start);
  for(int i = 0; i < N_EVALUATIONS; i++){
    Serial.printf("Evaluation[%d]: %f \n", i, evaluation_returns[i]);
  }
}
