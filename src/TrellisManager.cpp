#include "TrellisManager.h"

Adafruit_NeoTrellis TrellisManager::t_array[Grid::ROWS/4][Grid::COLS/4] = 
{
  { Adafruit_NeoTrellis(0x2E), Adafruit_NeoTrellis(0x30) },
  { Adafruit_NeoTrellis(0x31), Adafruit_NeoTrellis(0x2F) }
};

Adafruit_MultiTrellis TrellisManager::trellis((Adafruit_NeoTrellis*)t_array, Grid::ROWS/4, Grid::COLS/4);

void TrellisManager::init(TrellisCallback (*pressCallback)(keyEvent))
{
  if (!trellis.begin())
  {
    Serial.println("Error: failed to begin trellis");
    delay(1);
  }
  for (int y = 0; y < Grid::ROWS; y++)
  {
    for (int x = 0; x < Grid::COLS; x++)
    {
      trellis.activateKey(x, y, SEESAW_KEYPAD_EDGE_RISING, true);
      trellis.activateKey(x, y, SEESAW_KEYPAD_EDGE_FALLING, true);
      trellis.registerCallback(x, y, pressCallback);
      trellis.setPixelColor(x, y, 0x000000);
      trellis.show();
      delay(50);
    }
  }
}

void TrellisManager::update() { trellis.read(); }

//getNextKeyEvent() is not needed since we use callbacks