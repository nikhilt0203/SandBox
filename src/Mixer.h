#include "Module.h"
#pragma once

class Mixer : public Module 
{ 
public:
  static constexpr int MAX_SOURCES = 4;
  static constexpr int MAX_DESTINATIONS = 1;
  static constexpr int NUM_CHANNELS = 4;

  static constexpr float MAX_GAIN = 3.0f;

  static constexpr ModuleInfo INFO = {
    Type::MIXER,
    "Mixer",
    "4 channel mixer.",
    {"Ch 1", "Ch 2", "Ch 3", "Ch 4"},
    {"1", "2", "3", "4"}, {}
  };
  
public: 
  Mixer(int row, int col) 
  : Module(new AudioMixer4(), INFO.NAME, MAX_SOURCES, MAX_DESTINATIONS, row, col)
  { 
    m_Mixer = static_cast<AudioMixer4*>(m_Device); 
    m_Type = INFO.TYPE;
    setColor(m_Type);
    m_UIElement = ModuleUIElement(INFO.NAME, m_Color, INFO.PARAMETER_NAMES);
    m_Ports.setInputNames(INFO.INPUT_PORT_NAMES);

    for (int i = 0; i < NUM_CHANNELS; i++) 
      setGain(i, 1.0); 
  } 
  
  void changeParameter(int parameterNum, int amt) 
  { 
    setGain(parameterNum, m_ChannelGains[parameterNum] * powf(1.10f, amt));
  } 
  
  void setGain(int channel, float gain) 
  { 
    if (channel < 0 || channel > NUM_CHANNELS - 1) 
      return;

    if (gain < 0.0 || gain > MAX_GAIN) 
      return;
      
    m_ChannelGains[channel] = gain; 
    m_Mixer->gain(channel, gain); 
    m_UIElement.updateParameter(channel, gain, MAX_GAIN);
  }

  std::string toString() override
  {
    m_ID.makeSerializedID(INFO.NAME, m_Row, m_Col, NUM_CHANNELS);
    for (int i = 0; i < NUM_CHANNELS; i++)
    {
      m_ID.addParam(m_ChannelGains[i]);
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

    float gain0 = Serialize::extractFloatBetween("{", ",", s);
    float gain1 = Serialize::extractFloatBetween(firstComma, secondComma, s);
    float gain2 = Serialize::extractFloatBetween(secondComma, thirdComma, s);
    float gain3 = Serialize::extractFloatBetween(thirdComma, s.find("}"), s);

    int row = Serialize::extractIntBetween(s.find("["), positionComma, s);
    int col = Serialize::extractIntBetween(positionComma, s.find("]"), s);
    int ID =  Serialize::extractIntBetween("<", ">", s);

    Mixer* mixer = new Mixer(row, col);
    mixer->setID(ID);
    mixer->setGain(0, gain0);
    mixer->setGain(1, gain1);
    mixer->setGain(2, gain2);
    mixer->setGain(3, gain3);
    return mixer;
  }

private: 
  AudioMixer4* m_Mixer; 
  float m_ChannelGains[NUM_CHANNELS]; 
};