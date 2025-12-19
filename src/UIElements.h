#pragma once
#include "Adafruit_ILI9341.h"
#include "Module.h"
#include "Port.h"
#include "Colors.h"
#include <string>

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

namespace Rectangle 
{
  static uint16_t centerToUpperLeftX(uint16_t x, uint16_t width) { return x - width / 2; }
  static uint16_t centerToUpperLeftY(uint16_t y, uint16_t height) { return y - height / 2; }
}

/*
  Base class for all UI elements
*/
class UIElement
{
public:
  uint16_t m_X;
  uint16_t m_Y;
  uint16_t m_Width;
  uint16_t m_Height;

public:
  virtual void draw() const = 0;
  
  virtual void alignX(uint16_t refWidth, uint16_t refX) { m_X = refX + (refWidth - m_Width) / 2; }

  virtual void alignY(uint16_t refHeight, uint16_t refY) { m_Y = refY + (refHeight - m_Height) / 2; }

  virtual void centerX() { m_X = (SCREEN_WIDTH - m_Width) / 2; }

  virtual void centerY() { m_Y = (SCREEN_HEIGHT - m_Height) / 2; }

  virtual void setX(uint16_t x) { m_X = x; }
  
  virtual void setY(uint16_t y) { m_Y = y; }

  virtual void clearCanvas() const { m_Canvas->fillScreen(ILI9341_BLACK); }

protected:
  UIElement(uint16_t x, uint16_t y, uint16_t w, uint16_t h, GFXcanvas16* canvas) 
  : m_X(x), m_Y(y), m_Width(w), m_Height(h), m_Canvas(canvas) {}

  UIElement() : UIElement(0, 0, 0, 0, nullptr) {}
  
protected:
  GFXcanvas16* m_Canvas;
};

/*
  For arbitary text display
*/
class TextElement : public UIElement
{
public:
  static constexpr int LINE_SPACING = 3;

public:
  TextElement(uint16_t x, uint16_t y, const char* text, uint16_t color, uint8_t size, GFXcanvas16* canvas)
  : UIElement(x, y, {}, {}, canvas),
    m_Text(text),
    m_Color(color),
    m_TextSize(size)
  {
    if (!m_Canvas || !text) 
    { 
      return; 
    }

    int16_t textX, textY;
    uint16_t textWidth, textHeight, textHeightMin;

    m_Canvas->setTextSize(m_TextSize);
    m_Canvas->getTextBounds(m_Text.data(), 0, 0, &textX, &textY, &textWidth, &textHeight);
    m_Width = textWidth;
    m_OffsetX = textX * -1;
    m_OffsetY = textY * -1;

    m_Canvas->getTextBounds("l", 0, 0, &textX, &textY, &textWidth, &textHeightMin);
    m_Height = std::min(textHeightMin, textHeight);
  }

  void draw() const override
  {
    if (!m_Canvas || !m_Text.data()) 
    { 
      return; 
    }
  
    int16_t cursorX = m_X + m_OffsetX;
    int16_t cursorY = m_Y + m_OffsetY;

    m_Canvas->setTextSize(m_TextSize);
    m_Canvas->setTextColor(m_Color);
    m_Canvas->setCursor(cursorX, cursorY);

    if (m_WrappingPx == 0 || m_Width <= m_WrappingPx)
    {
      m_Canvas->print(m_Text.data());
      return;
    }

    if (m_Width <= m_WrappingPx) {
      return;
    }

    //line wrapping
    size_t charIndex = 0;
    while (charIndex < m_Text.length())
    {
      m_Canvas->print(m_Text.at(charIndex));
      charIndex += 1;

      if (m_Canvas->getCursorX() >= static_cast<int16_t>(m_X + m_WrappingPx))
      {
        cursorY += m_Height + (m_TextSize * LINE_SPACING);
        cursorX = m_X + m_OffsetX;
        m_Canvas->setCursor(cursorX, cursorY);
      }
    }
  }

  void setWrapping(uint16_t wrappingPx) 
  { 
    m_WrappingPx = wrappingPx; 
  }

public:
  std::string_view m_Text;
  uint16_t m_Color;
  uint8_t m_TextSize;
  int m_OffsetX{};
  int m_OffsetY{};
  size_t m_WrappingPx{};
};

/*
  Knob with label and variable turn amount
*/
class KnobElement : public UIElement
{
public:
  static constexpr int KNOB_RADIUS = 25;
  static constexpr int KNOB_WIDTH = KNOB_RADIUS * 2 + 1;

