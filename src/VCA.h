#include "Module.h"
#include "Port.h"
#pragma once

class VCA : public Module 
{ 
public:
  static constexpr int MAX_SOURCES = 2;
  static constexpr int MAX_DESTINATIONS = 1;

  static constexpr float MAX_MIX = 1.0f;
  static constexpr float DEFAULT_MIX = MAX_MIX / 2.0f;
  static constexpr float DEFAULT_GAIN = 1.0f;
  static constexpr float MAX_GAIN = 5.0f;

  static constexpr auto PARAMETER_NAMES = {"CV", "Gain"};
  static constexpr auto INPUT_PORT_NAMES = {Port::INPUT_NAME, Port::CV_IN_NAME};

  static constexpr ModuleInfo INFO = {
    Type::VCA,
    "VCA",
    "Voltage controlled amplifier",
    PARAMETER_NAMES,
    INPUT_PORT_NAMES, {}
  };

public:
  VCA(int row, int col) 
  : Module(new AudioEffectMultiply(), INFO.NAME, MAX_SOURCES, MAX_DESTINATIONS, row, col),
    m_Multiply(static_cast<AudioEffectMultiply*>(m_Device)), 
    m_Delay(new AudioEffectDelay()),
    m_Mixer(new AudioMixer4()),
    m_Amplifier(new AudioAmplifier()),
    m_DelaytoMixer(new AudioConnection(*m_Delay, 0, *m_Mixer, 0)),
    m_DelaytoMultiply(new AudioConnection(*m_Delay, 1, *m_Multiply, 0)),
    m_MultiplytoMixer(new AudioConnection(*m_Multiply, 0, *m_Mixer, 1)),
    m_MixertoAmplifier(new AudioConnection(*m_Mixer, 0, *m_Amplifier, 0))
  { 
    m_Type = INFO.TYPE;

    m_UIElement = ModuleUIElement(INFO.NAME, m_Color, INFO.PARAMETER_NAMES);
    m_Ports.setInputNames(INPUT_PORT_NAMES);
    setColor(m_Type);

    setMix(DEFAULT_MIX);
    setGain(DEFAULT_GAIN);
    m_Delay->delay(0, 0);
    m_Delay->delay(1, 0);
    for (int i = 2; i < 8; i++) 
      m_Delay->disable(i);
  } 

  ~VCA() 
  {
    delete m_DelaytoMixer;
    delete m_DelaytoMultiply;
    delete m_MultiplytoMixer;
    delete m_MixertoAmplifier;
  }

  void changeParameter(int parameterNum, int amt) override 
  { 
    switch (parameterNum) 
    { 
      case 0: setMix(m_Mix * powf(1.10f, amt)); break; 
      case 1: setGain(m_Gain * powf(1.10f, amt)); break;
    } 
  }

  void setMix(float mix) 
  { 
    if (mix < 0.0)
      return;

    if (mix > MAX_MIX)
      return;

    m_Mixer->gain(0, MAX_MIX - mix);
    m_Mix = mix; 
    m_UIElement.updateParameter(0, m_Mix, MAX_MIX); 
  }

  void setGain(float gain) 
  { 
    if (gain < 0.0f)
      return;

    if (gain > MAX_GAIN)
      return;

    m_Amplifier->gain(gain);
    m_Gain = gain; 
    m_UIElement.updateParameter(1, m_Gain, MAX_GAIN); 
  }

  //Format: VCA{mix,gain}[row,col]<ID>
  std::string toString() override 
  { 
    m_ID.makeSerializedID(INFO.NAME, m_Row, m_Col, 2);
    m_ID.addParam(m_Mix);
    m_ID.addParam(m_Gain);
    return m_ID.getSerializedID(); 
  }

  static Module* buildFromString(std::string s) 
  {
    int positionComma = s.find(",", s.find("["));

    float mix = Serialize::extractFloatBetween("{", ",", s);
    float gain = Serialize::extractFloatBetween(",", "}", s);
    int row = Serialize::extractIntBetween(s.find("["), positionComma, s);
    int col = Serialize::extractIntBetween(positionComma, s.find("]"), s);
    int ID =  Serialize::extractIntBetween("<", ">", s);

    VCA* vca = new VCA(row, col);
    vca->setMix(mix);
    vca->setGain(gain);
    vca->setID(ID);
    return vca;
  }

  int getOpenInputPort() const override 
  { 
    if (m_Ports.isInputFree(0)) 
      return 0; 
    if (m_Ports.isInputFree(1)) 
      return 1; 
    return -1; 
  }

  AudioStream* getOutputDevice() const override { return m_Amplifier; }

  AudioStream* getInputDevice() const override 
  { 
    if (m_Ports.isInputFree(0))
      return m_Delay;
    if (m_Ports.isInputFree(1))
      return m_Multiply;
    return nullptr;
  }

private:
  AudioEffectMultiply* m_Multiply;
  AudioEffectDelay* m_Delay;
  AudioMixer4* m_Mixer;
  AudioAmplifier* m_Amplifier;
  AudioConnection* m_DelaytoMixer;
  AudioConnection* m_DelaytoMultiply;
  AudioConnection* m_MultiplytoMixer;
  AudioConnection* m_MixertoAmplifier;
  float m_Mix;
  float m_Gain;
};