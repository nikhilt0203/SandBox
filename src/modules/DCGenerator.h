#include "Module.h"
#include "Port.h"
#pragma once

class DCGenerator : public Module 
{ 
public:
  MODULE_INFO(
    DCGENERATOR,
    "DC",
    "Generates a constant voltage",
    MODULE_PARAMS("Amplitude"),
    MODULE_INPUTS(),
    MODULE_OUTPUTS()
  );

public:
  static constexpr size_t MAX_SOURCES = 0;
  static constexpr size_t MAX_DESTINATIONS = 1;

  static constexpr float MAX_AMPLITUDE = 1.0f;
  static constexpr float DEFAULT_AMPLITUDE = 0.2f;
  
public: 
  DCGenerator() 
  : Module(new AudioSynthWaveformDc(), INFO.NAME, MAX_SOURCES, MAX_DESTINATIONS)
  { 
    m_Type = INFO.TYPE;
    m_DC = static_cast<AudioSynthWaveformDc*>(m_Device);
    
    m_UIElement = ModuleUIElement(INFO);
    setAmplitude(DEFAULT_AMPLITUDE);
  } 

  void changeParameter(int parameterNum, int amt) override
  { 
    switch (parameterNum)
    {
      case 0: setAmplitude(m_Amplitude + (amt / 100.0f)); break;
    }
  }

  void setAmplitude(float amplitude) 
  { 
    amplitude = std::clamp(amplitude, 0.0f, MAX_AMPLITUDE);
    m_Amplitude = amplitude;
    m_DC->amplitude(m_Amplitude);
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
    //Format: DC{amplitude}[row,col]<ID>
    int openBracket = s.find("[");
    int positionComma = s.find(",", s.find("["));

    float amplitude = Serialize::extractFloatBetween("{", "}", s);
    int row =         Serialize::extractIntBetween(openBracket, positionComma, s);
    int col =         Serialize::extractIntBetween(positionComma, s.find("]"), s);
    int ID =          Serialize::extractIntBetween("<", ">", s);

    DCGenerator* dc = new DCGenerator();
    dc->setID(ID);
    dc->setAmplitude(amplitude);
    dc->getLEDElement().setPosition(row, col);
    return dc;
  }

private:
  AudioSynthWaveformDc* m_DC;
  float m_Amplitude;
};