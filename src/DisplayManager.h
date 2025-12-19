#include "Adafruit_ILI9341.h"
#include "Adafruit_GFX.h"
#include "SPI.h"
#include <initializer_list>
#include <string>
#include <queue>
#include "AudioBufferInput.h"
#include "ModuleUIElement.h"
#pragma once

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define TFT_DC 23
#define TFT_CS 21

class Module;

class DisplayManager
{
private:
  Adafruit_ILI9341 m_TFT;
  GFXcanvas16 m_FrameBuffer1;
  GFXcanvas16 m_FrameBuffer2;
  std::queue<GFXcanvas16*> m_DoubleFrameBuffer;

  DisplayManager() 
  : m_TFT(TFT_CS, TFT_DC), 
    m_FrameBuffer1(SCREEN_WIDTH, SCREEN_HEIGHT), 
    m_FrameBuffer2(SCREEN_WIDTH, SCREEN_HEIGHT) 
  {
    m_DoubleFrameBuffer.push(&m_FrameBuffer1);
    m_DoubleFrameBuffer.push(&m_FrameBuffer2);
  }

public:
  struct ColorText
  {
    uint32_t color;
    std::string text;
    ColorText(uint32_t color, const std::string& text) : color(color), text(text) {}
  };

  static void init();
  static void renderFrame();
  static void clearScreen();
  static void displayMe(Module* m);
  static void displayBank(uint32_t color, const char* name);
  static void print(const std::string& text, int size = 2, uint32_t color = 0x00FF00);
  static void print(std::initializer_list<ColorText> words, int size = 2);
  static void print(const std::vector<ModuleUIElement::Parameter>& parameters, int size = 2);
  static void drawTextBox(const std::string& text, int y, uint32_t color, int size);
  static void drawWaveform(const std::array<float, AudioBufferInput::BUFFER_SIZE>& samples);
  static DisplayManager& getInstance();
  static Adafruit_ILI9341& getTFT();
  static GFXcanvas16* getFrameBuffer();
  static GFXcanvas16* getPreviousFrameBuffer();
  static GFXcanvas16* popFrame();
};