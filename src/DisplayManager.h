#include "Adafruit_ILI9341.h"
#include "Adafruit_GFX.h"
#include "SPI.h"
#include <initializer_list>
#include <string>
#include <type_traits>
#include <queue>
#include "ModuleUIElement.h"
#include "audio/AudioBufferInput.h"
#pragma once

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define TFT_DC 15
#define TFT_CS 14

class Module;
class UIElement;

class DisplayManager
{
public:

  static void init();

  static void renderFrame();

  static void clearScreen();

  static void displayMe(Module* m);

  static void displayBank(const char* name, const char* description, uint32_t color);

  static void displayPatch(Module* src, int srcPort, Module* dest, int destPort, bool isPatched);

  static void print(const std::vector<ModuleUIElement::Parameter>& parameters, int size = 2);

  static void drawTextBox(std::string_view text, int y, uint32_t color, int size);

  static void drawWaveform(const SampleBuffer& samples);

  static void displayKeyboard(size_t pressedKeyIndex, std::string_view currentText);

  static DisplayManager& getInstance();

  static Adafruit_ILI9341& getTFT();  

  template<typename T, typename... Args>
  static void draw(Args&&... args)
  {
    static_assert(std::is_base_of<UIElement, T>::value, "Type parameter must be derived from UIElement");
    
    T element(std::forward<Args>(args)..., getFrameBuffer());
    element.draw();
    getInstance().m_FrameAvailable = true;
  }

private:
  static GFXcanvas16* getFrameBuffer();

  static GFXcanvas16* getPreviousFrameBuffer();

  static GFXcanvas16* popFrame();

  DisplayManager();

private:
  Adafruit_ILI9341 m_TFT;
  GFXcanvas16 m_FrameBuffer1;
  GFXcanvas16 m_FrameBuffer2;
  std::queue<GFXcanvas16*> m_DoubleFrameBuffer;
  bool m_FrameAvailable{false};
};