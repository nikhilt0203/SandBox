#include "Module.h"
#include "AudioCVInput.h"
#pragma once

class Oscillator : public Module
{
public:
  static constexpr auto PARAMETER_NAMES = {"Coarse", "Fine", "FM", "Wave"};
  static constexpr auto INPUT_PORT_NAMES = {Port::FM_IN_NAME, "wv"};

  static constexpr ModuleInfo INFO = {
    Type::OSCILLATOR,
    "Oscillator",
    "Outputs a continuous waveform",
    PARAMETER_NAMES,
    INPUT_PORT_NAMES, {}
  };

public:
  Oscillator(int row, int col, float frequency = DEFAULT_FREQUENCY, int waveform = DEFAULT_WAVEFORM, float octaveControl = DEFAULT_OCTAVE_CONTROL);

  void changeParameter(int parameterNum, int amt) override;

  void setWaveform(int newWaveform);

  void setFrequency(float hz);

  void setOctaveControl(float octaves);

  AudioStream* getInputDevice() const override;

  int getOpenInputPort() const override;

  std::string toString() override;

  static Module* buildFromString(std::string s);

public:
  static constexpr float DEFAULT_FREQUENCY = 440.0f;
  static constexpr float DEFAULT_AMPLITUDE = 0.5f;
  static constexpr float DEFAULT_OCTAVE_CONTROL = 8.0f;
  static constexpr int DEFAULT_WAVEFORM = 0;

  static constexpr float MAX_FREQUENCY = 18000.0f;
  static constexpr float MAX_OCTAVE_CONTROL = 12.0f;
  static constexpr int MAX_FINE_TUNE = 50;
  static constexpr int MAX_SOURCES = 2;
  static constexpr int MAX_DESTINATIONS = 1;

  static constexpr float FREQUENCY_CURVE = 1.08f;
  static constexpr float OCTAVE_CONTROL_CURVE = 0.25f;

  static constexpr int NUM_WAVEFORMS = 6;
  static constexpr int WAVEFORMS[NUM_WAVEFORMS] = 
  {
    WAVEFORM_SINE, WAVEFORM_SQUARE, 
    WAVEFORM_SAWTOOTH, WAVEFORM_TRIANGLE_VARIABLE, 
    WAVEFORM_SAMPLE_HOLD, WAVEFORM_PULSE 
  };
  
  static constexpr uint32_t WAVEFORM_COLORS[NUM_WAVEFORMS] = 
  {
    0x00FF00,
    0xFF0000,
    0xFF00FF,
    0xFFFF00,
    0x32FF00,
    0x32FF32
  };

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