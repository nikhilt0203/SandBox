#include "Colors.h"

const std::map<Module::Type, uint32_t> Colors::moduleColors =
{
  { Module::Type::USBOUT,       0x0000FF }, 
  { Module::Type::OSCILLATOR,   0x00FF00 }, 
  { Module::Type::LFO,          0x00FFFF }, 
  { Module::Type::VCF,          0xB427F5 }, 
  { Module::Type::MIXER,        0x446C82 }, 
  { Module::Type::COMBINE,      0x1C6B26 }, 
  { Module::Type::MULT,         0x70043C }, 
  { Module::Type::VCA,          0x591A35 }, 
  { Module::Type::ENVELOPE,     0xFF7300 }, 
  { Module::Type::SEQUENCER,    0xFFFF00 }, 
  { Module::Type::BITCRUSHER,   0x6D0470 },
  { Module::Type::AMPLIFIER,    0x591A35 },
  { Module::Type::DCGENERATOR,  0x591FF8 },
  { Module::Type::DELAY,        0xEB8F00 },
  { Module::Type::OSCILLOSCOPE, ILI9341_PURPLE }
};

uint32_t Colors::getColor(Module::Type type)
{
  auto it = moduleColors.find(type);
  if (it != moduleColors.end()) 
    return it->second;
  return 0xFFFFFF;
}

uint32_t Colors::changeBrightness(uint32_t color, float brightness)
{
  Color c = getRGB(color);
  uint8_t r = c.r;
  uint8_t g = c.g;
  uint8_t b = c.b;

  r = ((r + 200) / 2) * brightness;
  g = ((g + 200) / 2) * brightness;
  b = ((b + 200) / 2) * brightness;

  return getHex(r, g, b);
}

uint32_t Colors::blend(uint32_t color1, uint32_t color2, float ratio)
{
  Color c1 = getRGB(color1);
  Color c2 = getRGB(color2);
  uint8_t r = c1.r * (1 - ratio) + c2.r * ratio;
  uint8_t g = c1.g * (1 - ratio) + c2.g * ratio;
  uint8_t b = c1.b * (1 - ratio) + c2.b * ratio; 

  return getHex(r, g, b);
}

uint16_t Colors::to565(uint32_t color)
{
  Color c = getRGB(color);
  return (c.r & 0xF8) << 8 | (c.g & 0xFC) << 3 | (c.b & 0xF8) >> 3;
}