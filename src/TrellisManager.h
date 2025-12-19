#include "Adafruit_NeoTrellis.h"
#include "Grid.h"
#pragma once

namespace TrellisManager
{
  extern Adafruit_NeoTrellis t_array[Grid::ROWS/4][Grid::COLS/4];
  extern Adafruit_MultiTrellis trellis;
  
  void init(TrellisCallback (*pressCallback)(keyEvent));
  void update();
}