#include "Adafruit_NeoTrellis.h"
#include "Grid.h"
#pragma once

namespace TrellisManager
{
  extern Adafruit_MultiTrellis trellis;

  void init(TrellisCallback (*pressCallback)(keyEvent));
  
  inline void update() { trellis.read(); }

  void waitForInput(bool wait = true);

  void saveKeyEvent(keyEvent event, unsigned long timePressed);

  keyEvent getKeyEvent();

  bool isWaiting();

  bool isNewEvent();
}