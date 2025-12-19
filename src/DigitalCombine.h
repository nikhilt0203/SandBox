#include "Module.h"
#pragma once

class DigitalCombine : public Module 
{ 
public:
  static constexpr int MAX_SOURCES = 2;
  static constexpr int MAX_DESTINATIONS = 1;
  static constexpr const char* NAME = "Combine";
public: 
  DigitalCombine(int row, int col) : 
  Module(new AudioEffectDigitalCombine(), NAME, MAX_SOURCES, MAX_DESTINATIONS, row, col) 
  { 
    m_Combine = static_cast<AudioEffectDigitalCombine*>(m_Device); 
    setCombineMode(0);
    m_Type = Module::Type::COMBINE;
    setColor(m_Type);
  } 
  
  void changeParameter(int parameterNum, int amt) override
  { 
    switch (parameterNum)
    {
      case 0: changeCombineMode(amt); break;
    }
  } 
  
  void setCombineMode(int mode) 
  { 
    if (mode < 0 || mode > 3) 
    {
      Serial.println("Error: Invalid combine mode.");
      return;
    }
    m_CombineMode = mode;
    m_Combine->setCombineMode(m_CombineMode);
  }

  std::string toString() override
  {
    m_ID.makeSerializedID(NAME, m_Row, m_Col, 1);
    m_ID.addParam(m_CombineMode);
    return m_ID.getSerializedID();
  }

  static Module* buildFromString(std::string s) 
  {
    //Format: Combine{combineMode}[row,col]<ID>
    int openBracket = s.find("[");
    int positionComma = s.find(",", s.find("["));

    int combineMode = Serialize::extractIntBetween("{", "}", s);
    int row =         Serialize::extractIntBetween(openBracket, positionComma, s);
    int col =         Serialize::extractIntBetween(positionComma, s.find("]"), s);
    int ID =          Serialize::extractIntBetween("<", ">", s);

    DigitalCombine* combine = new DigitalCombine(row, col);
    combine->setID(ID);
    combine->setCombineMode(combineMode);
    return combine;
  }

private:
  void changeCombineMode(int i)
  {
    //Trigger change after being called twice
    static int gate = 0;
    gate = gate + i;
    if (gate < -2 || gate > 2) return;
    gate = 0;

    int newMode = m_CombineMode + i;
    newMode = (newMode + 4) % 4;
    setCombineMode(newMode);
  }

private: 
  AudioEffectDigitalCombine* m_Combine; 
  int m_CombineMode;
};