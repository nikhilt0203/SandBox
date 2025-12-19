#pragma once
#include <map>
#include <Arduino.h>
#include "Module.h"
#include "Adafruit_ILI9341.h"

namespace Colors
{
  struct Color
  {
    uint8_t r;
    uint8_t g;
    uint8_t b;

    Color() : r(0), g(0), b(0) {}

    Color(uint8_t red, uint8_t green, uint8_t blue) 
    : r(red), g(green), b(blue) {}
  };

  extern const std::map<ModuleConfig::Type, uint32_t> moduleColors;

  inline Color getRGB(uint32_t hexColor) { return Color((hexColor >> 16) & 0xFF, (hexColor >> 8) & 0xFF, hexColor & 0xFF); }

  inline uint32_t getHex(uint8_t r, uint8_t g, uint8_t b) { return (r << 16) | (g << 8) | b; }

  inline uint32_t getColor(ModuleConfig::Type type) 
  {  
    auto it = moduleColors.find(type);
    if (it == moduleColors.end()) 
    {
      return 0;
    }
    return it->second;
  }

  uint32_t changeBrightness(uint32_t color, float brightness);

  uint32_t blend(uint32_t color1, uint32_t color2, float ratio);

  uint16_t to565(uint32_t color);

  uint32_t to888(uint16_t color565);
}