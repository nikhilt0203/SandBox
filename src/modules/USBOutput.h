#include "Module.h"
#pragma once

class USBOutput : public Module
{
public:
  static constexpr ModuleConfig::Info INFO = { 
    ModuleConfig::Type::USBOUT, 
    "Audio Out", 
    "Outputs audio via USB", 
    { }, { }, { } 
  };

  static constexpr size_t MAX_INPUTS = 1;
  static constexpr size_t MAX_OUTPUTS = 0; 

public:
  USBOutput::USBOutput() 
  : Module(new AudioOutputUSB(), INFO.NAME, MAX_INPUTS, MAX_OUTPUTS)
  {
    m_Type = INFO.TYPE;
    m_UIElement = ModuleUIElement(INFO);
  }

  std::string_view serialize() override { return m_ID.makeSerializationID(m_UIElement); };

public:
  static Module* buildFromString(std::string_view s) 
  {
    //Format: Audio Out{}[row,col]<ID>
    int positionComma = s.find(",", s.find("["));

    int row = Serialize::extractIntBetween(s.find("["), positionComma, s);
    int col = Serialize::extractIntBetween(positionComma, s.find("]"), s);
    int ID  = Serialize::extractIntBetween("<", ">", s);

    USBOutput* usbOut = new USBOutput();
    usbOut->setID(ID);
    usbOut->getLEDElement().setPosition(row, col);
    return usbOut;
  }
};