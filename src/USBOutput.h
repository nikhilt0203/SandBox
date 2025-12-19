#include "Module.h"
#pragma once

class USBOutput : public Module
{
public:
  static constexpr int MAX_SOURCES = 1;
  static constexpr int MAX_DESTINATIONS = 0; 
  static constexpr const char* NAME = "Audio out";
  static constexpr const char* SERIALIZATION_NAME = "USB Output";
public:
  USBOutput(int row, int col) 
  : Module(new AudioOutputUSB(), NAME, MAX_SOURCES, MAX_DESTINATIONS, row, col) 
  {
    setColor(Module::Type::USBOUT);
  }

  //Format: USBOutput{}[row,col]<ID>
  std::string toString() override { return m_ID.makeSerializedID(SERIALIZATION_NAME, m_Row, m_Col); }

  static Module* buildFromString(std::string s) 
  {
    //Format: USBOutput{}[row,col]<ID>
    int positionComma = s.find(",", s.find("["));

    int row = Serialize::extractIntBetween(s.find("["), positionComma, s);
    int col = Serialize::extractIntBetween(positionComma, s.find("]"), s);
    int ID  = Serialize::extractIntBetween("<", ">", s);

    USBOutput* usbOut = new USBOutput(row, col);
    usbOut->setID(ID);
    return usbOut;
  }
};