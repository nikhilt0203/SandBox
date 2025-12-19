#include "Module.h"
#pragma once

class Delay : public Module 
{ 
public:
  static constexpr size_t MAX_INPUTS = 1;
  static constexpr size_t MAX_OUTPUTS = 4;
  static constexpr size_t NUM_DELAYS = 4;
  static constexpr float MAX_DELAY_MS = 800.0f;
  static constexpr float MIN_DELAY_MS = 1.0f;
  static constexpr float MS_CURVE = 1.10f;

  static constexpr ModuleConfig::Info INFO = 
  {
    ModuleConfig::Type::DELAY,
    "Delay",
    "4 channel delay",
    {"D1", "D2", "D3", "D4"},
    {},
    {"D1", "D2", "D3", "D4"}
  };
public: 
  Delay() 
  : Module(new AudioEffectDelay(), INFO.NAME, MAX_INPUTS, MAX_OUTPUTS) 
  { 
    m_Type = INFO.TYPE;
    m_Delay = static_cast<AudioEffectDelay*>(m_Device); 
    m_UIElement = ModuleUIElement(INFO);

    for (size_t i{}; i < NUM_DELAYS; i++)
    {
      setDelayTime(i, 100.0 * i + 1.0); 
    } 
  }

  void changeParameter(int parameterNum, int amt) 
  { 
    setDelayTime(parameterNum, m_DelayTimes[parameterNum] * powf(1.10f, amt));
  } 
  
  void setDelayTime(int channel, float ms) 
  { 
    channel = std::clamp(channel, 0, static_cast<int>(NUM_DELAYS - 1));
    ms = std::clamp(ms, MIN_DELAY_MS, MAX_DELAY_MS);
    m_DelayTimes[channel] = ms;
    m_Delay->delay(channel, ms);
    m_UIElement.updateParameter(channel, ms, MAX_DELAY_MS);
  }

  std::string_view serialize() override
  {
    m_ID.makeSerializationID(m_UIElement);
    for (size_t i{}; i < NUM_DELAYS; i++)
    {
      m_ID.addParam(m_DelayTimes[i]);
    }
    return m_ID.getSerializationID();
  }

  static Module* buildFromString(std::string_view s) 
  {
    //Format: Delay{ms0,ms1,ms2,ms3}[row,col]<ID>
    int firstComma    = s.find(",");
    int secondComma   = s.find(",", firstComma + 1);
    int thirdComma    = s.find(",", secondComma + 1);
    int openBracket   = s.find("[");
    int positionComma = s.find(",", openBracket);

    float delay0 = Serialize::extractFloatBetween("{", ",", s);
    float delay1 = Serialize::extractFloatBetween(firstComma, secondComma, s);
    float delay2 = Serialize::extractFloatBetween(secondComma, thirdComma, s);
    float delay3 = Serialize::extractFloatBetween(thirdComma, s.find("}"), s);

    int row = Serialize::extractIntBetween(s.find("["), positionComma, s);
    int col = Serialize::extractIntBetween(positionComma, s.find("]"), s);
    int ID =  Serialize::extractIntBetween("<", ">", s);

    Delay* delay = new Delay();
    delay->setID(ID);
    delay->setDelayTime(0, delay0);
    delay->setDelayTime(1, delay1);
    delay->setDelayTime(2, delay2);
    delay->setDelayTime(3, delay3);
    delay->getLEDElement().setPosition(row, col);
    return delay;
  }
  
private: 
  AudioEffectDelay* m_Delay; 
  float m_DelayTimes[NUM_DELAYS]; 
};