#include "VCA.h"

VCA::VCA() 
: Module(new AudioEffectVCA(), INFO.NAME, MAX_INPUTS, MAX_OUTPUTS)
{ 
  m_Type = INFO.TYPE;
  m_UIElement = ModuleUIElement(INFO);
  m_Ports.setInputNames(INFO.INPUT_PORT_NAMES);

  m_VCA = static_cast<AudioEffectVCA*>(m_Device);

  setMix(DEFAULT_MIX);
  setGain(DEFAULT_GAIN);
} 

void VCA::changeParameter(int parameterNum, int amt) 
{ 
  switch (parameterNum) 
  { 
    case 0: setMix(m_Mix * powf(ADJUSTMENT_CURVE, amt)); break;
    case 1: setGain(m_Gain * powf(ADJUSTMENT_CURVE, amt)); break;
  } 
}

void VCA::setMix(float mix) 
{ 
  m_Mix = std::clamp(mix, 0.0f, MAX_MIX);
  m_VCA->setCVAmount(mix);
  m_UIElement.updateParameter(0, m_Mix, MAX_MIX); 
}

void VCA::setGain(float gain) 
{ 
  m_Gain = std::clamp(gain, 0.0f, MAX_GAIN_);
  m_VCA->setOutputGain(gain);
  m_UIElement.updateParameter(1, m_Gain, MAX_GAIN_); 
}

std::string_view VCA::serialize() 
{ 
  m_ID.makeSerializationID(m_UIElement);
  m_ID.addParam(m_Mix);
  m_ID.addParam(m_Gain);
  return m_ID.getSerializationID(); 
}

Module* VCA::buildFromString(std::string_view s) 
{
  //Format: VCA{mix,gain}[row,col]<ID>
  int positionComma = s.find(",", s.find("["));

  float mix = Serialize::extractFloatBetween("{", ",", s);
  float gain = Serialize::extractFloatBetween(",", "}", s);
  int row = Serialize::extractIntBetween(s.find("["), positionComma, s);
  int col = Serialize::extractIntBetween(positionComma, s.find("]"), s);
  int ID =  Serialize::extractIntBetween("<", ">", s);

  VCA* vca = new VCA();
  vca->setMix(mix);
  vca->setGain(gain);
  vca->setID(ID);
  vca->getLEDElement().setPosition(row, col);
  return vca;
}