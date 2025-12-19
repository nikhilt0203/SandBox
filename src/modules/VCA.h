#include "Module.h"
#include "Port.h"
#include "audio/AudioEffectVCA.h"
#pragma once

class VCA : public Module 
{ 
public:
  MODULE_INFO(
    VCA,
    "VCA",
    "Voltage controlled amplifier",
    MODULE_PARAMS("CV", "Gain"),
    MODULE_INPUTS(Port::INPUT_NAME, Port::CV_IN_NAME),
    MODULE_OUTPUTS(Port::OUTPUT_NAME)
  );
  
  static constexpr size_t MAX_INPUTS = 2;
  static constexpr size_t MAX_OUTPUTS = 1;

  static constexpr float DEFAULT_MIX = 0.5f;
  static constexpr float MAX_MIX = 1.0f;

  static constexpr float DEFAULT_GAIN = 1.0f;
  static constexpr float MAX_GAIN_ = 5.0f;

  static constexpr float ADJUSTMENT_CURVE = 1.10f;

public:
  VCA();

  void changeParameter(int parameterNum, int amt) override;
  
  void setMix(float mix);
  void setGain(float gain);

  std::string_view serialize() override;

public:
  static Module* buildFromString(std::string_view s);

private:
  AudioEffectVCA* m_VCA;
  float m_Mix;
  float m_Gain;
};