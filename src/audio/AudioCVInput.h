#include <Arduino.h>
#include <AudioStream.h>
#pragma once

class AudioCVInput: public AudioStream
{
public:
  AudioCVInput(std::function<void(float)> f, float minValue, float maxValue) 
  : AudioStream(1, inputQueueArray), 
    parameterCallback(f),
    m_MinValue(minValue),
    m_MaxValue(maxValue) 
  {}

  ~AudioCVInput() { SAFE_RELEASE_INPUTS(); }

  void update() override
  {
    audio_block_t* block = receiveReadOnly(0);
    if (!block) { return; }

    for (size_t i{}; i < AUDIO_BLOCK_SAMPLES; i++)
    {
      if (parameterCallback)
      {
        const float mappedValue = map(block->data[i] / 32767.0f, -1.0f, 1.0f, m_MinValue, m_MaxValue);
        parameterCallback(mappedValue);
      }
    }

    if (block) { release(block); }
  }

private:
  audio_block_t* inputQueueArray[1];
  std::function<void(float)> parameterCallback;
  float m_MinValue;
  float m_MaxValue;
  float m_Value;

};