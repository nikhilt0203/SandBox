#include <Audio.h>
#include <array>
#pragma once

#define SAMPLE_BUFFER_SIZE 1024

using SampleBuffer = std::array<float, SAMPLE_BUFFER_SIZE>;

class AudioBufferInput: public AudioStream
{
public:
  AudioBufferInput() : AudioStream(1, inputQueueArray), m_BufferIndex{}, m_SampleBuffer{} {}
  
  ~AudioBufferInput() { SAFE_RELEASE_INPUTS(); }

  void update() override
  {
    audio_block_t* block = receiveReadOnly(0);
    if (!block) { 
      return; 
    }

    for (size_t i{}; i < AUDIO_BLOCK_SAMPLES; i++)
    {
      size_t nextIndex = m_BufferIndex + 1;
      
      if (nextIndex >= SAMPLE_BUFFER_SIZE) { 
        break; 
      }

      float sampleNormalized = block->data[i] / 32767.0f;
      m_SampleBuffer[nextIndex % SAMPLE_BUFFER_SIZE] = sampleNormalized;
      m_BufferIndex = nextIndex;
    }

    if (!block) { 
      return; 
    }

    transmit(block, 0);
    release(block);
  }

  const SampleBuffer& flush() 
  {
    m_BufferIndex = 0;
    return m_SampleBuffer;
  }

  bool available() const { return m_BufferIndex == SAMPLE_BUFFER_SIZE - 1; }
  
private:
  audio_block_t* inputQueueArray[1];
  volatile size_t m_BufferIndex;
  SampleBuffer m_SampleBuffer;
};