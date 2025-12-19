#include "Module.h"
#include "Port.h"
#pragma once

class Envelope : public Module
{
public:
  MODULE_INFO(
    ENVELOPE,
    "Envelope",
    "Applies an ADSR to the input signal     Triggered via press or clock input",
    MODULE_PARAMS("A", "D", "S", "R"),
    MODULE_INPUTS(Port::INPUT_NAME, Port::CLOCK_IN_NAME), 
    MODULE_OUTPUTS(Port::OUTPUT_NAME, "trg")
  );

  static constexpr size_t MAX_SOURCES = 2;
  static constexpr size_t MAX_DESTINATIONS = 2;
  
  static constexpr float DEFAULT_ATTACK = 10.0f;
  static constexpr float MAX_ATTACK = 500.0f;

  static constexpr float DEFAULT_DECAY = 35.0f;
  static constexpr float MAX_DECAY = 500.0f;

  static constexpr float DEFAULT_SUSTAIN = 0.0f;
  static constexpr float MAX_SUSTAIN = 1.0f;

  static constexpr float DEFAULT_RELEASE = 200.0f;
  static constexpr float MAX_RELEASE = 500.0f;

  
public:
  Envelope(float attack = DEFAULT_ATTACK, float decay = DEFAULT_DECAY, 
    float sustain = DEFAULT_SUSTAIN, float release = DEFAULT_RELEASE) 
  : Module(new AudioEffectEnvelope(), INFO.NAME, MAX_SOURCES, MAX_DESTINATIONS), 
    m_LastState(false)
  {
    m_Type = INFO.TYPE;

    m_Envelope = static_cast<AudioEffectEnvelope*>(m_Device);
    m_Envelope->noteOff();

    m_Trig = new AudioTriggerInput();
    m_Trig->setRisingEdgeCallback([this]() { this->noteOn(); });
    m_Trig->setFallingEdgeCallback([this]() { this->noteOff(); });

    m_TrigOut = new AudioTriggerOutput();

    m_UIElement = ModuleUIElement(INFO);

    m_Ports.setInputNames(INFO.INPUT_PORT_NAMES);
    m_Ports.setOutputNames(INFO.OUTPUT_PORT_NAMES);
    m_Ports.setInputDevicePort(0, 1);
    m_Ports.setOutputDevicePort(0, 1);

    setAttack(attack);
    setDecay(decay);
    setSustain(sustain);
    setRelease(release);
  }

  ~Envelope() 
  {
    delete m_Trig;
    delete m_TrigOut;
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
    ms = std::clamp(ms, 0.0f, MAX_ATTACK);
    m_Attack = ms;
    m_Envelope->attack(m_Attack);
    m_UIElement.updateParameter(0, m_Attack, MAX_ATTACK);
  }

  void setDecay(float ms)
  {
    ms = std::clamp(ms, 0.0f, MAX_DECAY);
    m_Decay = ms;
    m_Envelope->decay(m_Decay);
    m_UIElement.updateParameter(1, m_Decay, MAX_DECAY);
  }

  void setSustain(float level)
  {
    level = std::clamp(level, 0.0f, MAX_SUSTAIN);
    m_Sustain = level;
    m_Envelope->sustain(m_Sustain);
    m_UIElement.updateParameter(2, m_Sustain, MAX_SUSTAIN);
  }

  void setRelease(float ms)
  {
    ms = std::clamp(ms, 0.0f, MAX_RELEASE);
    m_Release = ms;
    m_Envelope->release(m_Release);
    m_UIElement.updateParameter(3, m_Release, MAX_RELEASE);
  }

  void noteOn()
  {
    m_Envelope->noteOn();
    m_TrigOut->on();
    m_UIElement.updateColor(Colors::blend(m_UIElement.getColor(), 255, 0.5));
  }

  void noteOff()
  {
    m_Envelope->noteOff();
    m_TrigOut->off();
    m_UIElement.updateColor(Colors::getColor(INFO.TYPE));
  }

  AudioStream* getOutputDevice() const override 
  { 
    if (m_Ports.isOutputFree(0)) 
    { 
      return m_Envelope; 
    }
    if (m_Ports.isOutputFree(1)) 
    { 
      return m_TrigOut; 
    }
    return nullptr; 
  }

  AudioStream* getInputDevice() const override
  {
    if (m_Ports.isInputFree(0)) 
    { 
      return m_Envelope; 
    }
    if (m_Ports.isInputFree(1)) 
    { 
      return m_Trig; 
    }
    return nullptr;
  }

  void pressRisingEdge() override 
  { 
    noteOn();
  }

  void pressFallingEdge() override 
  { 
    noteOff();
  }

  std::string_view serialize() override
  {
    m_ID.makeSerializationID(m_UIElement);
    m_ID.addParam(m_Attack);
    m_ID.addParam(m_Decay);
    m_ID.addParam(m_Sustain);
    m_ID.addParam(m_Release);
    return m_ID.getSerializationID();
  }

  static Module* buildFromString(std::string_view s) 
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

    Envelope* env = new Envelope(attack, decay, sustain, release);
    env->setID(ID);
    env->getLEDElement().setPosition(row, col);
    return env;
  }

private:
  AudioEffectEnvelope* m_Envelope;
  AudioTriggerInput* m_Trig;
  AudioTriggerOutput* m_TrigOut;
  float m_Attack, m_Decay, m_Sustain, m_Release;
  bool m_LastState;
};