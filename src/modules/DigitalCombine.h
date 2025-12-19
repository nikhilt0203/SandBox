#include "Module.h"
#pragma once

class DigitalCombine : public Module 
{ 
public:
  static constexpr ModuleConfig::Info INFO = {
    ModuleConfig::Type::COMBINE,
    "Combine",
    "Combines two signals using digital  logic",
    {"Mode"}, {"A", "B"}, {}
  };

  static constexpr size_t MAX_SOURCES = 2;
  static constexpr size_t MAX_DESTINATIONS = 1;

  static constexpr int NUM_MODES = 4;

public: 
  DigitalCombine() : 
  Module(new AudioEffectDigitalCombine(), INFO.NAME, MAX_SOURCES, MAX_DESTINATIONS) 
  { 
    m_Combine = static_cast<AudioEffectDigitalCombine*>(m_Device); 
    m_Type = INFO.TYPE;
    m_UIElement = ModuleUIElement(INFO);
    setCombineMode(0);
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
    m_CombineMode = std::clamp(mode, 0, NUM_MODES - 1);
    m_Combine->setCombineMode(m_CombineMode);
    m_UIElement.updateParameter(0, m_CombineMode, NUM_MODES - 1);
  }

  std::string_view serialize() override
  {
    m_ID.makeSerializationID(m_UIElement);
    m_ID.addParam(m_CombineMode);
    return m_ID.getSerializationID();
  }

  static Module* buildFromString(std::string_view s) 
  {
    //Format: Combine{combineMode}[row,col]<ID>
    int openBracket = s.find("[");
    int positionComma = s.find(",", s.find("["));

    int combineMode = Serialize::extractIntBetween("{", "}", s);
    int row =         Serialize::extractIntBetween(openBracket, positionComma, s);
    int col =         Serialize::extractIntBetween(positionComma, s.find("]"), s);
    int ID =          Serialize::extractIntBetween("<", ">", s);

    DigitalCombine* combine = new DigitalCombine();
    combine->setID(ID);
    combine->setCombineMode(combineMode);
    combine->getLEDElement().setPosition(row, col);
    return combine;
  }

private:
  void changeCombineMode(int i)
  {
    static int gate = 0;
    gate = gate + i;
    if (gate < -2 || gate > 2) { return; }
    gate = 0;

    int newMode = m_CombineMode + i;
    newMode = (newMode + 4) % 4;
    setCombineMode(newMode);
  }

private: 
  AudioEffectDigitalCombine* m_Combine; 
  int m_CombineMode;
};