#pragma once
#include <Arduino.h>
#include <vector>
#include <string>
#include <memory>
#include "ModuleConfig.h"
#include <variant>

class ModuleUIElement
{
public:
  struct LEDElement
  {
    uint32_t color = 0;
    int row = 0;
    int col = 0;
    bool isUpdated = false;

    void setPosition(int r, int c) 
    { 
      row = r;
      col = c; 
    }
  };

  struct Parameter
  {
    const char* name = nullptr;
    std::variant<float, int> value;
    float percentage{};
    int parameterNum{};
  };

  struct DisplayElement
  {
    uint32_t color = 0;
    const char* displayName = nullptr;
    std::vector<Parameter> parameters;
    bool isUpdated = false;
  };
  
public:
  ModuleUIElement(ModuleConfig::Info info);

  ModuleUIElement();

  void updateColor(uint32_t color);

  void update(ModuleConfig::Info info);

  void updateParameter(int parameterNum, int value, int maxValue);

  void updateParameter(int parameterNum, float value, float maxValue);

  const char* getDisplayName() const { return m_DisplayElement.displayName; }

  const char* getParameterName(int parameterNum) const;

  std::variant<float, int> getParameterValue(int parameterNum) const;

  LEDElement& getLEDElement() { return m_LEDElement; }

  DisplayElement& getDisplayElement() { return m_DisplayElement; }

  uint32_t getColor() const { return m_Color; };

  template<typename T>
  void notifyChange()
  {
    static_assert(std::is_same<T, LEDElement>::value || std::is_same<T, DisplayElement>::value, 
      "Template parameter must be LEDElement or DisplayElement");

    if constexpr (std::is_same<T, LEDElement>::value) 
    { 
      this->m_LEDElement.isUpdated = false; 
    }
    else if constexpr (std::is_same<T, DisplayElement>::value) 
    { 
      this->m_DisplayElement.isUpdated = false; 
    }
  }

  template<typename T>
  void markAsUpdated()
  {
    static_assert(std::is_same<T, LEDElement>::value || std::is_same<T, DisplayElement>::value, 
      "Template parameter must be LEDElement or DisplayElement");

    if constexpr (std::is_same<T, LEDElement>::value) 
    { 
      this->m_LEDElement.isUpdated = true; 
    }
    else if constexpr (std::is_same<T, DisplayElement>::value) 
    { 
      this->m_DisplayElement.isUpdated = true; 
    }
  }

  bool isUpdated() const 
  { 
    return m_DisplayElement.isUpdated && m_LEDElement.isUpdated; 
  }

private:
  LEDElement m_LEDElement;
  DisplayElement m_DisplayElement;
  uint32_t m_Color;
};