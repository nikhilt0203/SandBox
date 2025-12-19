#pragma once
#include "Adafruit_ILI9341.h"
#include "AudioBufferInput.h"
#include "Module.h"
#include "Port.h"
#include "Colors.h"
#include <string>

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

namespace Align 
{
  //static uint16_t alignX(uint16_t x, uint16_t refWidth, uint16_t refX) { return refX + (refWidth - x) / 2; }
  //static uint16_t alignY(uint16_t y, uint16_t refHeight, uint16_t refY) { return refY + (refHeight - y) / 2; }
}

namespace Rectangle 
{
  static uint16_t centerToUpperLeftX(uint16_t x, uint16_t width) { return x - width / 2; }
  static uint16_t centerToUpperLeftY(uint16_t y, uint16_t height) { return y - height / 2; }
  //static uint16_t upperLeftToCenterX(uint16_t x, uint16_t width) { return x + width / 2; }
  //static uint16_t upperLeftToCenterY(uint16_t y, uint16_t height) { return y + height / 2; }
}

/*
  Base class for all UI elements
  **UI element width and height must be known at construction time**
*/
class UIElement
{
public:
  uint16_t m_X;
  uint16_t m_Y;
  uint16_t m_Width;
  uint16_t m_Height;

  virtual void alignX(uint16_t refWidth, uint16_t refX) { m_X = refX + (refWidth - m_Width) / 2; }
  virtual void alignY(uint16_t refHeight, uint16_t refY) { m_Y = refY + (refHeight - m_Height) / 2; }
  virtual void centerX(uint16_t refWidth, uint16_t refX) { m_X = (refWidth - SCREEN_WIDTH) / 2; }
  virtual void centerY(uint16_t refHeight, uint16_t refY) { m_Y = (refHeight - SCREEN_HEIGHT) / 2; }
  virtual void render() = 0;

protected:
  UIElement(uint16_t x, uint16_t y, uint16_t w, uint16_t h, GFXcanvas16* canvas) 
  : m_X(x), m_Y(y), m_Width(w), m_Height(h), m_Canvas(canvas) {}

  UIElement() : UIElement(0, 0, 0, 0, nullptr) {}

  virtual ~UIElement() = default;

  void* operator new(size_t) = delete;

protected:
  GFXcanvas16* m_Canvas;
};

/*
  For arbitary text display
*/
class TextElement : public UIElement
{
public:
  TextElement(uint16_t x, uint16_t y, const char* text, uint16_t color, uint8_t size, GFXcanvas16* canvas)
  : UIElement(x, y, 0, 0, canvas),
    m_Text(text),
    m_Color(color),
    m_TextSize(size)
  {
    if (!m_Canvas || !m_Text) return;

    int16_t textX, textY;
    uint16_t textWidth, textHeight;
    m_Canvas->setTextSize(m_TextSize);
    m_Canvas->getTextBounds(m_Text, 0, 0, &textX, &textY, &textWidth, &textHeight);

    m_Width = textWidth;
    m_Height = textHeight;
    m_OffsetX = textX * -1;
    m_OffsetY = textY * -1;
  }

  void render() override
  {
    if (!m_Canvas || !m_Text) return;

    m_Canvas->setTextSize(m_TextSize);
    m_Canvas->setTextColor(m_Color);
    m_Canvas->setCursor(m_X + m_OffsetX, m_Y + m_OffsetY);
    m_Canvas->print(m_Text);
  }

private:
  const char* m_Text;
  uint16_t m_Color;
  uint8_t m_TextSize;
  int m_OffsetX = 0;
  int m_OffsetY = 0;
};

/*
  Knob with label and variable turn amount
*/
class KnobElement : public UIElement
{
public:
  static constexpr int MIN_ROTATION = 120;
  static constexpr int MAX_ROTATION = 420;
  static constexpr int KNOB_RADIUS = 25;
  static constexpr int INDICATOR_RADIUS = 6;
  static constexpr float INDICATOR_SCALE = 0.9f;
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
  : UIElement(x, y, KNOB_RADIUS * 2 + 1, KNOB_RADIUS * 2 + 1, canvas), 
    m_PercentTurned(percentTurned), m_Label(label) 
  {
    if (m_PercentTurned < 0.0) m_PercentTurned = 0.0;
    if (m_PercentTurned > 1.0) m_PercentTurned = 1.0;
  }

