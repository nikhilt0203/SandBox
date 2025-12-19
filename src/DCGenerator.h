#include "Module.h"
#include "Port.h"
#pragma once

class DCGenerator : public Module 
{ 
public:
  static constexpr int MAX_SOURCES = 0;
  static constexpr int MAX_DESTINATIONS = 1;

  static constexpr float MAX_AMPLITUDE = 1.0f;
  static constexpr float DEFAULT_AMPLITUDE = 0.2f;

  static constexpr Type TYPE = Type::DCGENERATOR;
  static constexpr auto NAME = "DC";
  static constexpr auto PARAMETER_NAMES = {"Level"};
public: 
  DCGenerator(int row, int col) 
  : Module(new AudioSynthWaveformDc(), "DC", MAX_SOURCES, MAX_DESTINATIONS, row, col)
  { 
    m_Type = TYPE;
    m_Dc = static_cast<AudioSynthWaveformDc*>(m_Device);
    
    m_UIElement = ModuleUIElement("DC", m_Color, {"Level"});
    setColor(m_Type);
    setAmplitude(DEFAULT_AMPLITUDE);
  } 

  void changeParameter(int parameterNum, int amt) override
  { 
    switch (parameterNum)
    {
      case 0: setAmplitude(m_Amplitude + (amt / 100.0f)); break;
    }
  }

  void setAmplitude(float a) 
  { 
    if (a < 0.0f) 
      a = 0.0f;

    if (a > MAX_AMPLITUDE) 
      a = MAX_AMPLITUDE;

    m_Amplitude = a;
    m_Dc->amplitude(m_Amplitude);
    m_UIElement.updateParameter(0, m_Amplitude, MAX_AMPLITUDE);
  }

  std::string toString() override
  {
    m_ID.makeSerializedID("DC", m_Row, m_Col, 1);
    m_ID.addParam(m_Amplitude);
    return m_ID.getSerializedID();
  }

  static Module* buildFromString(std::string s) 
  { 
    //Format: DC{amplitude}[row,col]<ID>
    int openBracket = s.find("[");
    int positionComma = s.find(",", s.find("["));

    float amplitude = Serialize::extractFloatBetween("{", "}", s);
    int row =         Serialize::extractIntBetween(openBracket, positionComma, s);
    int col =         Serialize::extractIntBetween(positionComma, s.find("]"), s);
    int ID =          Serialize::extractIntBetween("<", ">", s);

    DCGenerator* dc = new DCGenerator(row, col);
    dc->setID(ID);
    dc->setAmplitude(amplitude);
    return dc;
  }

private:
  AudioSynthWaveformDc* m_Dc;
  float m_Amplitude;
};