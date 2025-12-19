#pragma once
#include <Arduino.h>
#include <AudioStream.h>

class AudioTriggerInput : public AudioStream
{
public:
  AudioTriggerInput() 
  : AudioStream(1, m_InputQueueArray), 
    m_LastSample(0.0f), 
    m_RisingEdge(false), 
    m_FallingEdge(false),
    m_Threshold(DEFAULT_TRIGGER_THRESHOLD)
  {}

  ~AudioTriggerInput() { SAFE_RELEASE_INPUTS(); }

  void threshold(float value) { m_Threshold = std::clamp(value, 0.0f, 1.0f); }

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

    for (size_t i{}; i < AUDIO_BLOCK_SAMPLES; i++)
    {
      const float sample = block->data[i] / 32767.0f;

      if (sample >= m_Threshold && m_LastSample < m_Threshold && !m_RisingEdge)
      {
        m_RisingEdge = true;
        if (m_RisingEdgeCallback) { m_RisingEdgeCallback(); }
      }

      if (sample < m_Threshold && m_LastSample > m_Threshold && !m_FallingEdge)
      {
        m_FallingEdge = true;
        if (m_FallingEdgeCallback) { m_FallingEdgeCallback(); }
      }

      m_LastSample = sample;
    }

    release(block);
  }

public:
  static constexpr float DEFAULT_TRIGGER_THRESHOLD = 0.25f;

private:
  audio_block_t* m_InputQueueArray[1];
  volatile float m_LastSample;
  volatile bool m_RisingEdge;
  volatile bool m_FallingEdge;
  float m_Threshold;
  std::function<void()> m_RisingEdgeCallback;
  std::function<void()> m_FallingEdgeCallback;
};