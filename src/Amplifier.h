#include "Module.h"
#pragma once

class Amplifier : public Module 
{ 
public:
  static constexpr int MAX_SOURCES = 1;
  static constexpr int MAX_DESTINATIONS = 1;

  static constexpr float MAX_GAIN = 5.0f;
  static constexpr float DEFAULT_GAIN = 1.0f;
  static constexpr float GAIN_CURVE = 1.10f;

  static constexpr ModuleInfo INFO =
  {
    Type::AMPLIFIER,
    "Amplifier",
    nullptr,
    {"Gain"}, {}, {}
  };

public:
  Amplifier(int row, int col) 
  : Module(new AudioAmplifier(), INFO.NAME, MAX_SOURCES, MAX_DESTINATIONS, row, col)
  { 
    m_Type = INFO.TYPE;
    m_Amp = static_cast<AudioAmplifier*>(m_Device);
    m_UIElement = ModuleUIElement(INFO.NAME, m_Color, INFO.PARAMETER_NAMES);
    setColor(m_Type);
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
    g = std::clamp(g, 0.0f, MAX_GAIN);
    m_Gain = g;
    m_Amp->gain(g); 
    m_UIElement.updateParameter(0, m_Gain, MAX_GAIN);
  }

  std::string toString() override
  {
    m_ID.makeSerializedID(INFO.NAME, m_Row, m_Col, 1);
    m_ID.addParam(m_Gain);
    return m_ID.getSerializedID();
  }

  static Module* buildFromString(std::string s) 
  {
    //Format: Amplifier{gain}[row,col]<ID>
    int openBracket = s.find("[");
    int positionComma = s.find(",", s.find("["));

    float gain = Serialize::extractFloatBetween("{", "}", s);
    int row =  Serialize::extractIntBetween(openBracket, positionComma, s);
    int col =  Serialize::extractIntBetween(positionComma, s.find("]"), s);
    int ID =   Serialize::extractIntBetween("<", ">", s);

    Amplifier* amp = new Amplifier(row, col);
    amp->setID(ID);
    amp->setGain(gain);
    return amp;
  }

private:
  AudioAmplifier* m_Amp;
  float m_Gain;
};