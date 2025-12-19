#pragma once
#include <Arduino.h>

class Log
{
public:
  enum class Mode 
  { 
    PRINT, NONE 
  };

  enum class Error
  { 
    OUT_OF_BOUNDS, BAD_VALUE, NULL_POINTER 
  };
public:
  Log() : m_Mode(Mode::PRINT) {}

  Log error(const char* message, Error error)
  {
    if (!message)
      return;
    if (m_Mode == Mode::PRINT)
      Serial.println(message);
  }

  void setMode(Mode mode) { m_Mode = mode; }
private:
  Mode m_Mode;
};