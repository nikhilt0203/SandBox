#pragma once
#include <vector>
#include <string>
#include <memory>
#include <variant>

class ModuleUIElement
{
public:
  struct LEDElement
  {
    uint32_t color;
    bool isUpdated = false;
  };

  struct Parameter
  {
    const char* name;
    std::variant<float, int> value;
    float percentage;
    int parameterNum;
  };

  struct DisplayElement
  {
    const char* displayName;
    std::vector<Parameter> parameters;
    bool isUpdated = false;
  };
  
public:
  ModuleUIElement();

  ModuleUIElement(const char* displayName, uint32_t color, std::initializer_list<const char*> parameterNames);

  void updateColor(uint32_t color);

  void updateDisplayName(const char* name);

  const char* getDisplayName() const;

  const char* getParameterName(int parameterNum) const;

  std::variant<float, int> getParameterValue(int parameterNum) const;

  const LEDElement& getLEDElement() const;

  const DisplayElement& getDisplayElement() const;

  void updateParameter(int parameterNum, int value, int maxValue);

  void updateParameter(int parameterNum, float value, float maxValue);

private:
  LEDElement m_LEDElement;
  DisplayElement m_DisplayElement;
};