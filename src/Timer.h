#pragma once
#include <Arduino.h>

class Timer
{
public:
  Timer() : lastTime(0) {}
  inline void reset() { lastTime = millis(); }
  inline bool hasReached(unsigned long maxMs) const { return millis() - lastTime > maxMs; }
  inline unsigned long read() const { return millis() - lastTime; }
  
private:
  unsigned long lastTime;
};