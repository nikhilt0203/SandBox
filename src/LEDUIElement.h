#pragma once
#include "UIElements.h"
#include "LEDFrameBuffer.h"
#include <initializer_list>

//Base class for LED UI elements
class LEDUIElement
{
public:
  LEDUIElement(LEDFrame* frame) : m_Frame(frame) {}

protected:
  virtual void draw() = 0;

protected:
  LEDFrame* m_Frame;
};

class KeyboardLEDElement : public LEDUIElement
{
public:
  static constexpr size_t NUM_KEYS = KeyboardElement::NUM_KEYS;

  static constexpr uint32_t KEY_COLOR = 0x000606;
  static constexpr uint32_t LIGHTER_KEY_COLOR = 0x008888;
  static constexpr uint32_t NUMBER_KEY_COLOR = 0x080808;
  static constexpr uint32_t BACKSPACE_KEY_COLOR = 0x060626;
  static constexpr uint32_t ENTER_KEY_COLOR = 0x008F00;
  static constexpr uint32_t ESCAPE_KEY_COLOR = 0x8F0000;

public:
  KeyboardLEDElement(LEDFrame* frame) : LEDUIElement(frame) {}

  void draw() override
  {
    if (!m_Frame) { 
      return; 
    }

    constexpr static uint8_t START_POS = Grid::COLS * 2;

    for (size_t key = 0; key < NUM_KEYS; key++)
    {
      const uint8_t row = (START_POS + key) / Grid::COLS;
      const uint8_t col = (START_POS + key) % Grid::COLS;
      uint32_t color = (key % 2 == 0) ? LIGHTER_KEY_COLOR : KEY_COLOR;

      switch (key)
      {
        case KeyboardElement::BACKSPACE_KEY:
          color = BACKSPACE_KEY_COLOR; break;

        case KeyboardElement::ENTER_KEY:
          color = ENTER_KEY_COLOR; break;

        case KeyboardElement::ESCAPE_KEY:
          color = ESCAPE_KEY_COLOR; break;

        default:
          if (key >= KeyboardElement::ZERO_KEY && key < KeyboardElement::BACKSPACE_KEY) {
            color = NUMBER_KEY_COLOR;
          }
          break;
      }
      m_Frame->drawPixel(row, col, color);
    }
  }
};