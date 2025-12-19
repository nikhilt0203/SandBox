#include "Module.h"
#pragma once

class Bitcrusher : public Module 
{ 
public:
  static constexpr ModuleConfig::Info INFO = { 
    ModuleConfig::Type::BITCRUSHER, 
    "Bitcrusher",
    "Reduces the bit depth of the input    signal", 
    { "Bits" }, 
    { Port::AUDIO_IN_NAME, Port::CV_IN_NAME }, 
    {}
  };

public:
  static constexpr size_t MAX_INPUTS = 2;
  static constexpr size_t MAX_OUTPUTS = 1;

  static constexpr int MIN_BITS = 1;
  static constexpr int MAX_BITS = 16;
  static constexpr int DEFAULT_BITS = 4;

public: 
  Bitcrusher() 
  : Module(new AudioEffectBitcrusher(), INFO.NAME, MAX_INPUTS, MAX_OUTPUTS)
  {
    m_Type = INFO.TYPE;
    m_UIElement = ModuleUIElement(INFO);

    m_Bitcrusher = static_cast<AudioEffectBitcrusher*>(m_Device);
    m_BitsCV = new AudioCVInput([this](float value) { this->setBits(static_cast<int>(value)); }, MIN_BITS, MAX_BITS);

    m_Ports.setInputNames(INFO.INPUT_PORT_NAMES);
    m_Ports.setInputDevice(0, m_Bitcrusher, 0);
    m_Ports.setInputDevice(1, m_BitsCV, 0);
    setBits(DEFAULT_BITS);
  } 

  void changeParameter(int parameterNum, int amt) override
  { 
    switch (parameterNum)
    {
      case 0: setBits(m_Bits + amt); break;
    }
  } 

  void setBits(int bits) 
  { 
    m_Bits = std::clamp(bits, MIN_BITS, MAX_BITS);
    m_Bitcrusher->bits(m_Bits);
    m_UIElement.updateParameter(0, m_Bits - MIN_BITS, MAX_BITS - MIN_BITS);
  }

  std::string_view serialize() override
  {
    m_ID.makeSerializationID(m_UIElement);
    m_ID.addParam(m_Bits);
    return m_ID.getSerializationID();
  }

  static Module* buildFromString(std::string_view s) 
  {
    //Format: Bitcrusher{bits}[row,col]<ID>
    int openBracket = s.find("[");
    int positionComma = s.find(",", openBracket);

    int bits = Serialize::extractIntBetween("{", "}", s);
    int row =  Serialize::extractIntBetween(openBracket, positionComma, s);
    int col =  Serialize::extractIntBetween(positionComma, s.find("]"), s);
    int ID =   Serialize::extractIntBetween("<", ">", s);

    Bitcrusher* bitcrusher = new Bitcrusher();
    bitcrusher->setID(ID);
    bitcrusher->setBits(bits);
    bitcrusher->getLEDElement().setPosition(row, col);
    return bitcrusher;
  }

private:
  AudioEffectBitcrusher* m_Bitcrusher; 
  AudioCVInput* m_BitsCV;
  int m_Bits;
};