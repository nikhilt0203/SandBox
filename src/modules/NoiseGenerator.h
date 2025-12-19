#include "Module.h"
#pragma once

class NoiseGenerator : public Module
{
public:
  static constexpr ModuleConfig::Info INFO = {
    ModuleConfig::Type::NOISE, 
    "Noise",
    "Outputs white noise",
    {"Level"},
    {}, {}
  };

  static constexpr size_t MAX_INPUTS = 0;
  static constexpr size_t MAX_OUTPUTS = 1;
  
  static constexpr float DEFAULT_AMPLITUDE = 0.5f;
  static constexpr float MAX_AMPLITUDE = 1.0f;
  static constexpr float AMPLITUDE_CURVE = 1.10f;

public:
  NoiseGenerator()
   : Module(new AudioSynthNoiseWhite(), INFO.NAME, MAX_INPUTS, MAX_OUTPUTS)
  {
    m_Type = INFO.TYPE;
    m_UIElement = ModuleUIElement(INFO);
    m_Noise = static_cast<AudioSynthNoiseWhite*>(m_Device);

    setAmplitude(DEFAULT_AMPLITUDE);
  }

  void changeParameter(int parameterNum, int amt) override
  {
    switch (parameterNum)
    {
      case 0: setAmplitude(m_Amplitude * powf(AMPLITUDE_CURVE, amt)); break;
    }
  }

  void setAmplitude(float amplitude)
  {
    m_Amplitude = std::clamp(amplitude, 0.0f, MAX_AMPLITUDE);
    m_Noise->amplitude(m_Amplitude);
    m_UIElement.updateParameter(0, m_Amplitude, MAX_AMPLITUDE);
  }

  std::string_view serialize() override
  {
    m_ID.makeSerializationID(m_UIElement);
    m_ID.addParam(m_Amplitude);
    return m_ID.getSerializationID();
  }

  static Module* buildFromString(std::string_view s)
  {
    //Format: Noise{amplitude}[row,col]<ID>_type_
    size_t positionComma = s.find(",", s.find("["));

    float amplitude = Serialize::extractFloatBetween("{", "}", s);
    int row = Serialize::extractIntBetween(s.find("["), positionComma, s);
    int col = Serialize::extractIntBetween(positionComma, s.find("]"), s);
    int ID  = Serialize::extractIntBetween("<", ">", s);

    NoiseGenerator* noise = new NoiseGenerator();
    noise->setID(ID);
    noise->getLEDElement().setPosition(row, col);
    noise->setAmplitude(amplitude);
    return noise;
  }

private:
  AudioSynthNoiseWhite* m_Noise;
  float m_Amplitude;
};