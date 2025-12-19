#include "PortManager.h"
#include "Module.h"
#include "Port.h"

PortManager::PortManager(size_t numInputs, size_t numOutputs) 
  : m_MaxInputs(numInputs), m_MaxOutputs(numOutputs)
{
  m_InputPorts.reserve(m_MaxInputs);
  m_OutputPorts.reserve(m_MaxOutputs);
  
  for (size_t i{}; i < m_MaxInputs; i++)
  {
    m_InputPorts.emplace_back(Port::INPUT_NAME, i, i);
  }
  for (size_t i{}; i < m_MaxOutputs; i++)
  {
    m_OutputPorts.emplace_back(Port::OUTPUT_NAME, i, i);
  }
}

void PortManager::setInputNames(std::initializer_list<const char*> names)
{
  size_t index = 0;
  for (Port& port : m_InputPorts)
  {
    if (index < names.size()) 
    {
      port.m_Name = names.begin()[index];
    }
    index++;
  }
}

void PortManager::setOutputNames(std::initializer_list<const char*> names)
{
  size_t index = 0;

  for (Port& port : m_OutputPorts)
  {
    if (index < names.size()) 
    {
      port.m_Name = names.begin()[index];
    }
    index++;
  }
}

void PortManager::setInputDevicePort(size_t devicePortNum, size_t modulePortNum)
{
  if (modulePortNum >= m_InputPorts.size()) 
  { 
    return; 
  }
  m_InputPorts[modulePortNum].m_DevicePortNum = devicePortNum;
}

void PortManager::setOutputDevicePort(size_t devicePortNum, size_t modulePortNum)
{
  if (modulePortNum >= m_OutputPorts.size()) 
  { 
    return; 
  }
  m_OutputPorts[modulePortNum].m_DevicePortNum = devicePortNum;
}

void PortManager::setInputDevice(size_t modulePortNum, AudioStream* device, size_t devicePortNum)
{
  if (modulePortNum >= m_InputPorts.size())
  {
    return; 
  }
  m_InputPorts[modulePortNum].m_Device = device;
  m_InputPorts[modulePortNum].m_DevicePortNum = devicePortNum;
}

void PortManager::setOutputDevice(size_t modulePortNum, AudioStream* device, size_t devicePortNum)
{
  if (modulePortNum >= m_MaxOutputs) 
  { 
    return; 
  }
  m_OutputPorts[modulePortNum].m_Device = device;
  m_OutputPorts[modulePortNum].m_DevicePortNum = devicePortNum;
}

bool PortManager::inputsFull() const
{
  for (int i = 0; i < m_MaxInputs; i++)
  {
    if (isInputFree(i)) 
    { 
      return false; 
    }
  }
  return true;
}

bool PortManager::outputsFull() const
{
  for (size_t i{}; i < m_MaxOutputs; i++)
  {
    if (isOutputFree(i)) 
    { 
      return false; 
    }
  }
  return true;
}

void PortManager::disconnectAll()
{
  for (Port& inputPort: m_InputPorts)
  {
    inputPort.m_ConnectedModule = nullptr;
  }
  for (Port& outputPort: m_OutputPorts) 
  {
    outputPort.m_ConnectedModule = nullptr;
  }
}

Port PortManager::getOpenInputPort()
{
  for (size_t i{}; i < m_MaxInputs; i++)
  {
    if (m_InputPorts[i].isFree()) 
    { 
      return m_InputPorts[i]; 
    }
  }
  return Port{nullptr, -1};
}

Port PortManager::getOpenOutputPort()
{
  for (size_t i{}; i < m_MaxOutputs; i++)
  {
    if (m_OutputPorts[i].isFree()) 
    { 
      return m_OutputPorts[i];
    }
  }
  return Port{nullptr, -1};
}

void PortManager::connectInput(Module* source, int portNum) 
{ 
  if (!source || portNum < 0 || portNum >= m_MaxInputs) 
  { 
    return; 
  }
  m_InputPorts[portNum].m_ConnectedModule = source; 
}

void PortManager::connectOutput(Module* destination, int portNum) 
{ 
  if (!destination || portNum < 0 || portNum >= m_MaxOutputs) 
  {
    return;
  }
  m_OutputPorts[portNum].m_ConnectedModule = destination; 
}

void PortManager::disconnectInput(int portNum) 
{ 
  if (portNum < 0 || portNum >= m_MaxInputs) 
  { 
    return; 
  }
  m_InputPorts[portNum].m_ConnectedModule = nullptr;
}

void PortManager::disconnectOutput(int portNum)
{ 
  if (portNum < 0 || portNum >= m_MaxOutputs) 
  { 
    return; 
  }
  m_OutputPorts[portNum].m_ConnectedModule = nullptr;
}

Module* PortManager::inputModuleAt(int portNum) const 
{ 
  if (portNum < 0 || portNum >= m_MaxInputs) 
  { 
    return nullptr; 
  }
  return m_InputPorts[portNum].m_ConnectedModule;
}

Module* PortManager::outputModuleAt(int portNum) const 
{ 
  if (portNum < 0 || portNum >= m_MaxOutputs) 
  { 
    return nullptr; 
  }
  return m_OutputPorts[portNum].m_ConnectedModule;
}

bool PortManager::isInputFree(int portNum) const 
{ 
  if (portNum < 0 || portNum >= m_MaxInputs) 
  { 
    return false; 
  }
  return m_InputPorts[portNum].isFree(); 
}

bool PortManager::isOutputFree(int portNum) const 
{ 
  if (portNum < 0 || portNum >= m_MaxOutputs) 
  {
    return false;
  }
  return m_OutputPorts[portNum].isFree(); 
}