  static constexpr int INDICATOR_RADIUS = 6;
  static constexpr float INDICATOR_SCALE = 0.9f;

  static constexpr int MIN_ROTATION = 120;
  static constexpr int MAX_ROTATION = 420;

  static constexpr int Y_OFFSET = 20;
  static constexpr int X_OFFSET = 2;

  static constexpr int TEXT_SIZE = 1;
  static constexpr int MAX_TEXT_LENGTH = 6;

  static constexpr uint16_t LABEL_COLOR = ILI9341_LIGHTGREY;
  static constexpr uint16_t OUTER_CIRCLE_COLOR = ILI9341_WHITE;
  static constexpr uint16_t INNER_CIRCLE_COLOR = ILI9341_DARKGREY;
  static constexpr uint16_t INDICATOR_COLOR = ILI9341_WHITE;

public:
  KnobElement() : UIElement(), m_PercentTurned(0.0), m_Label("") {}

  KnobElement(uint16_t x, uint16_t y, float percentTurned, const char* label, GFXcanvas16* canvas)
  : UIElement(x, y, KNOB_WIDTH, KNOB_WIDTH, canvas), 
    m_PercentTurned(std::clamp(percentTurned, 0.0f, 1.0f)), 
    m_Label(label) 
  {}

  void draw() const override
  {
    //Knob
    m_Canvas->drawCircle(m_X, m_Y, KNOB_RADIUS, OUTER_CIRCLE_COLOR);
    m_Canvas->drawCircle(m_X, m_Y, 10, INNER_CIRCLE_COLOR);

    int degreesTurned = MIN_ROTATION + (m_PercentTurned * (MAX_ROTATION - MIN_ROTATION));

    int indicatorX = m_X + std::cos(degreesTurned % 360 * (M_PI / 180.0f)) * (KNOB_RADIUS - INDICATOR_RADIUS) * INDICATOR_SCALE;
    int indicatorY = m_Y + std::sin(degreesTurned % 360 * (M_PI / 180.0f)) * (KNOB_RADIUS - INDICATOR_RADIUS) * INDICATOR_SCALE;

    m_Canvas->fillCircle(indicatorX, indicatorY, INDICATOR_RADIUS, INDICATOR_COLOR);

    //Text
    int16_t textX, textY;
    uint16_t textWidth, textHeight;
    m_Canvas->setTextSize(TEXT_SIZE);
    m_Canvas->getTextBounds(m_Label, 0, m_Y, &textX, &textY, &textWidth, &textHeight);

    textX = (m_X - KNOB_RADIUS) + ((m_Width - textWidth) / 2) - X_OFFSET;
    textY = m_Y + KNOB_RADIUS + Y_OFFSET;

    m_Canvas->setCursor(textX, textY);
    m_Canvas->setTextColor(LABEL_COLOR);
    m_Canvas->print(m_Label);
  }

public:
  float m_PercentTurned;
  const char* m_Label;
};

/*
  Row of colored squares representing the current state of the given module ports.
  port labels should be at most 3 characters.
*/
class PortsDisplayElement : public UIElement
{
public:
  static constexpr int SQUARE_WIDTH = 26;
  static constexpr int SQUARE_RADIUS = 6;

  static constexpr int SPACING_PX = 7;

  static constexpr int TEXT_Y_OFFSET = 0;
  static constexpr int TEXT_X_OFFSET = 1;

public:
  PortsDisplayElement(uint16_t x, uint16_t y, const std::vector<Port>& ports, GFXcanvas16* canvas)
  : UIElement(x, y, {}, SQUARE_WIDTH, canvas), 
    m_Ports(ports)
  {
    const size_t numPorts = m_Ports.size();
    
    m_Width = (m_Ports.size() == 0) ? 
      0 : SQUARE_WIDTH * numPorts + SPACING_PX * (numPorts - 1);
  }