  void render() override
  {
    if (!m_Canvas)
      return;

    int degreesTurned = MIN_ROTATION + (m_PercentTurned * (MAX_ROTATION - MIN_ROTATION));

    //Outer circle
    m_Canvas->drawCircle(m_X, m_Y, KNOB_RADIUS, OUTER_CIRCLE_COLOR);
    //Inner circle
    m_Canvas->drawCircle(m_X, m_Y, 10, INNER_CIRCLE_COLOR);

    //Calculate where indicator should be given how much the knob is turned
    int indicatorX = m_X + std::cos(degreesTurned % 360 * (M_PI / 180.0f)) * (KNOB_RADIUS - INDICATOR_RADIUS) * INDICATOR_SCALE;
    int indicatorY = m_Y + std::sin(degreesTurned % 360 * (M_PI / 180.0f)) * (KNOB_RADIUS - INDICATOR_RADIUS) * INDICATOR_SCALE;

    m_Canvas->fillCircle(indicatorX, indicatorY, INDICATOR_RADIUS, INDICATOR_COLOR);

    if (m_Label == nullptr)
      return;

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
  static constexpr int SQUARE_RADIUS = 5;
  static constexpr int SPACING_PX = 33;
  static constexpr int TEXT_Y_OFFSET = 0;
  static constexpr int TEXT_X_OFFSET = 1;
public:
  PortsDisplayElement(uint16_t x, uint16_t y, const std::vector<Port>& ports, GFXcanvas16* canvas)
  : UIElement(x, y, (SQUARE_WIDTH + SPACING_PX) * ports.size() - SPACING_PX, SQUARE_WIDTH, canvas), 
    m_Ports(&ports)
  {}

  void render() override
  {
    if (!m_Ports || !m_Canvas) 
      return;

    size_t numConnections = m_Ports->size();
    if (numConnections == 0) 
      return;

    uint32_t colors[numConnections];

    int index = 0;
    for (const Port& port: *m_Ports)
    {
      if (!port.m_ConnectedModule)  
      {
        colors[index] = 0; 
        index++;
        continue;
      }
      colors[index] = port.m_ConnectedModule->getColor();
      index++;
    }

    for (size_t i = 0; i < numConnections; i++)
    {
      int x = m_X + i * SPACING_PX;
      uint16_t textColor;

      if (colors[i] != 0)
      {
        m_Canvas->fillRoundRect(x, m_Y, SQUARE_WIDTH, SQUARE_WIDTH, SQUARE_RADIUS, Colors::to565(colors[i]));
        textColor = ILI9341_BLACK;
      }
      else
      {
        //empty port
        m_Canvas->drawRoundRect(x, m_Y, SQUARE_WIDTH, SQUARE_WIDTH, SQUARE_RADIUS - 1, ILI9341_DARKGREY);
        textColor = ILI9341_DARKGREY;
      }

      TextElement portLabel(0, 0, m_Ports->at(i).m_Name, textColor, 1, m_Canvas);
      portLabel.alignX(SQUARE_WIDTH, x - TEXT_X_OFFSET);
      portLabel.alignY(SQUARE_WIDTH, m_Y);
      portLabel.render();
    }
  }
private:
  const std::vector<Port>* m_Ports;
};

class LockIconElement : public UIElement
{
public:

  LockIconElement(uint16_t x, uint16_t y, bool locked, GFXcanvas16* canvas)
  : UIElement(x, y, BODY_WIDTH, BODY_HEIGHT + LATCH_RADIUS + Y_OFFSET, canvas), 
    m_Color(ILI9341_DARKGREY), m_Locked(locked)
  {}

