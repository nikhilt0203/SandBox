#include "Module.h"
#pragma once

class Amplifier : public Module 
{ 
public:
  MODULE_INFO(
    AMPLIFIER,
    "Amplifier",
    "Adjusts the gain of the input signal",
    MODULE_PARAMS("Gain"),
    MODULE_INPUTS(),
    MODULE_OUTPUTS()
  );

  static constexpr size_t MAX_SOURCES = 1;
  static constexpr size_t MAX_DESTINATIONS = 1;

  static constexpr float MAX_GAIN_ = 5.0f;
  static constexpr float DEFAULT_GAIN = 1.0f;
  static constexpr float GAIN_CURVE = 1.10f;

public:
  Amplifier() 
  : Module(new AudioAmplifier(), INFO.NAME, MAX_SOURCES, MAX_DESTINATIONS)
  { 
    m_Type = INFO.TYPE;
    m_Amp = static_cast<AudioAmplifier*>(m_Device);
    m_UIElement = ModuleUIElement(INFO);
    setGain(DEFAULT_GAIN);
  } 

  void changeParameter(int parameterNum, int amt) override
  { 
    switch (parameterNum)
    {
      case 0: setGain(m_Gain * powf(GAIN_CURVE, amt)); break;
    }
  }
  
  void setGain(float g) 
  { 
    m_Gain = std::clamp(g, 0.0f, MAX_GAIN_);
    m_Amp->gain(m_Gain); 
    m_UIElement.updateParameter(0, m_Gain, MAX_GAIN_);
  }

  std::string_view serialize() override
  {
    m_ID.makeSerializationID(m_UIElement);
    m_ID.addParam(m_Gain);
    return m_ID.getSerializationID();
  }

  static Module* buildFromString(std::string_view s) 
  {
    //Format: Amplifier{gain}[row,col]<ID>
    int openBracket = s.find("[");
    int positionComma = s.find(",", s.find("["));

    float gain = Serialize::extractFloatBetween("{", "}", s);
    int row =  Serialize::extractIntBetween(openBracket, positionComma, s);
    int col =  Serialize::extractIntBetween(positionComma, s.find("]"), s);
    int ID =   Serialize::extractIntBetween("<", ">", s);

    Amplifier* amp = new Amplifier();
    amp->setID(ID);
    amp->setGain(gain);
    amp->getLEDElement().setPosition(row, col);
    return amp;
  }

private:
  AudioAmplifier* m_Amp;
  float m_Gain;
};