  void draw() const override
  {
    size_t numConnections = m_Ports.size();

    if (numConnections == 0) { 
      return; 
    }

    uint32_t portColors[numConnections];

    size_t index = 0;
    for (const Port& port: m_Ports)
    {
      if (!port.m_ConnectedModule)  
      {
        portColors[index] = 0; 
        index++;
        continue;
      }

      portColors[index] = port.m_ConnectedModule->getColor();
      index++;
    }

    for (size_t i{}; i < numConnections; i++)
    {
      int x = m_X + i * (SQUARE_WIDTH + SPACING_PX);
      uint16_t textColor;

      if (portColors[i] != 0)
      {
        m_Canvas->fillRoundRect(x, m_Y, SQUARE_WIDTH, SQUARE_WIDTH, SQUARE_RADIUS, Colors::to565(portColors[i]));
        textColor = ILI9341_BLACK;
      }
      else
      {
        m_Canvas->drawRoundRect(x, m_Y, SQUARE_WIDTH, SQUARE_WIDTH, SQUARE_RADIUS - 1, ILI9341_DARKGREY);
        textColor = ILI9341_DARKGREY;
      }

      TextElement portLabel(0, 0, m_Ports.at(i).m_Name, textColor, 1, m_Canvas);
      portLabel.alignX(SQUARE_WIDTH, x - TEXT_X_OFFSET);
      portLabel.alignY(SQUARE_WIDTH, m_Y);
      portLabel.draw();
    }
  }

public:
  const std::vector<Port>& m_Ports;
};

/*
  A small lock icon.
*/
class LockIconElement : public UIElement
{
public:
  LockIconElement(uint16_t x, uint16_t y, bool isLocked, GFXcanvas16* canvas)
  : UIElement(x, y, BODY_WIDTH, BODY_HEIGHT + LATCH_RADIUS + Y_OFFSET, canvas), 
    m_Color(ILI9341_DARKGREY),
    m_isLocked(isLocked)
  {}

  void draw() const override
  {
    m_Canvas->fillRoundRect(
      Rectangle::centerToUpperLeftX(m_X, m_Width), 
      Rectangle::centerToUpperLeftY(m_Y, m_Height), 
      BODY_WIDTH, BODY_HEIGHT, 3, m_Color);

    const uint16_t circleY = Rectangle::centerToUpperLeftY(m_Y, m_Height) - Y_OFFSET;

    m_Canvas->drawCircle(m_X, circleY, LATCH_RADIUS, m_Color);
    m_Canvas->drawCircle(m_X, circleY, LATCH_RADIUS + 1, m_Color); //thicken line

    //Open/closed latch
    if (!m_isLocked)
    { 
      m_Canvas->fillRect(
        Rectangle::centerToUpperLeftX(m_X, m_Width) + 2, 
        Rectangle::centerToUpperLeftY(m_Y, m_Height) - 3, 
        4, 3, ILI9341_BLACK);
    }
  }

public:
  static constexpr uint16_t BODY_WIDTH = 13;
  static constexpr uint16_t BODY_HEIGHT = 13;
  static constexpr uint16_t LATCH_RADIUS = 3;
  static constexpr uint16_t Y_OFFSET = 2;

public:
  uint16_t m_Color;
  bool m_isLocked;
};

/*
  A colored text box.
*/
class TextBoxElement : public UIElement
{
public:
  static constexpr int WIDTH_PADDING = 3;
  static constexpr int HEIGHT_PADDING = 4;

  static constexpr int MAX_BOX_HEIGHT = 45;
  static constexpr int BOX_X_OFFSET = 5;
  static constexpr int BOX_Y_OFFSET = 3;

public:
  TextBoxElement(uint16_t x, uint16_t y, const char* text, uint16_t color, uint8_t size, GFXcanvas16* canvas)
  : UIElement(x, y, {}, {}, canvas),
    m_TextElement(x, y, text, color, size, canvas), 
    m_Color(color)
  {
    m_Width = m_TextElement.m_Width + (WIDTH_PADDING * 2);
    m_Height = m_TextElement.m_Height + (HEIGHT_PADDING * 2);
  }

  void draw() const override
  {
    m_TextElement.draw();
    m_Canvas->drawRoundRect(m_X, m_Y - HEIGHT_PADDING, m_Width, m_Height, 5, m_Color);
  }

  void alignX(uint16_t refWidth, uint16_t refX) override
  {
    UIElement::alignX(refWidth, refX);
    m_TextElement.alignX(refWidth, refX);
  }

  void alignY(uint16_t refHeight, uint16_t refY) override
  {
    UIElement::alignY(refHeight, refY);
    m_TextElement.alignY(refHeight, refY);
  }

  void centerX() override
  {
    UIElement::centerX();
    m_TextElement.centerX();
  }

  void centerY() override
  {
    UIElement::centerY();
    m_TextElement.centerY();
  }

  void setX(uint16_t x) override
  {
    UIElement::setX(x);
    m_TextElement.setX(x);
  }

  void setY(uint16_t y) override
  {
    UIElement::setY(y);
    m_TextElement.setY(y);
  }

public:
  TextElement m_TextElement;
  uint16_t m_Color;
};

