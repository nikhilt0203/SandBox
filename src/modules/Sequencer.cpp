#include "Sequencer.h"
#include "Colors.h"
#include "Port.h"
#include "Grid.h"

Sequencer::Sequencer(size_t numSteps) 
: Module(new AudioSynthWaveformDc(), INFO.NAME, MAX_INPUTS, MAX_OUTPUTS), 
  m_NumSteps(numSteps), 
  m_CurrentStep(-1), 
  m_LastState(false)
{
  m_Type = INFO.TYPE;
  m_Dc = static_cast<AudioSynthWaveformDc*>(m_Device);

  m_Trig = new AudioTriggerInput();
  m_TrigOut = new AudioTriggerOutput();
  m_Trig->setRisingEdgeCallback([this](){ this->nextStep(); });

  m_UIElement = ModuleUIElement(INFO);

  setTriggerThreshold(AudioTriggerInput::DEFAULT_TRIGGER_THRESHOLD);

  m_Ports.setInputNames(INFO.INPUT_PORT_NAMES);
  m_Ports.setOutputNames(INFO.OUTPUT_PORT_NAMES);
  m_Ports.setOutputDevicePort(0, 1);
}

Sequencer::~Sequencer() 
{
  delete m_Trig;
  delete m_TrigOut;
}

void Sequencer::changeParameter(int parameterNum, int amt)
{
  switch (parameterNum)
  {
    case 0: changeSequenceLength(amt); break;
    case 1: setTriggerThreshold(m_Threshold + (amt / 75.0f)); break;
  }
}

void Sequencer::nextStep()
{
  int previousStep = m_CurrentStep;
  m_CurrentStep = (m_CurrentStep + 1) % m_NumSteps;

  float currentStepAmplitude = m_Steps[m_CurrentStep]->getAmplitude();
  m_Dc->amplitude(currentStepAmplitude);
  m_SequenceMemory[m_CurrentStep] = currentStepAmplitude;

  if (previousStep >= 0) 
  { 
    m_Steps[previousStep]->off(); 
    m_TrigOut->off();
  }

  m_Steps[m_CurrentStep]->on();
    
  (currentStepAmplitude > 0.0f) ? m_TrigOut->on() : m_TrigOut->off();
}

void Sequencer::setStep(int stepIndex, float amplitude)
{
  amplitude = std::clamp(amplitude, 0.0f, 1.0f);
  stepIndex = std::clamp(stepIndex, 0, static_cast<int>(m_NumSteps) - 1);
  m_Steps[stepIndex]->setAmplitude(amplitude);
}

void Sequencer::changeSequenceLength(int amt) 
{
  if (amt == 0)
  {
    return;
  }

  auto& lastStep = m_Steps.back();
  
  // Add step to end
  if (amt > 0 && m_NumSteps < MAX_STEPS)
  {
    auto& lastStepElement = (m_NumSteps > 0) ? 
      lastStep->getLEDElement() : m_UIElement.getLEDElement();

    int newRow = lastStepElement.row + (lastStepElement.col + 1) / Grid::ROWS;
    int newCol = (lastStepElement.col + 1) % Grid::COLS;

    if (Grid::isValid(newRow, newCol) && !Grid::isModuleAt(newRow, newCol))
    {
      auto newStep = std::make_unique<SequencerStep>();
      float savedAmplitude = m_SequenceMemory[m_NumSteps];
      newStep->setAmplitude(savedAmplitude);
      newStep->getLEDElement().setPosition(newRow, newCol);
      m_Steps.push_back(std::move(newStep));
    }
  }

  // Remove step from end
  if (amt < 0 && m_NumSteps > 1)
  {
    lastStep->off();
    lastStep->getLEDElement().isUpdated = false;
    Module::deleteModule(lastStep.release());
    m_Steps.pop_back();
  }

  m_NumSteps = m_Steps.size();

  if (m_CurrentStep >= static_cast<int>(m_NumSteps))
  {
    m_CurrentStep = 0;
  }

  m_UIElement.updateParameter(0, static_cast<int>(m_NumSteps), MAX_STEPS);
}

void Sequencer::setTriggerThreshold(float value)
{
  m_Threshold = std::clamp(value, 0.01f, 1.0f); 
  m_Trig->threshold(m_Threshold);
  m_UIElement.updateParameter(1, m_Threshold, MAX_TRIGGER_THRESHOLD);
}

AudioStream* Sequencer::getOutputDevice() const 
{ 
  if (m_Ports.isOutputFree(0)) 
  {
    return m_Dc;
  }
  if (m_Ports.isOutputFree(1)) 
  {
    return m_TrigOut;
  }
  return nullptr;
}

std::string_view Sequencer::serialize()
{
  m_ID.makeSerializationID(m_UIElement);
  m_ID.addParam(m_NumSteps);

  for (size_t i{}; i < m_NumSteps; i++)
  {
    m_ID.addParam(m_Steps[i]->getAmplitude());
  }

  return m_ID.getSerializationID();
}

Module* Sequencer::buildFromString(std::string_view s)
{ 
  //Format: Sequencer{numSteps, amplitude1, amplitude2, amplitude 3, ... }[row,col]<ID>
  int positionComma = s.find(",", s.find("["));

  int numSteps = Serialize::extractIntBetween("{", ",", s);
  int row =      Serialize::extractIntBetween(s.find("["), positionComma, s);
  int col =      Serialize::extractIntBetween(positionComma, s.find("]"), s);
  int ID  =      Serialize::extractIntBetween("<", ">", s);

  Sequencer* seq = new Sequencer(numSteps);
  seq->setID(ID);
  seq->getLEDElement().setPosition(row, col);
  seq->createSteps(*seq, numSteps);

  int startIndex = s.find(",");
  for (int i{}; i < numSteps; i++)
  {
    size_t endIndex;
    if (i < numSteps - 1) 
    {
      endIndex = s.find(",", startIndex + 1);
    }
    else 
    {
      endIndex = s.find("}", startIndex + 1);
    }
      
    if (endIndex == std::string::npos) 
    {
      break;
    }

    float amplitude = Serialize::extractFloatBetween(startIndex, endIndex, s);
    startIndex = endIndex;
    seq->setStep(i, amplitude);
  }
  return seq;
}

void Sequencer::createSteps(Sequencer& sequencer, size_t numSteps)
{
  sequencer.m_Steps.clear();
  auto& ledElement = sequencer.m_UIElement.getLEDElement();

  for (size_t i{}; i < numSteps; i++)
  {
    int newRow = ledElement.row + (ledElement.col + i + 1) / 8;
    int newCol = (ledElement.col + i + 1) % 8;

    if (Grid::isModuleAt(newRow, newCol) || newRow > 6)
    {
      sequencer.m_NumSteps = i;
      break;
    }
    
    auto newStep = std::make_unique<SequencerStep>();
    newStep->getLEDElement().setPosition(newRow, newCol);
    sequencer.m_Steps.push_back(std::move(newStep));
  }
}

void Sequencer::deleteSteps(Sequencer& sequencer)
{
  auto& steps = sequencer.m_Steps;
  for (auto& step : steps)
  {
    Grid::updateSquare(Grid::Space::EMPTY, step->getLEDElement().row, step->getLEDElement().col);
    Module::deleteModule(step.release());
  }
  steps.clear();
}