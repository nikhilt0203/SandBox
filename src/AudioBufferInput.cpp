#include "AudioBufferInput.h"

static std::array<float, AudioBufferInput::BUFFER_SIZE> g_SharedBuffer = {};

void AudioBufferInput::update()
{
  audio_block_t* block = receiveReadOnly(0);
  if (!block) 
    return;

  for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
  {
    size_t nextIndex = m_BufferIndex + 1;
    if (nextIndex >= BUFFER_SIZE)
      break;

    float sampleNormalized = block->data[i] / 32767.0f;
    m_SampleBuffer[nextIndex % BUFFER_SIZE] = sampleNormalized;
    g_SharedBuffer[nextIndex % BUFFER_SIZE] = sampleNormalized;
    m_BufferIndex = nextIndex;
  }

  transmit(block, 0);
  release(block);
}

const std::array<float, AudioBufferInput::BUFFER_SIZE>& AudioBufferInput::flush()
{
  m_BufferIndex = 0;
  return m_SampleBuffer;
}

bool AudioBufferInput::available() const { return m_BufferIndex == BUFFER_SIZE - 1; }