/*
  Page representing the currently selected module
*/
class ModuleDisplayPage : public UIElement
{
public:
  static constexpr int TEXT_SIZE = 3;
  static constexpr int TEXT_BOX_Y = 45;

  static constexpr int INPUTS_X = 20;
  static constexpr int INPUTS_Y = 8;

  static constexpr int OUTPUTS_X = 20; 
  static constexpr int OUTPUTS_Y = 94;

  static constexpr int LOCK_X = SCREEN_WIDTH - 18;
  static constexpr int LOCK_Y = SCREEN_WIDTH - 18;

  static constexpr int KNOBS_Y = 160;
  static constexpr int EMPTY_KNOB_RADIUS = KnobElement::KNOB_RADIUS - 3;
  static constexpr int KNOB_SPACING_PX = (SCREEN_WIDTH - (KnobElement::KNOB_RADIUS * 2 * 4)) / 5;

public:
  ModuleDisplayPage(Module* m, GFXcanvas16* canvas)
  : UIElement({}, {}, SCREEN_WIDTH, SCREEN_HEIGHT, canvas),
    m_TextBox(SCREEN_WIDTH / 2, TEXT_BOX_Y, m->getName().data(), 
      Colors::to565(Colors::getColor(m->getType())), TEXT_SIZE, canvas),
    m_Inputs(INPUTS_X, INPUTS_Y, m->getInputs(), canvas),
    m_Outputs(OUTPUTS_X, OUTPUTS_Y, m->getOutputs(), canvas),
    m_Module(m)
  {
    m_TextBox.alignX(SCREEN_WIDTH, 0);
  }

  void draw() const override
  {
    if (!m_Module) {
      return; 
    }

    clearCanvas();

    m_TextBox.draw();
    m_Inputs.draw();
    m_Outputs.draw();

    auto& parameters = m_Module->getDisplayElement().parameters;
    const size_t numParameters = parameters.size();
    
    const uint16_t startX = KnobElement::KNOB_RADIUS + KNOB_SPACING_PX;

    for (size_t i{}; i < 4; i++)
    {
      const uint16_t x = startX + (KnobElement::KNOB_RADIUS * 2 * i) + (KNOB_SPACING_PX * i);

      if (i < numParameters)
      {
        KnobElement knob(x, KNOBS_Y, parameters[i].percentage, parameters[i].name, m_Canvas);
        knob.draw();
      }
      else
      {
        m_Canvas->drawCircle(x, KNOBS_Y, EMPTY_KNOB_RADIUS, ILI9341_DARKGREY);
      }
    }
  }

public:
  TextBoxElement m_TextBox;
  KnobElement m_Knobs[4];
  PortsDisplayElement m_Inputs;
  PortsDisplayElement m_Outputs;
  Module* m_Module;
};

/*
  Page displaying the current bank information
*/
class BankDisplayPage : public UIElement
{
public:
  BankDisplayPage(const char* name, const char* description, uint16_t color, GFXcanvas16* canvas)
  : UIElement({}, {}, SCREEN_WIDTH, SCREEN_HEIGHT, canvas), 
    m_ModuleNameText(m_X, TEXT_BOX_Y, name, color, TEXT_BOX_SIZE, canvas),
    m_DescriptionText(m_X, m_ModuleNameText.m_Y + TEXT_SPACING, description, ILI9341_LIGHTGREY, DESCRIPTION_SIZE, canvas)
  {
    m_ModuleNameText.alignX(m_Width, m_X);
    m_DescriptionText.alignX(m_Width, m_X); 
    //m_Description.setWrapping(SCREEN_WIDTH - 60);
  }
 
  void draw() const override 
  {
    clearCanvas();

    m_ModuleNameText.draw();
    m_DescriptionText.draw();

    TextElement tip(TIP_TEXT_X, TIP_TEXT_Y, "Scroll with knob 1.", ILI9341_DARKGREY, 1, m_Canvas);
    tip.centerX();
    tip.draw();
  }

public:
  static constexpr uint8_t TEXT_BOX_SIZE = 3;
  static constexpr uint8_t DESCRIPTION_SIZE = 1;

  static constexpr uint8_t TEXT_BOX_Y = 70;
  static constexpr uint8_t TEXT_SPACING = 60;
  static constexpr uint8_t TIP_TEXT_X = 50;
  static constexpr uint8_t TIP_TEXT_Y = 200;

private:
  TextBoxElement m_ModuleNameText;
  TextElement m_DescriptionText;
};

