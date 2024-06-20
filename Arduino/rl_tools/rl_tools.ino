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
#define RL_TOOLS_DEPLOYMENT_ARDUINO
// #define BENCHMARK
// #define RL_TOOLS_DISABLE_EVALUATION
// tested with: https://github.com/rl-tools/rl-tools/blob/c5a053fe5e521e9e6fd51f2808e87ea599fd8d6d/src/rl/environments/pendulum/sac/arm/training.h
// Copy this file somewhere on your PC and include it in the following:
#include </home/jonas/rl_tools/src/rl/environments/pendulum/sac/arm/training.h>

using CONFIG = LOOP_CORE_CONFIG;


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