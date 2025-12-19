#pragma once
#include <Arduino.h>

class Timer
{
public:
  Timer() = default;

  void start() { lastTime = millis(); }

  void reset() { lastTime = millis(); }

  bool hasReached(unsigned long milliseconds) const { return millis() - lastTime > milliseconds; }
  
  unsigned long read() const { return millis() - lastTime; }
  
private:
  unsigned long lastTime{};
};