/*
  Displays the waveform of a given audio buffer
*/
#include "audio/AudioBufferInput.h"
class WaveformDisplayFrame : public UIElement
{
public:
  static constexpr float WAVEFORM_SCALE = 0.7f;
  static constexpr float BORDER_SCALE = 0.85f;

  static constexpr int TITLE_Y = 6;
  static constexpr int WINDOW_X = SCREEN_WIDTH * (1 - BORDER_SCALE) / 2;
  static constexpr int WINDOW_Y = SCREEN_HEIGHT * (1 - BORDER_SCALE) / 2 + 7;
  static constexpr int WINDOW_WIDTH = SCREEN_WIDTH * BORDER_SCALE;
  static constexpr int WINDOW_HEIGHT = SCREEN_HEIGHT * BORDER_SCALE;

  static constexpr int LOCK_X = 18;
  static constexpr int LOCK_Y = 18;

  static constexpr uint16_t WAVEFORM_COLOR = ILI9341_GREEN;
  static constexpr uint16_t WAVEFORM_CLIPPING_COLOR = ILI9341_RED;
  static constexpr uint16_t ZERO_LINE_COLOR = ILI9341_DARKGREY;
  static constexpr uint16_t BORDER_COLOR = ILI9341_DARKCYAN;

  static constexpr size_t SAMPLE_INCREMENT = 8;

public:
  WaveformDisplayFrame(SampleBuffer sampleArray, GFXcanvas16* canvas)
  : UIElement({}, {}, WINDOW_WIDTH, WINDOW_HEIGHT, canvas), 
    m_SampleArray(sampleArray)
  {}

  void draw() const override
  {
    if (m_SampleArray.empty()) { 
      return; 
    }

    clearCanvas();

    m_Canvas->drawRoundRect(WINDOW_X, WINDOW_Y, WINDOW_WIDTH, WINDOW_HEIGHT, 20, BORDER_COLOR);
    m_Canvas->drawFastHLine(WINDOW_X, WINDOW_Y + WINDOW_HEIGHT / 2, WINDOW_WIDTH, ZERO_LINE_COLOR);

    TextElement title(0, TITLE_Y, "Oscilloscope", ILI9341_CYAN, 1, m_Canvas);
    title.alignX(WINDOW_WIDTH, WINDOW_X);
    title.draw();

    uint16_t color;
    int prevX = 0;
    int prevY = 0;

    for (size_t i{}; i < m_SampleArray.size(); i += SAMPLE_INCREMENT)
    {
      const float sampleValue = m_SampleArray.at(i);
      const float percentDrawn = static_cast<float>(i) / (m_SampleArray.size() - 1);

      const int x = (WINDOW_X + 1) + static_cast<int>(percentDrawn * (WINDOW_WIDTH - 1));
      const int y = (WINDOW_HEIGHT / 2) - (WINDOW_HEIGHT / 2) * (sampleValue * WAVEFORM_SCALE) + WINDOW_Y;

      if (i == 0)
      {
        prevX = x;
        prevY = y;
        continue;
      }

      if (prevX == x && prevY == y) { 
        continue; 
      }

      color = (sampleValue >= 1.0f || sampleValue <= -1.0f) ? WAVEFORM_CLIPPING_COLOR : WAVEFORM_COLOR;

      m_Canvas->drawLine(prevX, prevY, x, y, color);
      prevX = x;
      prevY = y;
    }
  }

private:
  SampleBuffer m_SampleArray;
};

/*
  Page displaying a connection between two modules
*/
class PatchDisplayPage : public UIElement
{
public:
  static constexpr size_t TEXT_SIZE = 2;
  static constexpr uint8_t TEXT_SPACING = 160;

  static constexpr uint8_t PORTS_Y_OFFSET = 5;

  static constexpr int8_t ARROWHEAD_X_OFFSET = -7;
  static constexpr int8_t ARROWHEAD_Y_OFFSET = -2;

  static constexpr uint16_t UNPATCHED_WIRE_COLOR = ILI9341_DARKGREY;

public:
  PatchDisplayPage(Module* src, int srcPort, Module* dest, int destPort, bool isPatched, GFXcanvas16* canvas)
  : UIElement({}, {}, SCREEN_WIDTH, SCREEN_HEIGHT, canvas),
    m_SourceModule(src), 
    m_DestinationModule(dest),
    m_SourcePort(srcPort),
    m_DestinationPort(destPort),
    m_isPatched(isPatched),
    m_WireColor(Colors::to565(Colors::changeBrightness(src->getColor(), 0.8f))),

