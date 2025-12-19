#pragma once
#include <Audio.h>
#include <memory>

class Module;
class Port;

class PortManager
{
public:
  PortManager(int numInputs, int numOutputs);
  ~PortManager();

  bool inputsFull() const;
  bool outputsFull() const;
  bool isInputFree(int portNum) const;
  bool isOutputFree(int portNum) const;

  void disconnectAll();
  void setInputDevice(int portNum, AudioStream* device, int devicePortNum);
  void setOutputDevice(int portNum, AudioStream* device, int devicePortNum);
  void setInputNames(std::initializer_list<const char*> names);
  void setOutputNames(std::initializer_list<const char*> names);
  void setInputDevicePort(int portNum, int devicePortNum);
  void setOutputDevicePort(int portNum, int devicePortNum);
  void connectInput(Module* source, int portNum);
  void connectOutput(Module* destination, int portNum);
  void disconnectInput(int portNum);
  void disconnectOutput(int portNum);

  int getOpenInputPort() const;
  int getOpenOutputPort() const;
  int maxInputs() const;
  int maxOutputs() const;

  Module* inputModuleAt(int portNum) const;
  Module* outputModuleAt(int portNum) const;

  const std::vector<Port>& getInputs() const;
  const std::vector<Port>& getOutputs() const;

private:
  std::vector<Port> m_InputPorts, m_OutputPorts;
  const int m_MaxInputs, m_MaxOutputs;
};