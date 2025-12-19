#include "Module.h"
#include "Port.h"
#pragma once

class VCF : public Module
{
public:
  VCF(int row, int col);


  void changeParameter(int parameterNum, int amt) override;
  void setCornerFrequency(float hz);
  void setResonance(float q);
  void setOctaveControl(float octaves);
  float getCornerFrequency() const;
  float getResonance() const;
  float getOctaveControl() const;
  int getOpenOutputPort() const override;
  std::string toString() override;
  static Module* buildFromString(std::string s);

public:
  static constexpr int MAX_SOURCES = 2;
  static constexpr int MAX_DESTINATIONS = 2;

  static constexpr float MAX_CORNER_FREQUENCY = 18000.0f;
  static constexpr float DEFAULT_CORNER_FREQUENCY = 2500.0f;
  static constexpr float MAX_OCTAVE_CONTROL = 7.0f;
  static constexpr float DEFAULT_OCTAVE_CONTROL = 5.0f;
  static constexpr float MIN_RESONANCE = 0.7f;
  static constexpr float MAX_RESONANCE = 5.0f;
  static constexpr float DEFAULT_RESONANCE = 0.8f;

  static constexpr float FREQUENCY_CURVE = 1.08f;
  static constexpr float RESONANCE_CURVE = 0.1f;
  static constexpr float OCTAVE_CONTROL_CURVE = 0.25f;

  static constexpr ModuleInfo INFO =
  {
    Type::VCF,
    "VCF",
    "Voltage controlled filter",
    {"Cutoff", "Reso", "FM"},
    {Port::INPUT_NAME, Port::FM_IN_NAME},
    {"LP", "HP"}
  };

private:
  AudioFilterStateVariable* m_Filter;
  float m_CornerFrequency;
  float m_Resonance;
  float m_OctaveControl;
};