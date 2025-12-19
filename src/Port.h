#include <Audio.h>
#pragma once

class Module;

class Port
{
public:
  Port(AudioStream* device, int devicePortNum)
  : m_Device(device), 
    m_DevicePortNum(devicePortNum)
  {}

  Port(const char* name, int modulePortNum, int devicePortNum) 
  : m_DevicePortNum(devicePortNum), 
    m_ModulePortNum(modulePortNum), 
    m_Name(name)
  {}

  bool isFree() const { return m_ConnectedModule == nullptr && m_ModulePortNum != -1; }
  
public:
  Module* m_ConnectedModule{nullptr};
  AudioStream* m_Device{nullptr};
  int m_DevicePortNum{-1};
  int m_ModulePortNum{-1};
  const char* m_Name{nullptr};

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