#pragma once
#include "Module.h"

class SequencerStep : public Module
{
public:
  static constexpr Type TYPE = Type::SEQUENCERSTEP;
  static constexpr const char* NAME = "Step";
public:
  SequencerStep(int row, int col) 
  : Module(nullptr, NAME, 0, 0, row, col), 
    m_Amplitude(0)
  {
    //Module::setPosition(row, col);
    m_Type = TYPE;
    updateStepColor();
    off();
    markDisplayUpdated(false);
  }

  void changeParameter(int parameterNum, int amt) override
  { 
    switch (parameterNum)
    {
      case 0: changeAmplitude(amt); break;
    }
  }

  void setAmplitude(float amplitude)
  {
    if (amplitude > 1.0 || amplitude < 0.0) return;
    m_Amplitude = amplitude;
    updateStepColor();
  }

  float getAmplitude() const { return m_Amplitude; }

  void on()
  {
    Module::setColor(0xFFFFFF); 
    markDisplayUpdated(false);
  }

  void off()
  {
    Module::setColor(m_StepColor);
    markDisplayUpdated(false);
  }

private:
  void updateStepColor()
  {
    int brightness;

    if (m_Amplitude == 0) 
      brightness = 10;
    else
      brightness = std::min(255, (int)(255.0 * m_Amplitude * 10));

    m_StepColor = Colors::getHex(brightness, brightness, brightness);
    Module::setColor(m_StepColor);
    markDisplayUpdated(false);
  }

  void changeAmplitude(int amt)
  {
    m_Amplitude += amt / 100.0f;
    if (m_Amplitude < 0.0) m_Amplitude = 0.0;
    if (m_Amplitude > 1.0) m_Amplitude = 1.0;
    updateStepColor();
  }
private:
  float m_Amplitude;
  uint32_t m_StepColor;
};