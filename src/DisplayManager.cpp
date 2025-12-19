#include "DisplayManager.h"
#include "Patcher.h"
#include "Port.h"
#include "Colors.h"
#include "Fonts/FreeSansBoldOblique9pt7b.h"
#include <cmath>
#include <cstring>

#define SCREEN_ROTATION 3

DisplayManager::DisplayManager() 
: m_TFT(TFT_CS, TFT_DC), 
  m_FrameBuffer1(SCREEN_WIDTH, SCREEN_HEIGHT), 
  m_FrameBuffer2(SCREEN_WIDTH, SCREEN_HEIGHT)
{
  m_DoubleFrameBuffer.push(&m_FrameBuffer1);
  m_DoubleFrameBuffer.push(&m_FrameBuffer2);
}

void DisplayManager::clearScreen()
{
  getFrameBuffer()->fillScreen(ILI9341_BLACK);
}

GFXcanvas16* DisplayManager::getFrameBuffer() 
{ 
  return getInstance().m_DoubleFrameBuffer.front(); 
}

GFXcanvas16* DisplayManager::getPreviousFrameBuffer() 
{ 
  return getInstance().m_DoubleFrameBuffer.back(); 
}

void DisplayManager::init()
{
  auto& tft = getTFT();
  auto& buffer1 = getInstance().m_FrameBuffer1;
  auto& buffer2 = getInstance().m_FrameBuffer2;
  tft.begin();
  tft.setRotation(SCREEN_ROTATION);
  tft.setFont(&FreeSansBoldOblique9pt7b);

  buffer1.setFont(&FreeSansBoldOblique9pt7b);
  buffer2.setFont(&FreeSansBoldOblique9pt7b);
  buffer1.fillScreen(ILI9341_BLACK);
  buffer2.fillScreen(ILI9341_WHITE);
}

[[nodiscard]] GFXcanvas16* DisplayManager::popFrame()
{
  auto& frameBuffers = getInstance().m_DoubleFrameBuffer;
  GFXcanvas16* frame = frameBuffers.front();
  frameBuffers.pop();
  return frame;
}

#include <chrono>
void DisplayManager::renderFrame()
{
  auto& instance = getInstance();
  if (!instance.m_FrameAvailable) {
    return;
  }

  GFXcanvas16* currentFrame = instance.popFrame();

  uint16_t* currentBuffer = currentFrame->getBuffer();
  uint16_t* previousBuffer = instance.getPreviousFrameBuffer()->getBuffer();

  constexpr static size_t rowSize = sizeof(uint16_t) * SCREEN_WIDTH;

  for (size_t y{}; y < SCREEN_HEIGHT; y++)
  {
    int offset = SCREEN_WIDTH * y;

    if (std::memcmp(currentBuffer + offset , previousBuffer + offset, rowSize) == 0) { 
      continue;
    }
    std::memcpy(previousBuffer + offset, currentBuffer + offset, rowSize);
    instance.m_TFT.drawRGBBitmap(0, y, currentBuffer + offset, SCREEN_WIDTH, 1);
  }

  instance.m_DoubleFrameBuffer.push(currentFrame);
  instance.m_FrameAvailable = false;
}

DisplayManager& DisplayManager::getInstance()
{
  static DisplayManager displayManager;
  return displayManager;
}

Adafruit_ILI9341& DisplayManager::getTFT()
{
  return getInstance().m_TFT;
}