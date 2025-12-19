#pragma once
#include <Encoder.h>
#include "Module.h"

#define NUM_ENCODERS 4

namespace EncoderManager 
{
  void init();
  void update();
  bool isWaiting();
  void waitForInput(bool wait = true);
  int getInput(size_t encoderNum);
}