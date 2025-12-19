#include <Audio.h>
#include <tuple>
#include <optional>
#pragma once

class Module;

struct ModulePort
{
  const char* m_Name{nullptr};
  Module* m_ConnectedModule{nullptr};
  std::pair<AudioStream*, uint8_t> m_Device{nullptr, 0};
  size_t m_Index{};

  ModulePort() = default;

  bool isFree() const { return !m_ConnectedModule; }

  void connect(Module* module) { m_ConnectedModule = module; }

  void disconnect() { m_ConnectedModule = nullptr; }
};

class ModulePortArray
{
private:
  const Module& m_ParentModule;
  const size_t m_NumPorts;
  std::vector<ModulePort> m_Ports{};

public:
  ModulePortArray(const Module& parent, size_t numPorts);

  bool isFree(size_t portNum)
  {
    if (portNum >= m_NumPorts) { 
      return false; 
    }
    return !m_Ports[portNum].m_ConnectedModule;
  }

  void setDevice(size_t portNum, std::pair<AudioStream*, uint8_t> device)
  {
    if (portNum < m_NumPorts) { 
      m_Ports[portNum].m_Device = device;
    }
  }

  ModulePort* getFreePort()
  {
    auto firstFreePort = std::find_if(m_Ports.begin(), m_Ports.end(), 
      [](const ModulePort& p){ return p.isFree(); });

    return (firstFreePort != m_Ports.end()) ? &(*firstFreePort) : nullptr;
  }

  ModulePort* getPort(size_t portNum) { return (portNum < m_NumPorts) ? &m_Ports[portNum] : nullptr; }

  std::vector<ModulePort>& getPorts() { return m_Ports; }

  void setNames(std::initializer_list<const char*> portNames)
  {
    for (size_t i{}; i < m_NumPorts && i < portNames.size(); i++)
    {
      m_Ports[i].m_Name = portNames.begin()[i];
    }
  }

  ModulePort& operator[](size_t index) { return m_Ports[index]; }

  size_t size() const { return m_NumPorts; }
};