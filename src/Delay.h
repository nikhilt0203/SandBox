#include "Module.h"
#pragma once

class Delay : public Module 
{ 
public:
  static constexpr int MAX_SOURCES = 1;
  static constexpr int MAX_DESTINATIONS = 4;
  static constexpr int NUM_DELAYS = 4;
  static constexpr float MAX_DELAY_MS = 800.0f;
  static constexpr float MIN_DELAY_MS = 1.0f;
  static constexpr float MS_CURVE = 1.10f;

  static constexpr Type TYPE = Module::Type::DELAY;
  static constexpr auto NAME = "Delay";
  static constexpr auto PARAMETER_NAMES = {"D1", "D2", "D3", "D4"};
public: 
  Delay(int row, int col) 
  : Module(new AudioEffectDelay(), NAME, MAX_SOURCES, MAX_DESTINATIONS, row, col) 
  { 
    m_Type = TYPE;
    m_Delay = static_cast<AudioEffectDelay*>(m_Device); 
    m_UIElement = ModuleUIElement(NAME, m_Color, PARAMETER_NAMES);
    setColor(m_Type);

    for (int i = 0; i < NUM_DELAYS; i++) 
      setDelayTime(i, 100.0 * i + 1.0); 
  } 
  
  void changeParameter(int parameterNum, int amt) 
  { 
    setDelayTime(parameterNum, m_DelayTimes[parameterNum] * powf(1.10f, amt));
  } 
  
  void setDelayTime(int channel, float milliseconds) 
  { 
    if (channel < 0 || channel >= NUM_DELAYS) 
      return;

    if (milliseconds < MIN_DELAY_MS) 
      milliseconds = MIN_DELAY_MS;

    if (milliseconds > MAX_DELAY_MS) 
      milliseconds = MAX_DELAY_MS;

    m_DelayTimes[channel] = milliseconds;
    m_Delay->delay(channel, milliseconds);
    m_UIElement.updateParameter(channel, milliseconds, MAX_DELAY_MS);
  }

  std::string toString() override
  {
    m_ID.makeSerializedID("Delay", m_Row, m_Col, NUM_DELAYS);
    for (int i = 0; i < NUM_DELAYS; i++)
    {
      m_ID.addParam(m_DelayTimes[i]);
    }
    return m_ID.getSerializedID();
  }

  static Module* buildFromString(std::string s) 
  {
    //Format: Mixer{gain0,gain1,gain2,gain3}[row,col]<ID>
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

    Delay* delay = new Delay(row, col);
    delay->setID(ID);
    delay->setDelayTime(0, delay0);
    delay->setDelayTime(1, delay1);
    delay->setDelayTime(2, delay2);
    delay->setDelayTime(3, delay3);
    return delay;
  }
private: 
  AudioEffectDelay* m_Delay; 
  float m_DelayTimes[NUM_DELAYS]; 
};