#include "DisplayManager.h"
#include "Patcher.h"
#include "Port.h"
#include "UIElements.h"
#include "Colors.h"
#include "Fonts/FreeSansBoldOblique9pt7b.h"
#include <cmath>
#include <cstring>

#define SCREEN_ROTATION 3

void DisplayManager::clearScreen()
{
  getFrameBuffer()->fillScreen(ILI9341_BLACK);
}

GFXcanvas16* DisplayManager::getFrameBuffer() { return getInstance().m_DoubleFrameBuffer.front(); }

GFXcanvas16* DisplayManager::getPreviousFrameBuffer() { return getInstance().m_DoubleFrameBuffer.back(); }

void DisplayManager::init()
{
  auto& tft = getTFT();
  auto& buffer1 = getInstance().m_FrameBuffer1;
  auto& buffer2 = getInstance().m_FrameBuffer2;
  tft.begin();
  tft.setRotation(SCREEN_ROTATION);
  tft.setFont(&FreeSansBoldOblique9pt7b);

  buffer1.setFont(&FreeSansBoldOblique9pt7b);
  buffer1.fillScreen(ILI9341_BLACK);
  buffer2.setFont(&FreeSansBoldOblique9pt7b);
  buffer2.fillScreen(ILI9341_WHITE);
}

GFXcanvas16* DisplayManager::popFrame()
{
  auto& frameBuffers = getInstance().m_DoubleFrameBuffer;
  GFXcanvas16* frame = frameBuffers.front();
  frameBuffers.pop();
  return frame;
}

void DisplayManager::renderFrame()
{
  auto& dm = getInstance();
  auto currentFrame = dm.popFrame();
  uint16_t* currentBuffer = currentFrame->getBuffer();
  uint16_t* previousBuffer = dm.getPreviousFrameBuffer()->getBuffer();
  
  constexpr size_t numBytesInRow = (SCREEN_WIDTH - 1) * sizeof(uint16_t);
  for (int y = 0; y < SCREEN_HEIGHT; y++)
  {        
    int offset = SCREEN_WIDTH * y;
    //skip current row if the pixels haven't changed
    if (std::memcmp(currentBuffer + offset , previousBuffer + offset, numBytesInRow) == 0)
      continue;

    memcpy(previousBuffer + offset, currentBuffer + offset, numBytesInRow);
    dm.m_TFT.drawRGBBitmap(0, y, currentBuffer + offset, SCREEN_WIDTH, 1);
  }

  dm.m_DoubleFrameBuffer.push(currentFrame);
}

void DisplayManager::displayBank(uint32_t color, const char* name)
{
  clearScreen();
  BankDisplayPage bankPage(name, Colors::to565(color), getFrameBuffer());
  bankPage.render();
}

void DisplayManager::displayMe(Module* m)
{
  ModuleDisplayPage modulePage(m, getFrameBuffer());
  clearScreen();
  modulePage.render();
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

void DisplayManager::print(const std::string& text, int size, uint32_t color)
{
  auto buffer = getFrameBuffer();
  color = Colors::to565(color);
  buffer->fillScreen(ILI9341_BLACK);
  buffer->setCursor(0, 80);
  buffer->setTextSize(size);
  buffer->setTextColor(color);

  //implement simple word wrapping
  int index = 0;
  for (int i = text.length(); i > 0; i -= 26)
  {
    buffer->println(text.substr(index, 26).c_str());
    index += 26;
  }
}

void DisplayManager::print(std::initializer_list<ColorText> words, int size)
{
  if (words.size() == 0) return;

  auto buffer = getFrameBuffer();
  clearScreen();
  buffer->setCursor(0, 80);
  buffer->setTextSize(size);

  //implement simple word wrapping
  for (ColorText word: words)
  {
    buffer->setTextColor(Colors::to565(word.color));
    buffer->print(word.text.c_str());
  }
}

void DisplayManager::print(const std::vector<ModuleUIElement::Parameter>& parameters, int size) 
{
  if (parameters.size() == 0) return;

  auto buffer = getFrameBuffer();
  buffer->fillScreen(ILI9341_BLACK);
  buffer->setCursor(0, 150);
  buffer->setTextSize(size);

  //implement simple word wrapping
  for (auto& p: parameters)
  {
    buffer->setTextColor(ILI9341_WHITE);
    buffer->print(p.name);
    buffer->print(": ");
    if(std::holds_alternative<float>(p.value))
      buffer->print(std::get<float>(p.value));
    else 
      buffer->print(std::get<int>(p.value));
    buffer->print("\n");
  }
}

void DisplayManager::drawWaveform(const std::array<float, AudioBufferInput::BUFFER_SIZE>& samples)
{
  WaveformDisplayFrame(samples, getFrameBuffer()).render();
}