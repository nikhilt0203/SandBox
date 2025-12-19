#include <Audio.h>
#include <Arduino.h>
#include <AudioStream.h>
#pragma once

//Modulates the amplitude of the input signal based on a control signal input
class AudioEffectVCA: public AudioStream
{
public:
  AudioEffectVCA() 
  : AudioStream(2, m_InputQueueArray), 
    m_MultiplyCoefficient(0.5f), 
    m_OutputGain(1.0f)
  {}

  ~AudioEffectVCA() { SAFE_RELEASE_INPUTS(); }

  void setCVAmount(float coef) { m_MultiplyCoefficient = coef; }

  void setOutputGain(float gain) { m_OutputGain = gain; }

  void update() override
  {
    audio_block_t* inputBlock = receiveWritable(0);
    audio_block_t* cvBlock = receiveReadOnly(1);

    if (!inputBlock)
    {
      if (cvBlock) { 
        release(cvBlock); 
      }
      return;
    }

    if (!cvBlock) 
    {
      transmit(inputBlock, 0);
      release(inputBlock);
      return;
    }
   
    for (size_t i{}; i < AUDIO_BLOCK_SAMPLES; i++)
    {
      const float inputSample = inputBlock->data[i] / 32767.0f;
      const float cvSample = cvBlock->data[i] / 32767.0f;

      float outSample = inputSample * (1.0f - m_MultiplyCoefficient) + 
        (inputSample * cvSample * m_MultiplyCoefficient);

      inputBlock->data[i] = outSample * m_OutputGain * 32767.0f;
    }

    transmit(inputBlock, 0);
    release(inputBlock);
    release(cvBlock);
  }

private:
  audio_block_t* m_InputQueueArray[2];
  float m_MultiplyCoefficient;
  float m_OutputGain;
};