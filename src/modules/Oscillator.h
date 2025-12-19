#include "Module.h"
#include "audio/AudioCVInput.h"
#pragma once

class Oscillator : public Module
{
public:
  static constexpr ModuleConfig::Info INFO = { 
    ModuleConfig::Type::OSCILLATOR, 
    "Oscillator", 
    "Outputs a continuous waveform", 
    { "Coarse", "Fine", "FM", "Wave" }, 
    { Port::FM_IN_NAME, "wv" }, 
    { } 
  };


  static constexpr float DEFAULT_FREQUENCY = 440.0f;
  static constexpr float DEFAULT_AMPLITUDE = 0.5f;
  static constexpr float DEFAULT_OCTAVE_CONTROL = 8.0f;
  static constexpr int DEFAULT_WAVEFORM = 0;

  static constexpr float MAX_FREQUENCY = 18000.0f;
  static constexpr float MAX_OCTAVE_CONTROL = 12.0f;
  static constexpr int MAX_FINE_TUNE = 50;

  static constexpr float FREQUENCY_CURVE = 1.08f;
  static constexpr float OCTAVE_CONTROL_CURVE = 0.25f;

  static constexpr int NUM_WAVEFORMS = 6;

  static constexpr int WAVEFORMS[NUM_WAVEFORMS] = {
    WAVEFORM_SINE, WAVEFORM_SQUARE, 
    WAVEFORM_SAWTOOTH, WAVEFORM_TRIANGLE_VARIABLE, 
    WAVEFORM_SAMPLE_HOLD, WAVEFORM_PULSE 
  };
  
  static constexpr uint32_t WAVEFORM_COLORS[NUM_WAVEFORMS] = {
    0x00FF00, 0xFF0000, 0xFF00FF, 0xFFFF00, 0x0083C2, 0x32FF32
  };

public:
  Oscillator(float frequency = DEFAULT_FREQUENCY, 
    int waveform = DEFAULT_WAVEFORM, 
    float octaveControl = DEFAULT_OCTAVE_CONTROL);

  ~Oscillator();

  void changeParameter(int parameterNum, int amt) override;

  void setWaveform(int newWaveform);

  void setFrequency(float hz);

  void setOctaveControl(float octaves);

  AudioStream* getInputDevice() const override;

  Port getOpenInputPort() override;

  std::string_view serialize() override;

  static Module* buildFromString(std::string_view s);

protected:
  AudioSynthWaveformModulated* m_Osc;
  AudioCVInput* m_WaveformCVInput;
  int m_Waveform;
  float m_Frequency;
  float m_OctaveControl;
  int m_FineTune;

private:
  void changeFrequencyFine(int hz);

  void updateColor(int waveform);
};