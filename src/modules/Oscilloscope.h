#include "Module.h"
#include <cstring>
#pragma once

class Oscilloscope : public Module
{
public:
  static constexpr ModuleConfig::Info INFO = { 
    ModuleConfig::Type::OSCILLOSCOPE, 
    "Scope", 
    "Displays the input signal", 
    { }, { }, { } 
  };

  static constexpr size_t MAX_INPUTS = 1;
  static constexpr size_t MAX_OUTPUTS = 1;

public:
  Oscilloscope() 
  : Module(new AudioBufferInput(), INFO.NAME, MAX_INPUTS, MAX_OUTPUTS)
  {
    m_Type = INFO.TYPE;
    m_AudioBuffer = static_cast<AudioBufferInput*>(m_Device);
    m_UIElement = ModuleUIElement(INFO);
  }

  ~Oscilloscope() { delete m_AudioBuffer; }

  void update() override;

  std::string_view serialize() override { return m_ID.makeSerializationID(m_UIElement); };

  static Module* buildFromString(std::string_view s);

private:
  AudioBufferInput* m_AudioBuffer;
};