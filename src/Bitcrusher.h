#include "Module.h"
#pragma once

class Bitcrusher : public Module 
{ 
public:
  static constexpr int MAX_SOURCES = 2;
  static constexpr int MAX_DESTINATIONS = 1;

  static constexpr int MIN_BITS = 1;
  static constexpr int MAX_BITS = 16;

  static constexpr Type TYPE = Type::BITCRUSHER;
  static constexpr auto NAME = "Bitcrusher";
  static constexpr auto PARAMETER_NAMES = {"Bits"};
public: 
  Bitcrusher(int row, int col) 
  : Module(new AudioEffectBitcrusher(), NAME, MAX_SOURCES, MAX_DESTINATIONS, row, col) 
  { 
    m_Type = TYPE;
    m_Bitcrusher = static_cast<AudioEffectBitcrusher*>(m_Device);
    
    m_UIElement = ModuleUIElement(NAME, m_Color, PARAMETER_NAMES);
    setBits(3);
    setColor(m_Type);
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
    if (bits < MIN_BITS) bits = MIN_BITS;
    if (bits > MAX_BITS) bits = MAX_BITS;

    m_Bits = bits;
    m_Bitcrusher->bits(m_Bits);
    m_UIElement.updateParameter(0, m_Bits - MIN_BITS, MAX_BITS - MIN_BITS);
  }

  std::string toString() override
  {
    m_ID.makeSerializedID("Bitcrusher", m_Row, m_Col, 1);
    m_ID.addParam(m_Bits);
    return m_ID.getSerializedID();
  }

  static Module* buildFromString(std::string s) 
  { 
    //Format: Bitcrusher{bits}[row,col]<ID>
    int openBracket = s.find("[");
    int positionComma = s.find(",", openBracket);

    int bits = Serialize::extractIntBetween("{", "}", s);
    int row =  Serialize::extractIntBetween(openBracket, positionComma, s);
    int col =  Serialize::extractIntBetween(positionComma, s.find("]"), s);
    int ID =   Serialize::extractIntBetween("<", ">", s);

    Bitcrusher* bitcrusher = new Bitcrusher(row, col);
    bitcrusher->setID(ID);
    bitcrusher->setBits(bits);
    return bitcrusher;
  }

private:
  AudioEffectBitcrusher* m_Bitcrusher; 
  int m_Bits;
};