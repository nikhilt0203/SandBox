#pragma once
#include <initializer_list>

namespace ModuleConfig
{
  enum class Type
  {
    NONE, 
    OSCILLATOR, 
    LFO, 
    MIXER, 
    COMBINE, 
    MULT, 
    VCF, 
    VCA, 
    BITCRUSHER, 
    SEQUENCER, 
    SEQUENCERSTEP, 
    ENVELOPE, 
    AMPLIFIER, 
    USBOUT, 
    DCGENERATOR, 
    DELAY,
    OSCILLOSCOPE,
    NOISE,

    /***/NUM_TYPES/***/ 
  };

  struct Info
  {
    const Type TYPE;
    const char* const NAME;
    const char* const DESCRIPTION;
    const std::initializer_list<const char*> PARAMETER_NAMES;
    const std::initializer_list<const char*> INPUT_PORT_NAMES;
    const std::initializer_list<const char*> OUTPUT_PORT_NAMES;
    uint32_t COLOR{};
  };
}

#define MODULE_PARAMS(...)   { __VA_ARGS__ }
#define MODULE_INPUTS(...)   { __VA_ARGS__ }
#define MODULE_OUTPUTS(...)  { __VA_ARGS__ }

#define MODULE_INFO(TYPE_ENUM, NAME_STR, DESC_STR, PARAMS, INPUTS, OUTPUTS) \
  static constexpr ModuleConfig::Info INFO = { \
    ModuleConfig::Type::TYPE_ENUM, NAME_STR, DESC_STR, PARAMS, INPUTS, OUTPUTS \
  };
