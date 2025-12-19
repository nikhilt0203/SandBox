#pragma once
#include "Grid.h"
#include "Module.h"
#include "Colors.h"
#include "Port.h"
#include "SequencerStep.h"
#include "AudioTriggerInput.h"
#include <functional>

class Sequencer : public Module
{
public:
  static constexpr int MAX_SOURCES = 1;
  static constexpr int MAX_DESTINATIONS = 1;
  static constexpr int DEFAULT_STEPS = 8;
  static constexpr int MAX_STEPS = 16;
  static constexpr float TRIGGER_THRESHOLD = 0.25;

  static constexpr Type TYPE = Type::SEQUENCER;
  static constexpr const char* NAME = "Sequencer";
  static constexpr auto PARAMETER_NAMES = {"Length"};
  static constexpr auto INPUT_PORT_NAMES = {Port::CLOCK_IN_NAME};
  
public:
  Sequencer(int row, int col, int numSteps = DEFAULT_STEPS) 
  : Module(new AudioSynthWaveformDc(), NAME, MAX_SOURCES, MAX_DESTINATIONS, row, col), 
  m_NumSteps(numSteps), m_CurrentStep(-1), m_LastState(false)
  {
    m_Type = TYPE;
    m_Dc = static_cast<AudioSynthWaveformDc*>(m_Device);
    m_Trig = new AudioTriggerInput();
    m_Trig->setRisingEdgeCallback([this]() { this->nextStep(); });
    m_UIElement = ModuleUIElement(NAME, m_Color, PARAMETER_NAMES);
    setColor(m_Type);
    m_Ports.setInputNames(INPUT_PORT_NAMES);
    createSteps();
  }

  ~Sequencer() 
  {
    for (auto& step : m_Steps)
      Module::deleteModule(step.release());
    m_Steps.clear();
  }
  
  void changeParameter(int parameterNum, int amt) override
  { 
    switch (parameterNum)
    {
      case 0: setSequenceLength(m_NumSteps + amt); break;
    }
  }

  void nextStep()
  {
    int previousStep = m_CurrentStep;
    m_CurrentStep = (m_CurrentStep + 1) % m_NumSteps;

    m_Dc->amplitude(m_Steps[m_CurrentStep]->getAmplitude());

    if (previousStep >= 0)
      m_Steps[previousStep]->off();

    m_Steps[m_CurrentStep]->on();
  }

  void setStep(int stepIndex, float amplitude)
  {
    if (amplitude < 0.0) amplitude = 0.0;
    if (amplitude > 1.0) amplitude = 1.0;
    m_Steps[stepIndex]->setAmplitude(amplitude);
  }

  void setSequenceLength(int numSteps)
  {

  }

  void stop() 
  {
    for (size_t i = 0; i < m_NumSteps; i++)
      m_Steps[i]->off();

    m_CurrentStep = -1;
  }

  void begin() { nextStep(); }

  AudioStream* getInputDevice() const override { return m_Trig; }

  AudioStream* getOutputDevice() const override { return m_Dc; }

  std::string toString() override
  {
    m_ID.makeSerializedID(NAME, m_Row, m_Col, m_NumSteps + 1);
    m_ID.addParam(m_NumSteps);

    for (size_t i = 0; i < m_NumSteps; i++)
      m_ID.addParam(m_Steps[i]->getAmplitude());
  
    return m_ID.getSerializedID();
  }

  static Module* buildFromString(std::string s)
  { 
    //Format: Sequencer{numSteps, amplitude1, amplitude2, amplitude 3, ... }[row,col]<ID>
    int positionComma = s.find(",", s.find("["));

    int numSteps = Serialize::extractIntBetween("{", ",", s);
    int row =      Serialize::extractIntBetween(s.find("["), positionComma, s);
    int col =      Serialize::extractIntBetween(positionComma, s.find("]"), s);
    int ID  =      Serialize::extractIntBetween("<", ">", s);

    Sequencer* seq = new Sequencer(row, col, numSteps);
    seq->setID(ID);

    int startIndex = s.find(",");
    for (int i = 0; i < numSteps; i++)
    {
      size_t endIndex;
      if (i < numSteps - 1)
        endIndex = s.find(",", startIndex + 1);
      else
        endIndex = s.find("}", startIndex + 1);
        
      if (endIndex == std::string::npos) 
        break;

      float amplitude = Serialize::extractFloatBetween(startIndex, endIndex, s);
      startIndex = endIndex;
      seq->setStep(i, amplitude);
    }
    return seq;
  }
private:
  void createSteps()
  {
    m_Steps.clear();
    for (size_t i = 0; i < m_NumSteps; i++)
    {
      int newRow = Module::getRow() + (Module::getCol() + i + 1) / 8;
      int newCol = (Module::getCol() + i + 1) % 8;
      
      //if there isn't room to add the next step, truncate the sequence
      if (Grid::isModuleAt(newRow, newCol) || newRow > 6)
      {
        m_NumSteps = i;
        break;
      }
      m_Steps.push_back(std::make_unique<SequencerStep>(newRow, newCol));
    }
  }
private:
  AudioSynthWaveformDc* m_Dc;
  AudioTriggerInput* m_Trig;
  std::vector<std::unique_ptr<SequencerStep>> m_Steps;
  size_t m_NumSteps;
  int m_CurrentStep;
  bool m_LastState;
};