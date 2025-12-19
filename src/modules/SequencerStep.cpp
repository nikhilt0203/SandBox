#include "SequencerStep.h"
#include "Colors.h"

SequencerStep::SequencerStep() 
: Module(nullptr, INFO.NAME, 0, 0)
{
  m_Type = INFO.TYPE;
  m_UIElement = ModuleUIElement(INFO);
  
  setAmplitude(0.0f);
  updateStepColor();
  off();
}

void SequencerStep::changeParameter(int parameterNum, int amt)
{ 
  switch (parameterNum)
  {
    case 0: setAmplitude(m_Amplitude + (amt / AMPLITUDE_STEP)); break;
  }
}

float SequencerStep::getAmplitude() const
{
  return m_Amplitude;
}

void SequencerStep::setAmplitude(float amplitude)
{
  m_Amplitude = std::clamp(amplitude, 0.0f, MAX_AMPLITUDE);
  m_UIElement.updateParameter(0, m_Amplitude, MAX_AMPLITUDE);
    updateStepColor();
}

void SequencerStep::updateStepColor()
{
  constexpr static uint8_t MIN_BRIGHTNESS = 8;
  constexpr static uint8_t MAX_BRIGHTNESS = 255;
  
  uint8_t brightness = (m_Amplitude == 0) ? 
    MIN_BRIGHTNESS : 
    std::min(MAX_BRIGHTNESS, static_cast<uint8_t>(MAX_BRIGHTNESS * m_Amplitude * 5));

  m_CurrentColor = Colors::getHex(brightness, brightness, brightness);
  m_UIElement.updateColor(m_CurrentColor);
}
