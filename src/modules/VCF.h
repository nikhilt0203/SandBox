#include "Module.h"
#include "Port.h"
#pragma once

class VCF : public Module
{
public:
  static constexpr ModuleConfig::Info INFO = {
    ModuleConfig::Type::VCF, 
    "VCF",
    "Voltage controlled filter",
    { "Cutoff", "Reso", "FM"  },
    { Port::INPUT_NAME, Port::FM_IN_NAME },
    { "LP", "HP" }
  };

public:
  VCF();

  void changeParameter(int parameterNum, int amt) override;

  void setCornerFrequency(float hz);

  void setResonance(float q);

  void setOctaveControl(float octaves);

  std::string_view serialize() override;

  static Module* buildFromString(std::string_view s);

public:
  static constexpr size_t MAX_INPUTS = 2;
  static constexpr size_t MAX_OUTPUTS = 2;

  static constexpr float MAX_CORNER_FREQUENCY = 18000.0f;
  static constexpr float DEFAULT_CORNER_FREQUENCY = 2500.0f;
  static constexpr float FREQUENCY_CURVE = 1.08f;

  static constexpr float DEFAULT_OCTAVE_CONTROL = 5.0f;
  static constexpr float MAX_OCTAVE_CONTROL = 7.0f;
  static constexpr float OCTAVE_CONTROL_CURVE = 0.25f;

  static constexpr float DEFAULT_RESONANCE = 0.8f;
  static constexpr float MIN_RESONANCE = 0.7f;
  static constexpr float MAX_RESONANCE = 5.0f;
  static constexpr float RESONANCE_CURVE = 0.1f;

private:
  AudioFilterStateVariable* m_Filter;
  float m_CornerFrequency;
  float m_Resonance;
  float m_OctaveControl;
};