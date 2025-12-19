#include "TrellisManager.h"

static Adafruit_NeoTrellis t_array[Grid::ROWS/4][Grid::COLS/4] = {
  { Adafruit_NeoTrellis(0x2E), Adafruit_NeoTrellis(0x30) },
  { Adafruit_NeoTrellis(0x31), Adafruit_NeoTrellis(0x2F) }
};

Adafruit_MultiTrellis TrellisManager::trellis((Adafruit_NeoTrellis*)t_array, Grid::ROWS / 4, Grid::COLS / 4);

struct KeyEventData 
{
  keyEvent event;
  unsigned long timePressed;
};

static KeyEventData s_StoredEvent;

static bool s_WaitingForInput = false;
static bool s_NewEvent = false;

void TrellisManager::init(TrellisCallback (*pressCallback)(keyEvent))
{
  if (!trellis.begin())
  {
    Serial.println("Error: failed to begin trellis");
    while(true) {}
  }

  for (size_t i{}; i < Grid::ROWS * Grid::COLS; i++)
  {
    trellis.activateKey(i, SEESAW_KEYPAD_EDGE_RISING, true);
    trellis.activateKey(i, SEESAW_KEYPAD_EDGE_FALLING, true);
    trellis.registerCallback(i, pressCallback);
    trellis.setPixelColor(i, 0x000000);
    trellis.show();
    delay(10);
  }
}

void TrellisManager::saveKeyEvent(keyEvent event, unsigned long timePressed)
{
  if (timePressed == s_StoredEvent.timePressed) {
    return;
  }
  s_StoredEvent = { event, timePressed };
  s_NewEvent = true;
}

void TrellisManager::waitForInput(bool wait)
{
  s_WaitingForInput = wait;
}

keyEvent TrellisManager::getKeyEvent()
{
  s_NewEvent = false;
  return s_StoredEvent.event;
}

bool TrellisManager::isWaiting()
{
  return s_WaitingForInput;
}

bool TrellisManager::isNewEvent()
{
  return s_NewEvent;
}