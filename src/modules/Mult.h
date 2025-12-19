#include "Module.h"
#include "Serialize.h"
#include "SDManager.h"
#pragma once

class Mult : public Module 
{ 
public:
  MODULE_INFO(
    MULT,
    "Mult",
    "Copies an input signal up to 8 times",
    MODULE_PARAMS(),
    MODULE_INPUTS(),
    MODULE_OUTPUTS()
  );

  static constexpr size_t MAX_INPUTS = 1;
  static constexpr size_t MAX_OUTPUTS = 8;

public: 
  Mult() 
  : Module(new AudioAmplifier(), INFO.NAME, MAX_INPUTS, MAX_OUTPUTS)
  { 
    m_Type = INFO.TYPE;
    m_UIElement = ModuleUIElement(INFO);
    for (size_t i{}; i < MAX_OUTPUTS; i++) 
    {
      m_Ports.setOutputDevicePort(0, i);
    }
  } 

  std::string_view serialize() override 
  { 
    return m_ID.makeSerializationID(m_UIElement); 
  }

  static Module* buildFromString(std::string_view s) 
  {
    //Format: Mult{}[row,col]<ID>
    int positionComma = s.find(",", s.find("["));

    int row = Serialize::extractIntBetween(s.find("["), positionComma, s);
    int col = Serialize::extractIntBetween(positionComma, s.find("]"), s);
    int ID =  Serialize::extractIntBetween("<", ">", s);

    Mult* mult = new Mult();
    mult->setID(ID);
    mult->getLEDElement().setPosition(row, col);
    return mult;
  }
};