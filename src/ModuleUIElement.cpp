#include "ModuleUIElement.h"
#include <Arduino.h>

ModuleUIElement::ModuleUIElement(const char* displayName, uint32_t color, std::initializer_list<const char*> parameterNames)
{
  updateColor(color);
  m_DisplayElement.parameters.reserve(parameterNames.size());
  m_DisplayElement.displayName = displayName;

  int parameterNum = 0;
  for (const char* name: parameterNames)
  {
    m_DisplayElement.parameters.push_back({name, 0, 0.0, parameterNum});
    parameterNum++;
  }
}

ModuleUIElement::ModuleUIElement()
{
  m_DisplayElement.displayName = nullptr;
  m_DisplayElement.isUpdated = false;
  m_DisplayElement.parameters.clear();
  m_LEDElement.color = 0;
  m_LEDElement.isUpdated = false;
}

void ModuleUIElement::updateColor(uint32_t color) { m_LEDElement.color = color; }

void ModuleUIElement::updateDisplayName(const char* name) { m_DisplayElement.displayName = name; }

const char* ModuleUIElement::getDisplayName() const { return m_DisplayElement.displayName; }

const char* ModuleUIElement::getParameterName(int parameterNum) const { return m_DisplayElement.parameters[parameterNum].name; }

std::variant<float, int> ModuleUIElement::getParameterValue(int parameterNum) const { return m_DisplayElement.parameters[parameterNum].value; }

const ModuleUIElement::LEDElement& ModuleUIElement::getLEDElement() const { return m_LEDElement; }

const ModuleUIElement::DisplayElement& ModuleUIElement::getDisplayElement() const { return m_DisplayElement; }

void ModuleUIElement::updateParameter(int parameterNum, int value, int maxValue) 
{ 
  m_DisplayElement.parameters[parameterNum].value = value; 
  m_DisplayElement.parameters[parameterNum].percentage = (float)value / maxValue; 
  m_DisplayElement.isUpdated = true;
}

void ModuleUIElement::updateParameter(int parameterNum, float value, float maxValue) 
{ 
  m_DisplayElement.parameters[parameterNum].value = value; 
  m_DisplayElement.parameters[parameterNum].percentage = value / maxValue;
  m_DisplayElement.isUpdated = true;
}