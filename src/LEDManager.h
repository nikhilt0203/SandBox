#include "TrellisManager.h"
#include "ModuleBuilder.h"
#include "Patcher.h"
#include "LEDFrameBuffer.h"
#pragma once

class LEDManager
{
public:
  static LEDManager& getInstance();

  static void init(Adafruit_MultiTrellis* t, Patcher* p);

  static void placeLed(int row, int col, uint32_t color);

  static void placeLed(Module* m);

  static void placeLEDElement(ModuleUIElement::LEDElement& ledElement);

  static void displayConnection(Module* src, Module* dest);

  static void refreshBank();

  static void clearGrid();

  static void refreshGrid(Module* m = nullptr);

  static void placeModule(Module* m);

  static void renderFrame();

  static void displayKeyboard();

private:
  static LEDFrame* getFrameBuffer();

  static LEDFrame* getPreviousFrameBuffer();

  static LEDFrame* popFrame();

private:
  Adafruit_MultiTrellis* m_Trellis;
  ModuleBuilder* m_Builder;
  Patcher* m_Patcher;
  LEDFrame m_LEDFrameBuffer1;
  LEDFrame m_LEDFrameBuffer2;
  std::queue<LEDFrame*> m_LEDDoubleFrameBuffer;


  LEDManager() 
  : m_Trellis(nullptr), 
    m_Patcher(nullptr),
    m_LEDFrameBuffer1{},
    m_LEDFrameBuffer2{},
    m_LEDDoubleFrameBuffer{}
  {}
  
  static bool checkPatcher() 
  { 
    return getInstance().m_Patcher != nullptr; 
  }
};