  void render() override
  {
    if (!m_Canvas)
      return;

    m_Canvas->fillRoundRect(Rectangle::centerToUpperLeftX(m_X, m_Width), Rectangle::centerToUpperLeftY(m_Y, m_Height), BODY_WIDTH, BODY_HEIGHT, 3, m_Color);
    m_Canvas->drawCircle(m_X, Rectangle::centerToUpperLeftY(m_Y, m_Height) - Y_OFFSET, LATCH_RADIUS, m_Color);
    m_Canvas->drawCircle(m_X, Rectangle::centerToUpperLeftY(m_Y, m_Height) - Y_OFFSET, LATCH_RADIUS + 1, m_Color); //thicken line

    if (!m_Locked)
      m_Canvas->fillRect(Rectangle::centerToUpperLeftX(m_X, m_Width) + 2, Rectangle::centerToUpperLeftY(m_Y, m_Height) - 3, 4, 3, ILI9341_BLACK);
  }

public:
  static constexpr uint16_t BODY_WIDTH = 13;
  static constexpr uint16_t BODY_HEIGHT = 13;
  static constexpr uint16_t LATCH_RADIUS = 3;
  static constexpr uint16_t Y_OFFSET = 2;

private:
  uint16_t m_Color;
  bool m_Locked;
};

/*
  Colored text box.
*/
class TextBoxElement : public UIElement
{
public:
  static constexpr int WIDTH_PADDING = 6;
  static constexpr int MAX_BOX_HEIGHT = 39;
  static constexpr int BOX_X_OFFSET = 5;
  static constexpr int BOX_Y_OFFSET = 3;
public:
  TextBoxElement(uint16_t x, uint16_t y, const char* text, uint16_t color, uint8_t size, GFXcanvas16* canvas)
  : UIElement(x, y, 0, 0, canvas),
    m_TextElement(x, y, text, color, size, canvas), m_Color(color)
  {
    m_Width = m_TextElement.m_Width + WIDTH_PADDING;
    m_Height = std::min((int)m_TextElement.m_Height, MAX_BOX_HEIGHT);
  }

  void render() override
  {
    m_TextElement.render();
    m_Canvas->drawRoundRect(m_X, m_Y, m_Width, m_Height, 5, m_Color);
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

public:
  TextElement m_TextElement;
  uint16_t m_Color;
};

class ModuleDisplayPage : public UIElement
{
public:
  static constexpr int TEXT_SIZE = 3;
  static constexpr int TEXT_BOX_Y = 45;
  static constexpr int INPUTS_X = 20;
  static constexpr int INPUTS_Y = 8;
  static constexpr int OUTPUTS_X = 20; 
  static constexpr int OUTPUTS_Y = 94;
  static constexpr int KNOBS_Y = 160;
  static constexpr int EMPTY_KNOB_RADIUS = KnobElement::KNOB_RADIUS - 3;
  static constexpr int KNOB_SPACING_PX = (SCREEN_WIDTH - (KnobElement::KNOB_RADIUS * 2 * 4)) / 5;

public:
  ModuleDisplayPage(Module* m, GFXcanvas16* canvas)
  : UIElement(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, canvas),
    m_TextBox(SCREEN_WIDTH / 2, TEXT_BOX_Y, m->getName().c_str(), Colors::to565(m->getColor()), TEXT_SIZE, canvas),
    m_Inputs(INPUTS_X, INPUTS_Y, m->getInputs(), canvas),
    m_Outputs(OUTPUTS_X, OUTPUTS_Y, m->getOutputs(), canvas),
    m_Module(m)
  {
    m_TextBox.alignX(SCREEN_WIDTH, 0);
  }

