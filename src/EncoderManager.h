#pragma once
#include <Encoder.h>
#include "Module.h"

namespace EncoderManager 
{
  constexpr int NUM_ENCODERS = 4;

  void init();
  void update();
}