#include "EncoderManager.h"
#include "LEDManager.h"
#include "SelectionManager.h"

namespace EncoderManager {

int oldPositions[NUM_ENCODERS];

Encoder encoders[NUM_ENCODERS] =
  { 
    Encoder(10, 24), Encoder(27, 28),
    Encoder(30, 31), Encoder(25, 26) 
  };

std::vector<std::pair<Module*, int>> encoderMap = 
  {
    {nullptr, -1},
    {nullptr, -1},
    {nullptr, -1},
    {nullptr, -1}
  };
}
void EncoderManager::init()
{
  for (int i = 0; i < NUM_ENCODERS; i++) 
  {
    oldPositions[i] = encoders[i].read();
  }
}

void updateEncoder(int encoderNum)
{
  int newPosition = EncoderManager::encoders[encoderNum].read();
  int change  = newPosition - EncoderManager::oldPositions[encoderNum];
 
  if (abs(change) < 4) return; 
  int position = SelectionManager::getPreviousSelection();
  if (position == -1)
  {
    if (encoderNum == 3)
    {
      ModuleBuilder::slideBankWindow(change > 0 ? 1 : -1);
      EncoderManager::oldPositions[encoderNum] = newPosition;
      LEDManager::refreshBank();
    }
    return;
  }
  
  Module* m = Module::getModule(position);
  if (!m) return;

  DisplayManager::displayMe(m);
  EncoderManager::oldPositions[encoderNum] = newPosition;
  m->changeParameter(encoderNum, change > 0 ? 1 : -1);
  LEDManager::refreshGrid(m);
}

void EncoderManager::update()
{
  for (int i = 0; i < NUM_ENCODERS; i++)
  {
    updateEncoder(i);
  }
}

