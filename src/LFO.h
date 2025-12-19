#include "Oscillator.h"
#include "Port.h"
#pragma once

class LFO : public Oscillator
{
public:
  static constexpr float DEFAULT_FREQUENCY = 2.0;

  static constexpr ModuleInfo INFO =
  {
    Type::LFO,
    "LFO",
    "Low frequency oscillator", 
    {},
    {Port::FM_IN_NAME, "wv"},
    {Port::CV_OUT_NAME}
  };
  
public:
  LFO(int row, int col, int waveform = DEFAULT_WAVEFORM, float frequency = DEFAULT_FREQUENCY, 
  float octaveControl = DEFAULT_OCTAVE_CONTROL) 
  : Oscillator(row, col, frequency, waveform, octaveControl) 
  {
    m_Type = Module::Type::LFO;
    setColor(m_Type);
    m_Ports.setInputNames(INFO.INPUT_PORT_NAMES);
    m_Ports.setOutputNames(INFO.OUTPUT_PORT_NAMES);
    setName(INFO.NAME);
  }

  std::string toString() override
  {
    m_ID.makeSerializedID(INFO.NAME, m_Row, m_Col, 3);
    m_ID.addParam(m_Waveform);
    m_ID.addParam(m_Frequency);
    m_ID.addParam(m_OctaveControl);
    return m_ID.getSerializedID();
  }

  static Module* buildFromString(std::string s) 
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

    LFO* lfo = new LFO(row, col, waveform, frequency, octaveControl);
    lfo->setID(ID);

    return lfo;
  }
};