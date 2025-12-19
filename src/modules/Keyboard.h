#include "Module.h"
#pragma once
#include "Grid.h"

class KeyboardKey : public Module
{
public:
  static constexpr ModuleConfig::Type TYPE = ModuleConfig::Type::NONE;
  static constexpr const char* NAME = "Key";
public:
  KeyboardKey() 
  : Module(nullptr, NAME, 0, 0), 
    m_Amplitude(0), m_Pressed(false)
  {
    //Module::setPosition(row, col);
    m_Type = TYPE;
  }

  void setAmplitude(float amplitude)
  {
    amplitude = std::clamp(amplitude, 0.0f, 1.0f);
    m_Amplitude = amplitude;
  }

  void pressRisingEdge() override { m_Pressed = true; }

  void pressFallingEdge() override { m_Pressed = false; }

  bool isPressed() const { return m_Pressed; }

  float getAmplitude() const { return m_Amplitude; }

private:
  float m_Amplitude;
  bool m_Pressed; 
};

class Keyboard : public Module
{
public:
  static constexpr int MAX_SOURCES = 0;
  static constexpr int MAX_DESTINATIONS = 1;

  static constexpr ModuleConfig::Info INFO = {
    ModuleConfig::Type::NONE,
    "Keyboard",
    "8 note keyboard.",
    {}, {}, 
    {Port::CV_OUT_NAME, "trg"}
  };

public:
  Keyboard()
  : Module(new AudioSynthWaveformDc(), INFO.NAME, MAX_SOURCES, MAX_DESTINATIONS),
    m_Dc(static_cast<AudioSynthWaveformDc*>(m_Device)), m_NumKeys(8)
  {
    m_Type = INFO.TYPE;
    m_UIElement = ModuleUIElement(INFO);
    m_Dc->amplitude(0.0f);
  }

  ~Keyboard() 
  { 
    m_Dc = nullptr; 
    m_Keys.clear(); 
  }

  void update() override
  {
    for (auto& key : m_Keys)
    {
      if (key->isPressed())
      {
        m_Dc->amplitude(key->getAmplitude());
        return;
      }
    }
  }

private: 
  AudioSynthWaveformDc* m_Dc;
  std::vector<std::unique_ptr<KeyboardKey>> m_Keys;
  size_t m_NumKeys;

  static void createKeys(Keyboard& keyboard)
  {
    keyboard.m_Keys.clear();
    auto& ledElement = keyboard.m_UIElement.getLEDElement();
    for (size_t i{}; i < keyboard.m_NumKeys; i++)
    {
      int newRow = ledElement.row + (ledElement.col + i + 1) / 8;
      int newCol = (ledElement.col + i + 1) % 8;

      if (Grid::isModuleAt(newRow, newCol) || newRow > 6)
      {
        keyboard.m_NumKeys = i;
        break;
      }
      auto newKey = std::make_unique<KeyboardKey>(newRow, newCol);
      newKey->getLEDElement().setPosition(newRow, newCol);
      keyboard.m_Keys.push_back(std::move(newKey));
    }
  }
};