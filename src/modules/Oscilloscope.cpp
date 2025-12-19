#include "Oscilloscope.h"
#include "UIElements.h"

#define MAX_INPUTS 1
#define MAX_OUTPUTS 1

void Oscilloscope::update()
{
  if (m_AudioBuffer->available() && !m_Ports.isInputFree(0)) 
  {
    m_DisplayManager.draw<WaveformDisplayFrame>(m_AudioBuffer->flush());
  }
}

Module* Oscilloscope::buildFromString(std::string_view s)
{
  //Format: Oscillator{waveform,frequency,octaveControl}[row,col]<ID>_type_
  size_t positionComma = s.find(",", s.find("["));

  int row = Serialize::extractIntBetween(s.find("["), positionComma, s);
  int col = Serialize::extractIntBetween(positionComma, s.find("]"), s);
  int ID  = Serialize::extractIntBetween("<", ">", s);

  Oscilloscope* scope = new Oscilloscope();
  scope->setID(ID);
  scope->getLEDElement().setPosition(row, col);
  return scope;
}
