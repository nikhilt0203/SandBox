#pragma once
#include "Module.h"

class SequencerStep : public Module
{
public:
  static constexpr float MAX_AMPLITUDE = 0.5f;
  static constexpr float AMPLITUDE_STEP = 100.0f;
  static constexpr uint32_t CURSOR_COLOR = 0x888807;

  static constexpr ModuleConfig::Info INFO = {
    ModuleConfig::Type::SEQUENCERSTEP,
    "Step",
    "",
    {"Amplitude"},
    {}, {}, {}
  };

public:
  SequencerStep();

  void changeParameter(int parameterNum, int amt) override;

  void setAmplitude(float amplitude);
  float getAmplitude() const;

  void on() { m_UIElement.updateColor(CURSOR_COLOR); }
  void off() { m_UIElement.updateColor(m_CurrentColor); }

private:
  void updateStepColor();

private:
  float m_Amplitude;
  uint32_t m_CurrentColor;
};