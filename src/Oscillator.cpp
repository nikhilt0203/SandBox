#include "Oscillator.h"

Oscillator::Oscillator(int row, int col, float frequency = DEFAULT_FREQUENCY, int waveform = DEFAULT_WAVEFORM, float octaveControl = DEFAULT_OCTAVE_CONTROL)
: Module(new AudioSynthWaveformModulated(), INFO.NAME, MAX_SOURCES, MAX_DESTINATIONS, row, col),
  m_Waveform(waveform), m_Frequency(frequency), m_FineTune(0)
{
  m_Osc = static_cast<AudioSynthWaveformModulated*>(m_Device);
  m_Osc->begin(DEFAULT_AMPLITUDE, m_Frequency, WAVEFORMS[m_Waveform]);
  m_WaveformCVInput = new AudioCVInput([this](float v){ this->setWaveform(v); }, 0, NUM_WAVEFORMS);

  m_UIElement = ModuleUIElement(INFO.NAME, m_Color, INFO.PARAMETER_NAMES);
  updateColor(m_Waveform); 
  m_Ports.setInputNames(INFO.INPUT_PORT_NAMES);

  setFrequency(m_Frequency);
  changeFrequencyFine(0);
  setOctaveControl(octaveControl);
  setWaveform(m_Waveform);
}

void Oscillator::changeParameter(int parameterNum, int amt)
{ 
  switch (parameterNum)
  {
    case 0: 
      setFrequency(m_Frequency * powf(FREQUENCY_CURVE, amt)); 
      break;
    case 1: 
      changeFrequencyFine(amt); 
      break;
    case 2: 
      setOctaveControl(m_OctaveControl + (amt * OCTAVE_CONTROL_CURVE)); 
      break;
    case 3: 
      setWaveform((m_Waveform + amt + NUM_WAVEFORMS) % NUM_WAVEFORMS); 
      break;
  }
}

void Oscillator::setWaveform(int newWaveform) 
{
  if (newWaveform < 0 || newWaveform >= NUM_WAVEFORMS) 
  {
    Serial.println("Error: Invalid waveform.");
    return;
  }

  m_Waveform = newWaveform;
  m_Osc->begin(DEFAULT_AMPLITUDE, m_Frequency, WAVEFORMS[m_Waveform]);
  updateColor(m_Waveform);
  m_UIElement.updateParameter(3, m_Waveform, NUM_WAVEFORMS);
}

void Oscillator::setFrequency(float hz) 
{
  if (hz < 0.0 || hz > MAX_FREQUENCY) 
  {
    Serial.println("Error: Invalid frequency.");
    return;
  }

  m_Osc->frequency(hz);
  m_Frequency = hz;
  Serial.println(std::log(m_Frequency) / std::log(FREQUENCY_CURVE));
  m_UIElement.updateParameter(0, (std::log(m_Frequency) / std::log(FREQUENCY_CURVE)), MAX_FREQUENCY);
}

void Oscillator::setOctaveControl(float octaves) 
{
  if (octaves < 0.0 || octaves > MAX_OCTAVE_CONTROL) 
  {
    Serial.println("Error: Invalid octave control.");
    return;
  }
  m_Osc->frequencyModulation(octaves);
  m_OctaveControl = octaves;
  m_UIElement.updateParameter(2, m_OctaveControl, MAX_OCTAVE_CONTROL);
}

AudioStream* Oscillator::getInputDevice() const 
{

  if (m_Ports.isInputFree(0))
  {
    return m_Osc;
  }

  bool isTriangleOrPulse = WAVEFORMS[m_Waveform] == WAVEFORM_TRIANGLE_VARIABLE || WAVEFORMS[m_Waveform] == WAVEFORM_PULSE;

  if (m_Ports.isInputFree(1))
  {
    if (isTriangleOrPulse)
      return m_Osc;
    else
      return m_WaveformCVInput;
  }

  return nullptr;
}

int Oscillator::getOpenInputPort() const 
{
  if (m_Ports.isInputFree(0))
  {
    return 0;
  }

  bool isTriangleOrPulse = WAVEFORMS[m_Waveform] == WAVEFORM_TRIANGLE_VARIABLE || WAVEFORMS[m_Waveform] == WAVEFORM_PULSE;

  if (m_Ports.isInputFree(1))
  {
    return isTriangleOrPulse ? 1 : 0;
  }

  return -1;
}

std::string Oscillator::toString()
{
  m_ID.makeSerializedID(INFO.NAME, m_Row, m_Col, 3);
  m_ID.addParam(m_Waveform);
  m_ID.addParam(m_Frequency);
  m_ID.addParam(m_OctaveControl);
  return m_ID.getSerializedID();
}

Module* Oscillator::buildFromString(std::string s) 
{
  //Format: Oscillator{waveform,frequency,octaveControl}[row,col]<ID>
  size_t firstComma = s.find(",");
  size_t secondComma = s.find(",", firstComma + 1);
  size_t positionComma = s.find(",", s.find("["));

  int waveform =        Serialize::extractIntBetween("{", ",", s);
  float frequency =     Serialize::extractFloatBetween(firstComma, secondComma, s);
  float octaveControl = Serialize::extractFloatBetween(secondComma, s.find("}"), s);
  int row =             Serialize::extractIntBetween(s.find("["), positionComma, s);
  int col =             Serialize::extractIntBetween(positionComma, s.find("]"), s);
  int ID  =             Serialize::extractIntBetween("<", ">", s);

  Oscillator* osc = new Oscillator(row, col, frequency, waveform, octaveControl);
  osc->setID(ID);

  return osc;
}

void Oscillator::changeFrequencyFine(int hz) 
{ 
  const int oldFine = m_FineTune;

  m_FineTune += hz;

  if (m_FineTune > MAX_FINE_TUNE) 
  {
    m_FineTune = MAX_FINE_TUNE;
  }
  else if (m_FineTune < -1 * MAX_FINE_TUNE) 
  {
    m_FineTune = -1 * MAX_FINE_TUNE;
  }

  m_UIElement.updateParameter(1, m_FineTune, MAX_FINE_TUNE);
  setFrequency(m_Frequency - oldFine + m_FineTune);
}

void Oscillator::updateColor(int waveform) 
{ 
  setColor(WAVEFORM_COLORS[waveform]); 
}