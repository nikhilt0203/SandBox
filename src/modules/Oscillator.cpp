#include "Oscillator.h"

#define MAX_INPUTS 2
#define MAX_OUTPUTS 1

Oscillator::Oscillator(float frequency, int waveform, float octaveControl)
: Module(new AudioSynthWaveformModulated(), INFO.NAME, MAX_INPUTS, MAX_OUTPUTS),
  m_Waveform(waveform), m_Frequency(frequency), m_FineTune(0)
{
  m_Type = INFO.TYPE;
  m_Osc = static_cast<AudioSynthWaveformModulated*>(m_Device);
  m_Osc->begin(DEFAULT_AMPLITUDE, m_Frequency, WAVEFORMS[m_Waveform]);
  m_WaveformCVInput = new AudioCVInput([this](float v){ this->setWaveform(v); }, 0, NUM_WAVEFORMS);

  m_UIElement = ModuleUIElement(INFO);

  m_Ports.setInputNames(INFO.INPUT_PORT_NAMES);
  m_Ports.setInputDevicePort(0, 0);
  m_Ports.setInputDevicePort(1, 0);

  setFrequency(m_Frequency);
  changeFrequencyFine(0);
  setOctaveControl(octaveControl);
  setWaveform(m_Waveform);
}

Oscillator::~Oscillator()
{
  delete m_WaveformCVInput;
}

void Oscillator::changeParameter(int parameterNum, int amt)
{ 
  switch (parameterNum)
  {
    case 0: 
      setFrequency(m_Frequency * powf(FREQUENCY_CURVE, amt)); break;
    case 1: 
      changeFrequencyFine(amt); break;
    case 2: 
      setOctaveControl(m_OctaveControl + (amt * OCTAVE_CONTROL_CURVE)); break;
    case 3: 
      setWaveform((m_Waveform + amt + NUM_WAVEFORMS) % NUM_WAVEFORMS); break;
  }
}

void Oscillator::setWaveform(int newWaveform) 
{
  m_Waveform = std::clamp(newWaveform, 0, NUM_WAVEFORMS - 1);

  AudioNoInterrupts();
  m_Osc->begin(DEFAULT_AMPLITUDE, m_Frequency, WAVEFORMS[m_Waveform]);
  AudioInterrupts();

  updateColor(m_Waveform);
  m_UIElement.updateParameter(3, m_Waveform, NUM_WAVEFORMS);
}

void Oscillator::setFrequency(float hz) 
{
  constexpr static float FREQUENCY_CURVE_MAX = 127.0f;
  m_Frequency = std::clamp(hz, 0.0f, MAX_FREQUENCY);
  m_Osc->frequency(m_Frequency);
  m_UIElement.updateParameter(0, (std::log(m_Frequency) / std::log(FREQUENCY_CURVE)), FREQUENCY_CURVE_MAX);
}

void Oscillator::setOctaveControl(float octaves) 
{
  m_OctaveControl = std::clamp(octaves, 0.0f, MAX_OCTAVE_CONTROL);
  m_Osc->frequencyModulation(m_OctaveControl);
  m_UIElement.updateParameter(2, m_OctaveControl, MAX_OCTAVE_CONTROL);
}

AudioStream* Oscillator::getInputDevice() const 
{
  if (m_Ports.isInputFree(0)) 
  { 
    return m_Osc; 
  }

  int waveform = WAVEFORMS[m_Waveform];

  if (m_Ports.isInputFree(1))
  {
    if (waveform == WAVEFORM_TRIANGLE_VARIABLE 
      || waveform == WAVEFORM_PULSE) 
    { 
      return m_Osc; 
    }
    else 
    { 
      return m_WaveformCVInput; 
    }
  }

  return nullptr;
}

Port Oscillator::getOpenInputPort() 
{
  if (m_Ports.isInputFree(0)) 
  {
    m_Ports.setInputDevicePort(0, 0);
    return m_Ports.getOpenInputPort();
  }

  int waveform = WAVEFORMS[m_Waveform];
  
  if (m_Ports.isInputFree(1))
  {
    if (waveform == WAVEFORM_TRIANGLE_VARIABLE 
      || waveform == WAVEFORM_PULSE)
    {
      m_Ports.setInputDevicePort(1, 1);
    } 
    else 
    {
      m_Ports.setInputDevicePort(0, 1);
    }
  }

  return m_Ports.getOpenInputPort();
}

std::string_view Oscillator::serialize()
{
  m_ID.makeSerializationID(m_UIElement);
  m_ID.addParam(m_Waveform);
  m_ID.addParam(m_Frequency);
  m_ID.addParam(m_OctaveControl);
  return m_ID.getSerializationID();
}

Module* Oscillator::buildFromString(std::string_view s) 
{
  //Format: Oscillator{waveform,frequency,octaveControl}[row,col]<ID>_type_
  size_t firstComma = s.find(",");
  size_t secondComma = s.find(",", firstComma + 1);
  size_t positionComma = s.find(",", s.find("["));

  int waveform =        Serialize::extractIntBetween("{", ",", s);
  float frequency =     Serialize::extractFloatBetween(firstComma, secondComma, s);
  float octaveControl = Serialize::extractFloatBetween(secondComma, s.find("}"), s);
  int row =             Serialize::extractIntBetween(s.find("["), positionComma, s);
  int col =             Serialize::extractIntBetween(positionComma, s.find("]"), s);
  int ID  =             Serialize::extractIntBetween("<", ">", s);

  Oscillator* osc = new Oscillator(frequency, waveform, octaveControl);
  osc->setID(ID);
  osc->getLEDElement().setPosition(row, col);
  return osc;
}

void Oscillator::changeFrequencyFine(int hz) 
{ 
  const int oldFineTune = m_FineTune;

  m_FineTune += hz;

  if (m_FineTune > MAX_FINE_TUNE) 
  {
    m_FineTune = MAX_FINE_TUNE;
  }
  else if (m_FineTune < -1 * MAX_FINE_TUNE) 
  {
    m_FineTune = -1 * MAX_FINE_TUNE;
  }

  m_UIElement.updateParameter(1, m_FineTune + MAX_FINE_TUNE, MAX_FINE_TUNE * 2);
  setFrequency(m_Frequency - oldFineTune + m_FineTune);
}

void Oscillator::updateColor(int waveform) 
{ 
  m_UIElement.updateColor(WAVEFORM_COLORS[waveform]); 
}