    m_SourceName({}, {}, src->getName().data(), Colors::to565(src->getColor()), TEXT_SIZE, canvas),
    m_DestinationName({}, {}, dest->getName().data(), Colors::to565(dest->getColor()), TEXT_SIZE, canvas),
    m_SourceOutputs({}, {}, src->getOutputs(), canvas),
    m_DestinationInputs({}, {}, dest->getInputs(), canvas)
  {
    m_DestinationName.centerX();
    m_SourceName.centerX();
    m_SourceOutputs.centerX();
    m_DestinationInputs.centerX();

    m_SourceName.setY((SCREEN_HEIGHT - TEXT_SPACING - m_DestinationName.m_Height) / 2);
    m_DestinationName.setY(m_SourceName.m_Y + TEXT_SPACING);
    m_SourceOutputs.setY(m_SourceName.m_Y + m_SourceName.m_Height + PORTS_Y_OFFSET);
    m_DestinationInputs.setY(m_DestinationName.m_Y - m_DestinationName.m_Height - PORTS_Y_OFFSET);

    m_X = std::min(m_SourceName.m_X, m_DestinationName.m_X);
    m_Y = std::min(m_SourceName.m_Y, m_DestinationName.m_Y);
    m_Width = std::max(m_SourceName.m_Width, m_DestinationName.m_Width);
    m_Height = m_SourceName.m_Height + m_DestinationName.m_Height + TEXT_SPACING;
  }

  void draw() const override
  {
    clearCanvas();

    m_SourceName.draw();
    m_DestinationName.draw();
    m_SourceOutputs.draw();
    m_DestinationInputs.draw();

    int16_t wireStartX = m_SourceOutputs.m_X + (PortsDisplayElement::SQUARE_WIDTH + PortsDisplayElement::SPACING_PX) 
      * m_SourcePort + (PortsDisplayElement::SQUARE_WIDTH / 2);

    int16_t wireStartY = m_SourceOutputs.m_Y + PortsDisplayElement::SQUARE_WIDTH;

    int16_t wireEndX = m_DestinationInputs.m_X + (PortsDisplayElement::SQUARE_WIDTH + PortsDisplayElement::SPACING_PX) 
      * m_DestinationPort + (PortsDisplayElement::SQUARE_WIDTH / 2);

    int16_t wireEndY = m_DestinationInputs.m_Y;

    m_isPatched ? drawPatched(wireStartX, wireStartY, wireEndX, wireEndY) 
                : drawUnpatched(wireStartX, wireStartY, wireEndX, wireEndY);
  }

  void drawPatched(int16_t startX, int16_t startY, int16_t endX, int16_t endY) const
  {
    int16_t midpointY = (endY + startY) / 2;

    if (startX == endX) {
      m_Canvas->drawFastVLine(startX, startY, endY - startY, m_WireColor);
        m_Canvas->drawFastVLine(startX + 1, startY, endY - startY, m_WireColor);
        m_Canvas->drawFastVLine(startX - 1, startY, endY - startY, m_WireColor);
    }
    else
    {
      m_Canvas->drawFastVLine(startX, startY, midpointY - startY, m_WireColor);
      m_Canvas->drawFastHLine(startX, midpointY, endX - startX, m_WireColor);
      m_Canvas->drawFastVLine(endX, midpointY, endY - midpointY, m_WireColor);

        m_Canvas->drawFastVLine(startX + 1, startY, midpointY - startY, m_WireColor);
        m_Canvas->drawFastHLine(startX, midpointY + 1, endX - startX, m_WireColor);
        m_Canvas->drawFastVLine(endX + 1, midpointY, endY - midpointY, m_WireColor);

        m_Canvas->drawFastVLine(startX - 1, startY, midpointY - startY, m_WireColor);
        m_Canvas->drawFastHLine(startX, midpointY - 1, endX - startX, m_WireColor);
        m_Canvas->drawFastVLine(endX - 1, midpointY, endY - midpointY, m_WireColor);
    }
    //Arrowhead
    m_Canvas->setTextSize(1);
    m_Canvas->setTextColor(m_WireColor);
    m_Canvas->setCursor(endX + ARROWHEAD_X_OFFSET, endY + ARROWHEAD_Y_OFFSET);
    m_Canvas->print("V");
  }

