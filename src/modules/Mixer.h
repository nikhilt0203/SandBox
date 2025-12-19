#include "Module.h"
#pragma once

class Mixer : public Module 
{ 
public:
  MODULE_INFO(
    MIXER,
    "Mixer",
    "4 channel mixer",
    MODULE_PARAMS("Ch 1", "Ch 2", "Ch 3", "Ch 4"),
    MODULE_INPUTS("1", "2", "3", "4"),
    MODULE_OUTPUTS()
  );

  static constexpr size_t MAX_SOURCES = 4;
  static constexpr size_t MAX_DESTINATIONS = 1;

  static constexpr size_t NUM_CHANNELS = 4;

  static constexpr float MAX_GAIN_ = 3.0f;
  
public: 
  Mixer() 
  : Module(new AudioMixer4(), INFO.NAME, MAX_SOURCES, MAX_DESTINATIONS)
  { 
    m_Mixer = static_cast<AudioMixer4*>(m_Device); 
    m_Type = INFO.TYPE;
    m_UIElement = ModuleUIElement(INFO);
    m_Ports.setInputNames(INFO.INPUT_PORT_NAMES);

    for (size_t i{}; i < NUM_CHANNELS; i++)
    {
      setGain(i, 1.0); 
    }
  } 
  
  void changeParameter(int parameterNum, int amt) 
  { 
    setGain(parameterNum, m_ChannelGains[parameterNum] * powf(1.10f, amt));
  } 
  
  void setGain(int channel, float gain) 
  { 
    channel = std::clamp(channel, 0, static_cast<int>(NUM_CHANNELS - 1));
    gain = std::clamp(gain, 0.0f, MAX_GAIN_);
    m_ChannelGains[channel] = gain; 
    m_Mixer->gain(channel, gain); 
    m_UIElement.updateParameter(channel, gain, MAX_GAIN_);
  }

  std::string_view serialize() override
  {
    m_ID.makeSerializationID(m_UIElement);
    for (size_t i{}; i < NUM_CHANNELS; i++)
    {
      m_ID.addParam(m_ChannelGains[i]);
    }
    return m_ID.getSerializationID();
  }

  static Module* buildFromString(std::string_view s) 
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

    Mixer* mixer = new Mixer();
    mixer->setID(ID);
    mixer->setGain(0, gain0);
    mixer->setGain(1, gain1);
    mixer->setGain(2, gain2);
    mixer->setGain(3, gain3);
    mixer->getLEDElement().setPosition(row, col);
    return mixer;
  }

private: 
  AudioMixer4* m_Mixer; 
  float m_ChannelGains[NUM_CHANNELS]; 
};