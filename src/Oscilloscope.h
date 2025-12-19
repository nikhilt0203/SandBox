#include <Module.h>
#include "AudioBufferInput.h"
#include <cstring>
#pragma once

class Oscilloscope : public Module
{
public:
 static constexpr ModuleInfo INFO = 
  {
    Type::OSCILLOSCOPE,
    "Scope",
    "Displays the input audio signal.",
    {}, {}, {}
  };

  static constexpr int MAX_SOURCES = 1;
  static constexpr int MAX_DESTINATIONS = 1;
  static constexpr int MAX_INSTANCES = 4;
  static int instanceCount;
public:
  Oscilloscope(int row, int col) 
  : Module(new AudioBufferInput(), INFO.NAME, MAX_SOURCES, MAX_DESTINATIONS, row, col)
  {
    m_Type = INFO.TYPE;
    m_AudioBuffer = static_cast<AudioBufferInput*>(m_Device);
    setColor(m_Type);
    m_UIElement = ModuleUIElement(INFO.NAME, m_Color, {});
  }

  ~Oscilloscope() { m_AudioBuffer = nullptr; }

  void update() override
  {
    if (m_AudioBuffer->available() && !m_Ports.isInputFree(0))
      m_DisplayManager.drawWaveform(m_AudioBuffer->flush());
  }

private:
  AudioBufferInput* m_AudioBuffer;
};