  void drawUnpatched(int16_t startX, int16_t startY, int16_t endX, int16_t endY) const
  {
    m_Canvas->setTextSize(1);
    m_Canvas->setTextColor(ILI9341_RED);

    m_Canvas->setCursor(startX + ARROWHEAD_X_OFFSET, startY + PortsDisplayElement::SQUARE_WIDTH / 2);
    m_Canvas->print("X");

    m_Canvas->setCursor(endX + ARROWHEAD_X_OFFSET, endY);
    m_Canvas->print("X");
  }

private:
  Module* m_SourceModule;
  Module* m_DestinationModule;
  size_t m_SourcePort;
  size_t m_DestinationPort;
  bool m_isPatched;
  uint16_t m_WireColor;
  TextBoxElement m_SourceName;
  TextBoxElement m_DestinationName;
  PortsDisplayElement m_SourceOutputs;
  PortsDisplayElement m_DestinationInputs;
};


class KeyboardElement : public UIElement
{
public:
  static constexpr int KEY_WIDTH = 26;
  static constexpr int SQUARE_RADIUS = 6;
  static constexpr int SPACING_PX = 7;

  static constexpr int TEXT_Y_OFFSET = -15;
  static constexpr int TEXT_X_OFFSET = 1;
  static constexpr size_t TEXT_SIZE = 2;

  static constexpr size_t NUM_KEYS = 26 + 10 + 4;
  static constexpr size_t NUM_ROWS = NUM_KEYS / 7;
  static constexpr size_t ZERO_KEY = 26;
  static constexpr size_t BACKSPACE_KEY = 37;
  static constexpr size_t ENTER_KEY = 38;
  static constexpr size_t ESCAPE_KEY = 39;

  static constexpr uint16_t KEY_COLOR = ILI9341_DARKCYAN;
  static constexpr uint16_t NUMBER_KEY_COLOR = ILI9341_LIGHTGREY;
  static constexpr uint16_t LIGHTER_KEY_COLOR = ILI9341_CYAN;
  static constexpr uint16_t BACKSPACE_KEY_COLOR = ILI9341_DARKGREY;
  static constexpr uint16_t ENTER_KEY_COLOR = ILI9341_GREEN;
  static constexpr uint16_t ESCAPE_KEY_COLOR = ILI9341_RED;

  static constexpr std::array<const char*, NUM_KEYS> KEY_LABELS = {
    "a", "b", "c", "d", "e", "f", "g", "h", 
    "i", "j", "k", "l", "m", "n", "o", "p", 
    "q", "r", "s", "t", "u", "v", "w", "x", 
    "y", "z", "0", "1", "2", "3", "4", "5", 
    "6", "7", "8", "9", "-", "del", ">", "X"
  };

public:
  KeyboardElement(std::string_view currentText, GFXcanvas16* canvas)
  : UIElement({}, 60, {}, {}, canvas), 
    m_CurrentText({}, {}, currentText.data(), ILI9341_LIGHTGREY, TEXT_SIZE, canvas)
  {
    m_Width = (KEY_WIDTH + SPACING_PX) * 8 - SPACING_PX;
    m_Height = (KEY_WIDTH + SPACING_PX) * NUM_ROWS + m_CurrentText.m_Height + TEXT_Y_OFFSET;
    UIElement::centerX();
    m_CurrentText.centerX();
    m_CurrentText.setY(m_Y - m_CurrentText.m_Height + TEXT_Y_OFFSET);
  }

  void draw() const override
  {
    clearCanvas();

    m_CurrentText.draw();

    for (size_t key{}; key < NUM_KEYS; key++)
    {
      uint16_t keyX = m_X + (key % 8) * (KEY_WIDTH + SPACING_PX);
      uint16_t keyY = m_Y + (key / 8) * (KEY_WIDTH + SPACING_PX);

      uint16_t color = (key % 2 == 0) ? LIGHTER_KEY_COLOR : KEY_COLOR;

      switch (key)
      {
        case BACKSPACE_KEY:
          color = BACKSPACE_KEY_COLOR; break;

        case ENTER_KEY:
          color = ENTER_KEY_COLOR; break;

        case ESCAPE_KEY:
          color = ESCAPE_KEY_COLOR; break;

        default:
          if (key >= ZERO_KEY && key < BACKSPACE_KEY) {
            color = NUMBER_KEY_COLOR;
          }
      }

      m_Canvas->fillRoundRect(keyX, keyY, KEY_WIDTH, KEY_WIDTH, SQUARE_RADIUS, color);

      TextElement keyLabel(0, 0, KEY_LABELS[key], ILI9341_BLACK, 1, m_Canvas);
      keyLabel.alignX(KEY_WIDTH, keyX);
      keyLabel.alignY(KEY_WIDTH, keyY);
      keyLabel.draw();
    }
  }

public:
  TextBoxElement m_CurrentText;
};


