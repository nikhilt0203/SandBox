#pragma once
#include "Module.h"
#include "Port.h"
#include "SequencerStep.h"
#include "audio/AudioTriggerInput.h"
#include "audio/AudioTriggerOutput.h"

class Sequencer : public Module
{
public:
  MODULE_INFO(
    SEQUENCER,
    "Sequencer",
    "Outputs a sequence of DC voltages   Triggered via clock input",
    MODULE_PARAMS("Length", "Thresh"),
    MODULE_INPUTS(Port::CLOCK_IN_NAME),
    MODULE_OUTPUTS(Port::CV_OUT_NAME, "trg")
  );

  static constexpr size_t MAX_INPUTS = 1;
  static constexpr size_t MAX_OUTPUTS = 2;

  static constexpr size_t DEFAULT_STEPS = 8;
  static constexpr size_t MAX_STEPS = 32;

  static constexpr float MAX_TRIGGER_THRESHOLD = 1.0f;

public:
  Sequencer(size_t numSteps = DEFAULT_STEPS);

  ~Sequencer();
  
  void changeParameter(int parameterNum, int amt) override;

  void nextStep();
  void setStep(int stepIndex, float amplitude);
  void changeSequenceLength(int amt);
  void setTriggerThreshold(float value);

  AudioStream* getInputDevice() const override { return m_Trig; }
  AudioStream* getOutputDevice() const override;

  std::string_view serialize() override;

public:
  static Module* buildFromString(std::string_view s);

  static void createSteps(Sequencer& sequencer, size_t numSteps = DEFAULT_STEPS);
  
  static void deleteSteps(Sequencer& sequencer);

private:
  AudioSynthWaveformDc* m_Dc;
  AudioTriggerInput* m_Trig;
  AudioTriggerOutput* m_TrigOut;
  std::vector<std::unique_ptr<SequencerStep>> m_Steps;
  std::array<float, MAX_STEPS> m_SequenceMemory{};
  size_t m_NumSteps;
  int m_CurrentStep;
  bool m_LastState;
  float m_Threshold;
};