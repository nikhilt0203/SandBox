#include <Audio.h>
#include <array>
#pragma once

class AudioBufferInput: public AudioStream
{
public:
  static constexpr size_t BUFFER_SIZE = 1024;

public:
  AudioBufferInput() : AudioStream(1, inputQueueArray), m_BufferIndex(0), m_SampleBuffer{} {}
  void update() override;
  const std::array<float, BUFFER_SIZE>& flush();
  bool available() const;
  
private:
  audio_block_t* inputQueueArray[1];
  size_t m_BufferIndex;
  std::array<float, BUFFER_SIZE> m_SampleBuffer;
  bool m_Paused = false;
};