/*
  For displaying the files within a directory
*/
class DirectoryDisplayPage : public UIElement
{
public:
  static constexpr size_t TEXT_SIZE = 2;

  static constexpr uint16_t FILE_NAME_COLOR = ILI9341_YELLOW;
  static constexpr uint16_t FILE_INDEX_COLOR = ILI9341_LIGHTGREY;

  static constexpr int SPACING_PX = 20;
  static constexpr int TEXT_START_X = 20;
  static constexpr int TIP_TEXT_Y = 40;
  
public:
  DirectoryDisplayPage(const std::vector<std::string>& directoryContents, size_t directoryIndex, GFXcanvas16* canvas)
  : UIElement(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, canvas),
    m_FileName(directoryContents[directoryIndex].substr(0, directoryContents[directoryIndex].find('.'))),
    m_FileNameText({}, {}, m_FileName.c_str(), FILE_NAME_COLOR, TEXT_SIZE, canvas),
    m_FileIndexText({}, {}, std::to_string(directoryIndex + 1).data(), FILE_INDEX_COLOR, TEXT_SIZE, canvas)
  {
    m_FileNameText.centerX();
    m_FileNameText.centerY();

    m_FileIndexText.setX(TEXT_START_X);
    m_FileIndexText.centerY();
  }

  void draw() const override
  {
    clearCanvas();
    m_FileNameText.draw();
    m_FileIndexText.draw();

    m_Canvas->drawRect(
      m_FileIndexText.m_X - 5, 
      m_FileIndexText.m_Y - 5, 
      m_FileIndexText.m_Width + 10, 
      m_FileIndexText.m_Height + 10, 
      FILE_INDEX_COLOR);

    TextElement tip({}, TIP_TEXT_Y, "Press LOAD to load patch", ILI9341_DARKGREY, 1, m_Canvas);
    tip.centerX();
    tip.draw();

    TextElement tip2({}, SCREEN_HEIGHT - TIP_TEXT_Y - 10, "Scroll with knob 3", ILI9341_DARKGREY, 1, m_Canvas);
    tip2.centerX();
    tip2.setX(tip2.m_X - KnobElement::KNOB_WIDTH / 2);
    tip2.draw();

    KnobElement knob(tip2.m_X + tip2.m_Width + KnobElement::KNOB_WIDTH, tip2.m_Y + 10, 0.0f, "", m_Canvas);
    knob.draw();
  }

public:
  std::string m_FileName;
  TextElement m_FileNameText;
  TextElement m_FileIndexText;
};


class Dialog : public UIElement
{
public:
  static constexpr size_t TITLE_SIZE = 2;
  static constexpr size_t MESSAGE_SIZE = 1;
  static constexpr uint8_t SPACING_PX = 10;

public:
  Dialog(std::string_view title, std::string_view message, uint16_t color, GFXcanvas16* canvas)
  : UIElement(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, canvas),
    m_TitleText({}, {}, title.data(), color, (title.length() < 16) ? TITLE_SIZE : 1, canvas),
    m_MessageText({}, {}, message.data(), color, MESSAGE_SIZE, canvas)
  {
    m_TitleText.centerX();
    m_TitleText.centerY();
    m_TitleText.setY(m_TitleText.m_Y - (SPACING_PX + m_MessageText.m_Height) / 2);

    m_MessageText.centerX();
    m_MessageText.centerY();
    m_MessageText.setY(m_MessageText.m_Y + (SPACING_PX + m_TitleText.m_Height) / 2);
  }

  void draw() const override
  {
    clearCanvas();
    m_TitleText.draw();
    m_MessageText.draw();
  }

private:
  TextElement m_TitleText;
  TextElement m_MessageText;
};

/*
  For displaying an error to the user
*/
class ErrorDialog : public UIElement
{
public:
  ErrorDialog(std::string_view message, GFXcanvas16* canvas)
  : UIElement(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, canvas),
    m_Error("Error", message, ILI9341_RED, canvas)
  {}

  void draw() const override
  {
    clearCanvas();
    m_Error.draw();
  }

public:
  Dialog m_Error;
};

/*
  For displaying the splash screen on startup
*/
#include "sandbox_logo_bitmap.h"
class SplashScreen : public UIElement
{
public:
  SplashScreen(GFXcanvas16* canvas)
  : UIElement(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, canvas)
  {}

  void draw() const override
  {
    m_Canvas->drawRGBBitmap(0, 0, SANDBOX_LOGO_BITMAP.data(), SCREEN_WIDTH, SCREEN_HEIGHT);
  }
};