  void render() override
  {
    if (!m_Canvas || !m_Module)
      return;

    m_TextBox.render();
    m_Inputs.render();
    m_Outputs.render();

    auto& parameters = m_Module->getUIElement().getDisplayElement().parameters;
    size_t numParameters = parameters.size();
    
    uint16_t startX = KnobElement::KNOB_RADIUS + KNOB_SPACING_PX;

    for (size_t i = 0; i < 4; i++)
    {
      uint16_t x = startX + (KnobElement::KNOB_RADIUS * 2 * i) + (KNOB_SPACING_PX * i);

      if (i < numParameters)
      {
        KnobElement knob(x, KNOBS_Y, parameters[i].percentage, parameters[i].name, m_Canvas);
        knob.render();
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
  PortsDisplayElement m_Inputs, m_Outputs;
  Module* m_Module;
};

/*
  For displaying bank information
*/
class BankDisplayPage : public UIElement
{
public:
  BankDisplayPage(const char* label, uint16_t color, GFXcanvas16* canvas)
  : UIElement(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, canvas), 
    m_ModuleNameTextBox(m_X, m_Y, label, color, TEXT_SIZE, canvas),
    m_Description(m_X, DESCRIPTION_Y, "Description", ILI9341_LIGHTGREY, DESCRIPTION_SIZE, canvas)
  {}

  void render() override 
  { 
    m_ModuleNameTextBox.alignX(m_Width, m_X);
    m_ModuleNameTextBox.alignY(m_Height, m_Y);
    m_Description.alignX(m_Width, m_X);
    m_ModuleNameTextBox.render(); 
    m_Description.render();
  }

public:
  static constexpr int TEXT_SIZE = 3;
  static constexpr int DESCRIPTION_SIZE = 1;
  static constexpr int DESCRIPTION_Y = 180;

private:
  TextBoxElement m_ModuleNameTextBox;
  TextElement m_Description;
};

class WaveformDisplayFrame : public UIElement
{
public:
  static constexpr float WAVEFORM_SCALE = 0.7f;
  static constexpr float BORDER_SCALE = 0.85f;

  static constexpr int WINDOW_X = SCREEN_WIDTH * (1 - BORDER_SCALE) / 2;
  static constexpr int WINDOW_Y = SCREEN_HEIGHT * (1 - BORDER_SCALE) / 2 + 7;
  static constexpr int WINDOW_WIDTH = SCREEN_WIDTH * BORDER_SCALE;
  static constexpr int WINDOW_HEIGHT = SCREEN_HEIGHT * BORDER_SCALE;
  static constexpr int LOCK_X = 18;
  static constexpr int LOCK_Y = 18;
  static constexpr int TITLE_Y = 6;

  static constexpr uint16_t WAVEFORM_COLOR = ILI9341_GREEN;
  static constexpr uint16_t WAVEFORM_CLIP_COLOR = ILI9341_RED;
  static constexpr uint16_t ZERO_LINE_COLOR = ILI9341_DARKGREY;
  static constexpr uint16_t BORDER_COLOR = ILI9341_DARKCYAN;
  static constexpr uint16_t BACKGROUND_COLOR = ILI9341_BLACK;

  static constexpr size_t RESOLUTION = 8;
  static constexpr size_t NUM_SAMPLES = AudioBufferInput::BUFFER_SIZE;

public:
  WaveformDisplayFrame(const std::array<float, AudioBufferInput::BUFFER_SIZE>& samples, GFXcanvas16* canvas)
  : UIElement(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, canvas), 
    m_LockIcon(LOCK_X, LOCK_Y, false, canvas),
    m_SampleArray(&samples)
  {}

  void render() override
  {
    if (!m_Canvas)
      return;

    m_Canvas->fillScreen(BACKGROUND_COLOR);
    m_Canvas->drawRoundRect(WINDOW_X, WINDOW_Y, WINDOW_WIDTH, WINDOW_HEIGHT, 20, BORDER_COLOR);
    m_Canvas->drawFastHLine(WINDOW_X, WINDOW_Y + WINDOW_HEIGHT / 2, WINDOW_WIDTH, ZERO_LINE_COLOR);
    m_LockIcon.render();

    TextElement title(0, TITLE_Y, "Oscilloscope", ILI9341_CYAN, 1, m_Canvas);
    title.alignX(WINDOW_WIDTH, WINDOW_X);
    title.render();

    size_t numSamples = m_SampleArray->size();
    uint16_t color;
    int prevX = 0;
    int prevY = 0;

    for (size_t i = 0; i < numSamples; i += RESOLUTION)
    {
      float sampleValue = m_SampleArray->at(i);
      float percentDrawn = static_cast<float>(i) / (numSamples - 1);
      int x = (WINDOW_X + 1) + static_cast<int>(percentDrawn * (WINDOW_WIDTH - 1));
      int y = (WINDOW_HEIGHT / 2) - (WINDOW_HEIGHT / 2) * (sampleValue * WAVEFORM_SCALE) + WINDOW_Y;

      if (i == 0)
      {
        prevX = x;
        prevY = y;
        continue;
      }

      if (prevX == x && prevY == y)
        continue;

      if (sampleValue >= 1.0f || sampleValue <= -1.0f)
        color = WAVEFORM_CLIP_COLOR;
      else
        color = WAVEFORM_COLOR;

      m_Canvas->drawLine(prevX, prevY, x, y, color);
      prevX = x;
      prevY = y;
    }
  }

private:
  LockIconElement m_LockIcon;
  const std::array<float, NUM_SAMPLES>* m_SampleArray;
};
