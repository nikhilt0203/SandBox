#include "PortManager.h"
#include "Module.h"
#include "Port.h"

PortManager::PortManager(int numInputs, int numOutputs) 
  : m_MaxInputs(numInputs), m_MaxOutputs(numOutputs)
{
  m_InputPorts.reserve(m_MaxInputs);
  m_OutputPorts.reserve(m_MaxOutputs);
  
  for (int i = 0; i < m_MaxInputs; i++)
    m_InputPorts.emplace_back(Port::INPUT_NAME);

  for (int i = 0; i < m_MaxOutputs; i++)
    m_OutputPorts.emplace_back(Port::OUTPUT_NAME);
}

PortManager::~PortManager() 
{  
  m_InputPorts.clear();
  m_OutputPorts.clear();
}

void PortManager::setInputNames(std::initializer_list<const char*> names)
{
  size_t index = 0;
  for (Port& port : m_InputPorts)
  {
    if (index < names.size())
      port.m_Name = names.begin()[index];
    index++;
  }
}

void PortManager::setOutputNames(std::initializer_list<const char*> names)
{
  size_t index = 0;
  for (Port& port : m_OutputPorts)
  {
    if (index < names.size())
      port.m_Name = names.begin()[index];
    index++;
  }
}

void PortManager::setInputDevicePort(int portNum, int devicePortNum)
{
  if (portNum < 0 || portNum >= m_MaxInputs)
    return;
  m_InputPorts[portNum].m_DevicePortNum = devicePortNum;
}

void PortManager::setOutputDevicePort(int portNum, int devicePortNum)
{
  if (portNum < 0 || portNum >= m_MaxOutputs)
    return;
  m_OutputPorts[portNum].m_DevicePortNum = devicePortNum;
}

void PortManager::setInputDevice(int portNum, AudioStream* device, int devicePortNum)
{
  if (portNum < 0 || portNum >= m_MaxInputs)
    return;
  m_InputPorts[portNum].m_Device = device;
  m_InputPorts[portNum].m_DevicePortNum = devicePortNum;
}

void PortManager::setOutputDevice(int portNum, AudioStream* device, int devicePortNum)
{
  if (portNum < 0 || portNum >= m_MaxOutputs)
    return;
  m_OutputPorts[portNum].m_Device = device;
  m_OutputPorts[portNum].m_DevicePortNum = devicePortNum;
}

bool PortManager::inputsFull() const
{
  for (int i = 0; i < m_MaxInputs; i++)
  {
    if (isInputFree(i)) 
      return false;
  }
  return true;
}

bool PortManager::outputsFull() const
{
  for (int i = 0; i < m_MaxOutputs; i++)
  {
    if (isOutputFree(i))
      return false;
  }
  return true;
}

void PortManager::disconnectAll()
{
  for (Port& inputPort: m_InputPorts) 
    inputPort.m_ConnectedModule = nullptr;
  for (Port& outputPort: m_OutputPorts) 
    outputPort.m_ConnectedModule = nullptr;
} 

int PortManager::getOpenInputPort() const
{
  for (int i = 0; i < m_MaxInputs; i++)
  {
    if (m_InputPorts[i].isFree()) 
      return i;
  }
  return -1;
}

int PortManager::getOpenOutputPort() const
{
  for (int i = 0; i < m_MaxOutputs; i++)
  {
    if (m_OutputPorts[i].isFree()) 
      return i;
  }
  return -1;
}

int PortManager::maxInputs() const { return m_MaxInputs; }

int PortManager::maxOutputs() const { return m_MaxOutputs; }

void PortManager::connectInput(Module* source, int portNum) 
{ 
  if (!source || portNum < 0 || portNum >= m_MaxInputs) return;
  m_InputPorts[portNum].m_ConnectedModule = source; 
}

void PortManager::connectOutput(Module* destination, int portNum) 
{ 
  if (!destination || portNum < 0 || portNum >= m_MaxOutputs) return;
  m_OutputPorts[portNum].m_ConnectedModule = destination; 
}

void PortManager::disconnectInput(int portNum) 
{ 
  if (portNum < 0 || portNum >= m_MaxInputs) return;
  m_InputPorts[portNum].m_ConnectedModule = nullptr;
}

void PortManager::disconnectOutput(int portNum)
{ 
  if (portNum < 0 || portNum >= m_MaxOutputs) return;
  m_OutputPorts[portNum].m_ConnectedModule = nullptr; 
}

Module* PortManager::inputModuleAt(int portNum) const 
{ 
  if (portNum < 0 || portNum >= m_MaxInputs) return nullptr;
  return m_InputPorts[portNum].m_ConnectedModule;
}

Module* PortManager::outputModuleAt(int portNum) const 
{ 
  if (portNum < 0 || portNum >= m_MaxOutputs) return nullptr;
  return m_OutputPorts[portNum].m_ConnectedModule;
}

bool PortManager::isInputFree(int portNum) const 
{ 
  if (portNum < 0 || portNum >= m_MaxInputs) return false;
  return m_InputPorts[portNum].isFree(); 
}

bool PortManager::isOutputFree(int portNum) const 
{ 
  if (portNum < 0 || portNum >= m_MaxOutputs) return false;
  return m_OutputPorts[portNum].isFree(); 
}

const std::vector<Port>& PortManager::getInputs() const { return m_InputPorts; }

const std::vector<Port>& PortManager::getOutputs() const { return m_OutputPorts; }