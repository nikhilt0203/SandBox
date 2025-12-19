#include "Colors.h"

const std::map<ModuleConfig::Type, uint32_t> Colors::moduleColors =
{
  { ModuleConfig::Type::USBOUT,        0x0000FF }, 
  { ModuleConfig::Type::OSCILLATOR,    0x00FF00 }, 
  { ModuleConfig::Type::LFO,           0x00FFFF }, 
  { ModuleConfig::Type::VCF,           0xB427F5 }, 
  { ModuleConfig::Type::MIXER,         0x446C82 }, 
  { ModuleConfig::Type::COMBINE,       0x1C6B26 }, 
  { ModuleConfig::Type::MULT,          0x70043C }, 
  { ModuleConfig::Type::VCA,           0x591A35 }, 
  { ModuleConfig::Type::ENVELOPE,      0xFF7300 }, 
  { ModuleConfig::Type::SEQUENCER,     0xFFFF00 }, 
  { ModuleConfig::Type::SEQUENCERSTEP, 0xFFFF00 }, 
  { ModuleConfig::Type::BITCRUSHER,    0x6D0470 },
  { ModuleConfig::Type::AMPLIFIER,     0x591A35 },
  { ModuleConfig::Type::DCGENERATOR,   0x591FF8 },
  { ModuleConfig::Type::DELAY,         0xEB8F00 },
  { ModuleConfig::Type::OSCILLOSCOPE,  0x07472D },
  { ModuleConfig::Type::NOISE,         0xDB4B6D },
  { ModuleConfig::Type::NONE,          0x110011 },

  // { ModuleConfig::Type::USBOUT,        0xFAC60E }, 
  // { ModuleConfig::Type::OSCILLATOR,    0x382C18 }, 
  // { ModuleConfig::Type::LFO,           0x07472D }, 
  // { ModuleConfig::Type::VCF,           0x78F61D }, 
  // { ModuleConfig::Type::MIXER,         0x5B423E }, 
  // { ModuleConfig::Type::COMBINE,       0x39255B }, 
  // { ModuleConfig::Type::MULT,          0x656037 }, 
  // { ModuleConfig::Type::VCA,           0x3F7370 }, 
  // { ModuleConfig::Type::ENVELOPE,      0x7E3685 }, 
  // { ModuleConfig::Type::SEQUENCER,     0x465A8E }, 
  // { ModuleConfig::Type::SEQUENCERSTEP, 0x948E17 }, 
  // { ModuleConfig::Type::BITCRUSHER,    0x6F9E56 },
  // { ModuleConfig::Type::AMPLIFIER,     0x0083C2 },
  // { ModuleConfig::Type::DCGENERATOR,   0xDE6237 },
  // { ModuleConfig::Type::DELAY,         0x2B43AD },
  // { ModuleConfig::Type::OSCILLOSCOPE,  0xDB4B6D },
  // { ModuleConfig::Type::NOISE,         0x3DCC52 },
  // { ModuleConfig::Type::NONE,          0xB69227 }
};

uint32_t Colors::changeBrightness(uint32_t color, float brightness)
{
  Color c = getRGB(color);
  uint8_t r = c.r;
  uint8_t g = c.g;
  uint8_t b = c.b;

  r = static_cast<uint8_t>(r * brightness);
  g = static_cast<uint8_t>(g * brightness);
  b = static_cast<uint8_t>(b * brightness);

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

uint32_t Colors::to888(uint16_t color565)
{
  uint8_t r = (color565 >> 11) & 0x1F;
  uint8_t g = (color565 >> 5) & 0x3F;
  uint8_t b = color565 & 0x1F;

  r = (r * 527 + 23) >> 6;
  g = (g * 259 + 33) >> 6;
  b = (b * 527 + 23) >> 6;

  return getHex(r, g, b);
}