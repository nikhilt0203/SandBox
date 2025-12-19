#include <Audio.h>
#pragma once

class Module;

class Port
{
public:
  Port();
  Port(const char* name);
  bool isFree() const;

public:
  Module* m_ConnectedModule;
  AudioStream* m_Device;
  uint8_t m_DevicePortNum;
  const char* m_Name;

public:
  static constexpr const char* INPUT_NAME = "in";
  static constexpr const char* OUTPUT_NAME = "out";
  static constexpr const char* AUDIO_IN_NAME = INPUT_NAME;
  static constexpr const char* AUDIO_OUT_NAME = OUTPUT_NAME;
  static constexpr const char* FM_IN_NAME = "fm";
  static constexpr const char* CV_IN_NAME = "cv";
  static constexpr const char* CV_OUT_NAME = "cv";
  static constexpr const char* CLOCK_IN_NAME = "clk";
};