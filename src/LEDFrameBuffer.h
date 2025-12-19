#pragma once
#include "Grid.h"
#include <Arduino.h>

//Simple frame buffer for the 8x8 LED matrix

static constexpr size_t LED_FRAME_SIZE = Grid::ROWS * Grid::COLS;

using LEDFrameBuffer = std::array<uint32_t, LED_FRAME_SIZE>;

class LEDFrame
{
public:
  LEDFrame() = default;

  LEDFrameBuffer* data() 
  { 
    return &m_Data; 
  }

  void drawPixel(uint8_t row, uint8_t col, uint32_t color) 
  {
    m_Data[Grid::toPosition(row, col) % m_Data.size()] = color;
  }

  void drawPixel(uint8_t position, uint32_t color)
 {
    m_Data[position % m_Data.size()] = color;
  }

  void clear() 
  { 
    m_Data.fill(0x000000); 
  }

  uint32_t operator[](size_t index) const 
  { 
    return m_Data[index]; 
  }

private:
  LEDFrameBuffer m_Data{};
};