#include "LEDManager.h"
#include <initializer_list>

class LEDUIElement
{
public:
  struct LED 
  { 
    uint32_t color;
    uint8_t x; 
    uint8_t y; 

    LED() : color(0), x(-1), y(-1) {}
    LED(uint32_t color, uint8_t x, uint8_t y) : color(color), x(x), y(y) {}
    LED(uint32_t color, uint8_t position) : color(color), x(position / Grid::COLS), y(position % Grid::ROWS) {}
  };

  std::vector<LED> m_ActiveLEDs;
protected:
  virtual void render(const LEDManager& ledManager) = 0;
  LEDUIElement(std::initializer_list<LED> leds) : m_ActiveLEDs(leds) {}

  LEDUIElement(std::initializer_list<uint32_t> ledColors) 
  {
    int position = 0;
    for (auto& color : ledColors)
    {
      m_ActiveLEDs.emplace_back(color, position);
      position++;
    }
  }
};

class FileBrowserElement : public LEDUIElement
{
public:
  static constexpr uint32_t EMPTY_COLOR = 0x00FF00;
public:
  FileBrowserElement() 
  : LEDUIElement({EMPTY_COLOR, EMPTY_COLOR, EMPTY_COLOR, EMPTY_COLOR, EMPTY_COLOR, EMPTY_COLOR, EMPTY_COLOR, EMPTY_COLOR})
  {}

  void render(const LEDManager& ledManager) 
  {
  /*A B C D E F G H
    I J K L M N O P
    Q R S T U V W X
    Y Z 0 1 2 3 4 5
    6 7 8 9 _ - < >*/
  }
};