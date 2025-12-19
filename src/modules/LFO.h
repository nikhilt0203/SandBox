#include "modules/Oscillator.h"
#include "Port.h"
#include "Colors.h"
#pragma once

class LFO : public Oscillator
{
public:
  static constexpr ModuleConfig::Info INFO = { 
    ModuleConfig::Type::LFO, 
    "LFO", 
    "Low frequency oscillator",
    { "Coarse", "Fine", "FM", "Wave" }, 
    { Port::FM_IN_NAME, "wv" }, 
    { Port::CV_OUT_NAME }
  };

  static constexpr float DEFAULT_FREQUENCY = 2.0f;
  
public:
  LFO(int waveform = DEFAULT_WAVEFORM, float frequency = DEFAULT_FREQUENCY, 
  float octaveControl = DEFAULT_OCTAVE_CONTROL) 
  : Oscillator()
  {
    m_Type = INFO.TYPE; 
    m_ID.setName(INFO.NAME);
    m_UIElement.update(INFO);

    setFrequency(frequency);
    setWaveform(waveform);
    setOctaveControl(octaveControl);
  
    m_Ports.setOutputNames(INFO.OUTPUT_PORT_NAMES);
    m_UIElement.updateColor(Colors::getColor(INFO.TYPE));
  }

  std::string_view serialize() override
  {
    m_ID.makeSerializationID(m_UIElement);
    m_ID.addParam(m_Waveform);
    m_ID.addParam(m_Frequency);
    m_ID.addParam(m_OctaveControl);
    return m_ID.getSerializationID();
  }

  static Module* buildFromString(std::string_view s) 
  {
    //Format: LFO{waveform,frequency,octaveControl}[row,col]<ID>
    size_t firstComma = s.find(",");
    size_t secondComma = s.find(",", firstComma + 1);
    size_t positionComma = s.find(",", s.find("["));

    int waveform =        Serialize::extractIntBetween("{", ",", s);
    float frequency =     Serialize::extractFloatBetween(firstComma, secondComma, s);
    float octaveControl = Serialize::extractFloatBetween(secondComma, s.find("}"), s);
    int row =             Serialize::extractIntBetween(s.find("["), positionComma, s);
    int col =             Serialize::extractIntBetween(positionComma, s.find("]"), s);
    int ID  =             Serialize::extractIntBetween("<", ">", s);

    LFO* lfo = new LFO(waveform, frequency, octaveControl);
    lfo->setID(ID);
    lfo->getLEDElement().setPosition(row, col);

    return lfo;
  }
};