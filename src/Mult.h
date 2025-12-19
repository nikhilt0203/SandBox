#include "Module.h"
#include "Serialize.h"
#include "SDManager.h"
#pragma once

class Mult : public Module 
{ 
public:
  static constexpr int MAX_SOURCES = 1;
  static constexpr int MAX_DESTINATIONS = 8;

  static constexpr ModuleInfo INFO = 
  {
    Type::MULT,
    "Mult",
    "Copies an input signal up to 8 times.",
    {}, {}, {}
  };
public: 
  Mult(int row, int col) 
  : Module(new AudioAmplifier(), INFO.NAME, MAX_SOURCES, MAX_DESTINATIONS, row, col)
  { 
    m_Type = INFO.TYPE;
    setColor(m_Type);
  } 

  int getOpenOutputPort() const override { return m_Ports.outputsFull() ? -1 : 0; }

  std::string toString() override { return m_ID.makeSerializedID(INFO.NAME, m_Row, m_Col); }

  static Module* buildFromString(std::string s) 
  {
    //Format: Mult{}[row,col]<ID>
    int positionComma = s.find(",", s.find("["));

    int row = Serialize::extractIntBetween(s.find("["), positionComma, s);
    int col = Serialize::extractIntBetween(positionComma, s.find("]"), s);
    int ID =  Serialize::extractIntBetween("<", ">", s);

    Mult* mult = new Mult(row, col);
    mult->setID(ID);
    return mult;
  }
};