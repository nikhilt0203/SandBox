#include "VCF.h"
#include "Port.h"

VCF::VCF(int row, int col)
: Module(new AudioFilterStateVariable(), INFO.NAME, MAX_SOURCES, MAX_DESTINATIONS, row, col)
{
  m_Type = INFO.TYPE;
  setColor(m_Type);
  m_Filter = static_cast<AudioFilterStateVariable*>(m_Device);

  m_UIElement = ModuleUIElement(INFO.NAME, m_Color, INFO.PARAMETER_NAMES);
  m_Ports.setInputNames(INFO.INPUT_PORT_NAMES);
  m_Ports.setOutputNames(INFO.OUTPUT_PORT_NAMES);
  
  setCornerFrequency(DEFAULT_CORNER_FREQUENCY);
  setResonance(DEFAULT_RESONANCE);
  setOctaveControl(DEFAULT_OCTAVE_CONTROL);
}

void VCF::setCornerFrequency(float hz) 
{
  if (hz < 0.0 || hz > MAX_CORNER_FREQUENCY) 
    return;

  m_Filter->frequency(hz);
  m_CornerFrequency = hz;
  m_UIElement.updateParameter(0, m_CornerFrequency, MAX_CORNER_FREQUENCY);
}

void VCF::setResonance(float q) 
{
  if (q < MIN_RESONANCE || q > MAX_RESONANCE)
    return;

  m_Filter->resonance(q);
  m_Resonance = q;
  m_UIElement.updateParameter(1, m_Resonance - MIN_RESONANCE, MAX_RESONANCE - MIN_RESONANCE);
}

void VCF::setOctaveControl(float octaves) 
{ 
  if (octaves < 0.0 || octaves > MAX_OCTAVE_CONTROL) 
    return;

  m_Filter->octaveControl(octaves);
  m_OctaveControl = octaves;
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

std::string VCF::toString()
{
  //Format: VCF(frequency,resonance,octaveControl)[row,col]{ID}
  m_ID.makeSerializedID(INFO.NAME, m_Row, m_Col, 3);
  m_ID.addParam(m_CornerFrequency);
  m_ID.addParam(m_Resonance);
  m_ID.addParam(m_OctaveControl);
  return m_ID.getSerializedID();
}

Module* VCF::buildFromString(std::string s) 
{ 
  //Format: VCF{frequency,m_Resonance,m_OctaveControl}[row,col]<ID>
  size_t secondComma = s.find(",", s.find(",") + 1);
  size_t positionComma = s.find(",", s.find("["));

  float frequency =     Serialize::extractFloatBetween("{", ",", s);
  float resonance =     Serialize::extractFloatBetween(s.find(","), secondComma, s);
  float octaveControl = Serialize::extractFloatBetween(secondComma, s.find("}"), s);
  int row =             Serialize::extractIntBetween(s.find("["), positionComma, s);
  int col =             Serialize::extractIntBetween(positionComma, s.find("]"), s);
  int ID  =             Serialize::extractIntBetween("<", ">", s);

  VCF* vcf = new VCF(row, col);
  vcf->setID(ID);
  vcf->setCornerFrequency(frequency);
  vcf->setResonance(resonance);
  vcf->setOctaveControl(octaveControl);

  return vcf;
}

int VCF::getOpenOutputPort() const
{ 
  if (m_Ports.isOutputFree(0)) 
    return 0; 
  if (m_Ports.isOutputFree(1)) 
    return 2; 
  return -1; 
}

float VCF::getCornerFrequency() const { return m_CornerFrequency; }

float VCF::getResonance() const { return m_Resonance; }

float VCF::getOctaveControl() const { return m_OctaveControl; }