#include "Module.h"
#include "Port.h"
#pragma once

class Envelope : public Module
{
public:
  static constexpr int MAX_SOURCES = 2;
  static constexpr int MAX_DESTINATIONS = 1;
  
  static constexpr float DEFAULT_ATTACK = 10.0;
  static constexpr float DEFAULT_DECAY = 35.0;
  static constexpr float DEFAULT_SUSTAIN = 0.5;
  static constexpr float DEFAULT_RELEASE = 200.0;
  static constexpr float MAX_ATTACK = 500.0;
  static constexpr float MAX_DECAY = 500.0;
  static constexpr float MAX_SUSTAIN = 1.0;
  static constexpr float MAX_RELEASE = 500.0;

  static constexpr Type TYPE = Type::ENVELOPE;
  static constexpr const char* NAME = "Envelope";
  static constexpr auto PARAMETER_NAMES = {"A", "D", "S", "R"};
  static constexpr auto INPUT_PORT_NAMES = {Port::INPUT_NAME, Port::CLOCK_IN_NAME};

public:
  Envelope(int row, int col, float attack = DEFAULT_ATTACK, float decay = DEFAULT_DECAY, 
    float sustain = DEFAULT_SUSTAIN, float release = DEFAULT_RELEASE) 
  : Module(new AudioEffectEnvelope(), NAME, MAX_SOURCES, MAX_DESTINATIONS, row, col), 
  m_LastState(false)
  {
    m_Type = TYPE;

    m_Envelope = static_cast<AudioEffectEnvelope*>(m_Device);
    m_Envelope->noteOff();
    m_Ports.setInputNames(INPUT_PORT_NAMES);

    m_Trig = new AudioTriggerInput();
    m_Trig->setRisingEdgeCallback([this]() { this->m_Envelope->noteOn(); });
    m_Trig->setFallingEdgeCallback([this]() { this->m_Envelope->noteOff(); });

    setColor(m_Type);
    m_UIElement = ModuleUIElement("Envelope", m_Color, PARAMETER_NAMES);

    setAttack(attack);
    setDecay(decay);
    setSustain(sustain);
    setRelease(release);
  }

  ~Envelope() 
  {
    m_Envelope->noteOff();
    m_Envelope = nullptr;
    m_Trig = nullptr;
  }

  void changeParameter(int parameterNum, int amt) override
  { 
    switch (parameterNum)
    {
      case 0: setAttack(m_Attack + (amt * 5)); break;
      case 1: setDecay(m_Decay + (amt * 5)); break;
      case 2: setSustain(m_Sustain + (amt / 50.0f)); break;
      case 3: setRelease(m_Release + (amt * 5)); break;
    }
  }

  void setAttack(float ms)
  {
    if (ms < 0.0 || ms > MAX_ATTACK) 
      return;

    m_Attack = ms;
    m_Envelope->attack(m_Attack);
    m_UIElement.updateParameter(0, m_Attack, MAX_ATTACK);
  }

  void setDecay(float ms)
  {
    if (ms < 0.0 || ms > MAX_DECAY) 
      return;

    m_Decay = ms;
    m_Envelope->decay(m_Decay);
    m_UIElement.updateParameter(1, m_Decay, MAX_DECAY);
  }

  void setSustain(float level)
  {
    if (level < 0.0 || level > MAX_SUSTAIN) 
      return;

    m_Sustain = level;
    m_Envelope->sustain(m_Sustain);
    m_UIElement.updateParameter(2, m_Sustain, MAX_SUSTAIN);
  }

  void setRelease(float ms)
  {
    if (ms < 0.0 || ms > MAX_RELEASE) 
      return;

    m_Release = ms;
    m_Envelope->release(m_Release);
    m_UIElement.updateParameter(3, m_Release, MAX_RELEASE);
  }

  AudioStream* getOutputDevice() const override { return m_Envelope; }

  AudioStream* getInputDevice() const override
  {
    if (m_Ports.isInputFree(0)) 
      return m_Envelope;  
    if (m_Ports.isInputFree(1)) 
      return m_Trig;

    return nullptr;
  }

  int getOpenInputPort() const override
  {
    if (!(m_Ports.isInputFree(0)) && !(m_Ports.isInputFree(1))) 
      return -1;
    return 0;
  }

  void pressRisingEdge() override { m_Envelope->noteOn(); m_DisplayManager.displayMe(this); }

  void pressFallingEdge() override { m_Envelope->noteOff(); }

  std::string toString() override
  {
    m_ID.makeSerializedID(NAME, m_Row, m_Col, 4);
    m_ID.addParam(m_Attack);
    m_ID.addParam(m_Decay);
    m_ID.addParam(m_Sustain);
    m_ID.addParam(m_Release);
    return m_ID.getSerializedID();
  }

  static Module* buildFromString(std::string s) 
  {
    //Format: Envelope{attack,decay,sustain,release}[row,col]<ID>
    int firstComma = s.find(",");
    int secondComma = s.find(",", firstComma + 1);
    int thirdComma = s.find(",", secondComma + 1);
    int positionComma = s.find(",", s.find("["));

    float attack =  Serialize::extractFloatBetween(s.find("{"), firstComma, s);
    float decay =   Serialize::extractFloatBetween(firstComma, secondComma, s);
    float sustain = Serialize::extractFloatBetween(secondComma, thirdComma, s);
    float release = Serialize::extractFloatBetween(thirdComma, s.find("}"), s);
    int row =       Serialize::extractIntBetween(s.find("["), positionComma, s);
    int col =       Serialize::extractIntBetween(positionComma, s.find("]"), s);
    int ID =        Serialize::extractIntBetween("<", ">", s);

    Envelope* env = new Envelope(row, col, attack, decay, sustain, release);
    env->setID(ID);

    return env;
  }

private:
  AudioEffectEnvelope* m_Envelope;
  AudioTriggerInput* m_Trig;
  float m_Attack, m_Decay, m_Sustain, m_Release;
  bool m_LastState;
};