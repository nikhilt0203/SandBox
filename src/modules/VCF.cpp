#include "VCF.h"
#include "Port.h"
#include "audio/AudioTriggerOutput.h"

#define HP_OUT_PORT 2

VCF::VCF()
: Module(new AudioFilterStateVariable(), INFO.NAME, MAX_INPUTS, MAX_OUTPUTS)
{
  m_Type = INFO.TYPE;
  m_Filter = static_cast<AudioFilterStateVariable*>(m_Device);

  m_UIElement = ModuleUIElement(INFO);
  m_Ports.setInputNames(INFO.INPUT_PORT_NAMES);
  m_Ports.setOutputNames(INFO.OUTPUT_PORT_NAMES);
  m_Ports.setOutputDevicePort(HP_OUT_PORT, 1);

  setCornerFrequency(DEFAULT_CORNER_FREQUENCY);
  setResonance(DEFAULT_RESONANCE);
  setOctaveControl(DEFAULT_OCTAVE_CONTROL);
}

void VCF::setCornerFrequency(float hz) 
{
  m_CornerFrequency = std::clamp(hz, 0.0f, MAX_CORNER_FREQUENCY);
  m_Filter->frequency(m_CornerFrequency);
  m_UIElement.updateParameter(0, m_CornerFrequency, MAX_CORNER_FREQUENCY);
}

void VCF::setResonance(float q) 
{
  m_Resonance = std::clamp(q, MIN_RESONANCE, MAX_RESONANCE);
  m_Filter->resonance(m_Resonance);
  m_UIElement.updateParameter(1, m_Resonance - MIN_RESONANCE, MAX_RESONANCE - MIN_RESONANCE);
}

void VCF::setOctaveControl(float octaves) 
{ 
  m_OctaveControl = std::clamp(octaves, 0.0f, MAX_OCTAVE_CONTROL);
  m_Filter->octaveControl(m_OctaveControl);
  m_UIElement.updateParameter(2, m_OctaveControl, MAX_OCTAVE_CONTROL);
}

void VCF::changeParameter(int parameterNum, int amt)
{
  switch (parameterNum)
  {
    case 0: setCornerFrequency(m_CornerFrequency * powf(FREQUENCY_CURVE, amt)); break;
    case 1: setResonance(m_Resonance + (amt * RESONANCE_CURVE)); break;
    case 2: setOctaveControl(m_OctaveControl + (amt * OCTAVE_CONTROL_CURVE)); break;
  }
}

std::string_view VCF::serialize()
{
  m_ID.makeSerializationID(m_UIElement);
  m_ID.addParam(m_CornerFrequency);
  m_ID.addParam(m_Resonance);
  m_ID.addParam(m_OctaveControl);
  return m_ID.getSerializationID();
}

Module* VCF::buildFromString(std::string_view s) 
{
  //Format: VCF{m_CornerFrequency,m_Resonance,m_OctaveControl}[row,col]<ID>
  size_t secondComma = s.find(",", s.find(",") + 1);
  size_t positionComma = s.find(",", s.find("["));

  float frequency =     Serialize::extractFloatBetween("{", ",", s);
  float resonance =     Serialize::extractFloatBetween(s.find(","), secondComma, s);
  float octaveControl = Serialize::extractFloatBetween(secondComma, s.find("}"), s);
  int row =             Serialize::extractIntBetween(s.find("["), positionComma, s);
  int col =             Serialize::extractIntBetween(positionComma, s.find("]"), s);
  int ID  =             Serialize::extractIntBetween("<", ">", s);

  VCF* vcf = new VCF();
  vcf->setID(ID);
  vcf->setCornerFrequency(frequency);
  vcf->setResonance(resonance);
  vcf->setOctaveControl(octaveControl);
  vcf->getLEDElement().setPosition(row, col);

  return vcf;
}