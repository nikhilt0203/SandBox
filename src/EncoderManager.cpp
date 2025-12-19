#include "EncoderManager.h"
#include "LEDManager.h"
#include "SelectionHandler.h"
#include "UIElements.h"

#define ENCODER_PIN_4B 40
#define ENCODER_PIN_4A 39
#define ENCODER_PIN_3B 38
#define ENCODER_PIN_3A 37
#define ENCODER_PIN_2B 36
#define ENCODER_PIN_2A 35
#define ENCODER_PIN_1B 34
#define ENCODER_PIN_1A 33
#define ENCODER_SW_PIN_1 25
#define ENCODER_SW_PIN_2 26
#define ENCODER_SW_PIN_3 27
#define ENCODER_SW_PIN_4 28

#define ENCODER_1_NUM 0
#define ENCODER_2_NUM 1
#define ENCODER_3_NUM 2
#define ENCODER_4_NUM 3

static int oldPositions[NUM_ENCODERS] = {0};

static Encoder encoders[NUM_ENCODERS] = { 
  Encoder(ENCODER_PIN_1A, ENCODER_PIN_1B), 
  Encoder(ENCODER_PIN_2A, ENCODER_PIN_2B),
  Encoder(ENCODER_PIN_3A, ENCODER_PIN_3B), 
  Encoder(ENCODER_PIN_4A, ENCODER_PIN_4B)
};

static bool isWaitingForInput = false;

void EncoderManager::init()
{
  for (size_t i{}; i < NUM_ENCODERS; i++) 
  {
    oldPositions[i] = encoders[i].read();
  }
}

void updateEncoder(size_t encoderNum)
{
  int newPosition = encoders[encoderNum].read();
  int change = newPosition - oldPositions[encoderNum];
 
  if (abs(change) < 4) 
  { 
    return; 
  }

  if (isWaitingForInput)
  {
    oldPositions[encoderNum] = newPosition;
    return;
  }

  int position = SelectionHandler::currentSelection();

  if (position < 0 || Grid::isInBank(position))
  {
    if (encoderNum == ENCODER_1_NUM)
    {
      ModuleBuilder::slideBankWindow((change > 0) ? 1 : -1);
      LEDManager::refreshGrid();
    }
    oldPositions[encoderNum] = newPosition;
    return;
  }

  if (Module* m = Module::getModuleAt(position))
  {
    m->changeParameter(encoderNum, (change > 0) ? 1 : -1);
    if (!SelectionHandler::isLocked()) 
    {
      DisplayManager::draw<ModuleDisplayPage>(m);
    }
    LEDManager::refreshGrid();
  }

  oldPositions[encoderNum] = newPosition;
}

void EncoderManager::update()
{
  for (size_t i{}; i < NUM_ENCODERS; i++)
  {
    updateEncoder(i);
  }
}

int EncoderManager::getInput(size_t encoderNum)
{
  return oldPositions[encoderNum];
}

void EncoderManager::waitForInput(bool wait)
{
  isWaitingForInput = wait;
}

bool EncoderManager::isWaiting()
{
  update();
  return isWaitingForInput;
}
