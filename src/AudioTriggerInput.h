#include <Arduino.h>
#include <AudioStream.h>

class AudioTriggerInput : public AudioStream
{
public:
  static constexpr float TRIGGER_THRESHOLD = 0.25f;
private:
  audio_block_t* m_InputQueueArray[1];
  float m_LastSample;
  bool m_RisingEdge;
  bool m_FallingEdge;
  std::function<void()> m_RisingEdgeCallback;
  std::function<void()> m_FallingEdgeCallback;
public:
  AudioTriggerInput() 
  : AudioStream(1, m_InputQueueArray), m_LastSample(0.0f), m_RisingEdge(false), m_FallingEdge(false)
  {}

  void setRisingEdgeCallback(std::function<void()> f) { m_RisingEdgeCallback = f; }

  void setFallingEdgeCallback(std::function<void()> f) { m_FallingEdgeCallback = f; }

  void update() override
  {
    audio_block_t* block = receiveReadOnly(0);
    if (!block) 
    {
      m_RisingEdge = false;
      m_FallingEdge = false;
      return;
    }

    m_RisingEdge = false;
    m_FallingEdge = false;

    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
    {
      float sample = block->data[i] / 32767.0f;

      if (!m_RisingEdge && m_LastSample < TRIGGER_THRESHOLD 
        && sample >= TRIGGER_THRESHOLD)
      {
        m_RisingEdge = true;
        if (m_RisingEdgeCallback) 
          m_RisingEdgeCallback();
      }

      if (!m_FallingEdge && m_LastSample > TRIGGER_THRESHOLD 
          && sample < TRIGGER_THRESHOLD)
      {
        m_FallingEdge = true;
        if (m_FallingEdgeCallback) 
          m_FallingEdgeCallback();
      }

      m_LastSample = sample;
    }
    release(block);
  }
};