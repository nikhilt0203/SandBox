#include "ModuleUIElement.h"
#include "Colors.h"
#include <Arduino.h>

ModuleUIElement::ModuleUIElement(ModuleConfig::Info info)
{
  updateColor(Colors::getColor(info.TYPE));
  m_DisplayElement.parameters.reserve(info.PARAMETER_NAMES.size());
  m_DisplayElement.displayName = info.NAME;

  int parameterNum = 0;
  for (const char* name: info.PARAMETER_NAMES)
  {
    m_DisplayElement.parameters.push_back({name, 0, 0.0, parameterNum});
    parameterNum++;
  }
}

ModuleUIElement::ModuleUIElement() 
: m_LEDElement(), 
  m_DisplayElement(), 
  m_Color(0) 
{}

void ModuleUIElement::update(ModuleConfig::Info info)
{
  updateColor(Colors::getColor(info.TYPE));
  m_DisplayElement.displayName = info.NAME;
  m_DisplayElement.parameters.clear();
  m_DisplayElement.parameters.reserve(info.PARAMETER_NAMES.size());

  int parameterNum = 0;
  for (const char* name: info.PARAMETER_NAMES)
  {
    m_DisplayElement.parameters.push_back({name, 0, 0.0, parameterNum});
    parameterNum++;
  }
}

void ModuleUIElement::updateColor(uint32_t color) 
{ 
  m_LEDElement.color = color;
  notifyChange<LEDElement>(); 

  m_DisplayElement.color = color;
  notifyChange<DisplayElement>();

  m_Color = color;
}

void ModuleUIElement::updateParameter(int parameterNum, int value, int maxValue) 
{ 
  m_DisplayElement.parameters[parameterNum].value = value; 
  m_DisplayElement.parameters[parameterNum].percentage = static_cast<float>(value) / maxValue; 
  m_DisplayElement.isUpdated = true;
}

void ModuleUIElement::updateParameter(int parameterNum, float value, float maxValue) 
{ 
  m_DisplayElement.parameters[parameterNum].value = value; 
  m_DisplayElement.parameters[parameterNum].percentage = value / maxValue;
  m_DisplayElement.isUpdated = true;
}
