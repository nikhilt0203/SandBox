#pragma once
#include <Audio.h>
#include <memory>

class Module;
class Port;

class PortManager
{
public:
  PortManager(size_t numInputs, size_t numOutputs);

  bool inputsFull() const;
  bool outputsFull() const;

  bool isInputFree(int portNum) const;
  bool isOutputFree(int portNum) const;

  bool isInputConnected(int portNum) const { return !isInputFree(portNum); };
  bool isOutputConnected(int portNum) const { return !isOutputFree(portNum); };

  void disconnectAll();

  void setInputDevice(size_t modulePortNum, AudioStream* device, size_t devicePortNum = 0);
  void setOutputDevice(size_t modulePortNum, AudioStream* device, size_t devicePortNum = 0);

  void setInputNames(std::initializer_list<const char*> names);
  void setOutputNames(std::initializer_list<const char*> names);

  void setInputDevicePort(size_t devicePortNum, size_t modulePortNum);
  void setOutputDevicePort(size_t devicePortNum, size_t modulePortNum);

  void connectInput(Module* source, int portNum);
  void connectOutput(Module* destination, int portNum);
  
  void disconnectInput(int portNum);
  void disconnectOutput(int portNum);

  Port getOpenInputPort();
  Port getOpenOutputPort();

  size_t maxInputs() const { return m_MaxInputs; };
  size_t maxOutputs() const { return m_MaxOutputs; };

  Module* inputModuleAt(int portNum) const;
  Module* outputModuleAt(int portNum) const;

  const std::vector<Port>& getInputs() const { return m_InputPorts; };
  const std::vector<Port>& getOutputs() const { return m_OutputPorts; };

private:
  std::vector<Port> m_InputPorts;
  std::vector<Port> m_OutputPorts;
  const size_t m_MaxInputs;
  const size_t m_MaxOutputs;
};