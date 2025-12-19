#pragma once
#include "Module.h"
#include <memory>

struct Patch2
{
  const ModulePort& m_SourcePort;
  const ModulePort& m_DestinationPort;
  std::unique_ptr<AudioConnection> m_PatchCord;

  Patch2(const ModulePort& srcPort, const ModulePort& destPort)
  : m_SourcePort(srcPort),
    m_DestinationPort(destPort)
  {
    auto[srcDevice, srcDevicePort] = m_SourcePort.m_Device;
    auto[destDevice, destDevicePort] = m_DestinationPort.m_Device;

    m_PatchCord = std::make_unique<AudioConnection>(srcDevice, srcDevicePort, destDevice, destDevicePort);
  }

  const Module& getSource() const { return *m_SourcePort.m_ConnectedModule; }

  const Module& getDestination() const { return *m_DestinationPort.m_ConnectedModule; }
};

class Patcher2
{
private:
  static std::vector<std::unique_ptr<Patch2>> allPatches;

public:
  bool isPatched(Module& src, Module& dest)
  {
    for (const auto& patch : allPatches)
    {
      if (patch->getSource() == src && patch->getDestination() == dest) {
        return true;
      }
    }
    return false;
  }

  bool patchExists(Module& src, size_t srcPortIndex, Module& dest, size_t destPortIndex) 
  { 
    auto find = std::find_if(allPatches.begin(), allPatches.end(), 
      [&](const std::unique_ptr<Patch2>& patch) {
        return patch->getSource() == src &&
              patch->getDestination() == dest &&
              patch->m_SourcePort.m_Index == srcPortIndex &&
              patch->m_DestinationPort.m_Index == destPortIndex;
    });
            
    return find != allPatches.end();
  }

  bool patch(Module& src, size_t srcPortIndex, Module& dest, size_t destPortIndex)
  {
    const ModulePort& sourcePort = src.m_OutputPorts[srcPortIndex];
    const ModulePort& destPort = dest.m_InputPorts[destPortIndex];

    if (sourcePort.isFree() && destPort.isFree()) 
    {
      allPatches.emplace_back(std::make_unique<Patch2>(sourcePort, destPort));
      return true;
    }

    return false;
  }

  bool unpatch(Module& src, size_t srcPortIndex, Module& dest, size_t destPortIndex)
  {
    ModulePort& sourcePort = src.m_OutputPorts[srcPortIndex];
    ModulePort& destPort = dest.m_InputPorts[destPortIndex];

    if (!sourcePort.isFree() && !destPort.isFree())
    {
      sourcePort.disconnect();
      destPort.disconnect();
      src.disconnectFrom(&dest); 
      dest.disconnectFrom(&src);
    }

    auto it = std::remove_if(allPatches.begin(), allPatches.end(), 
      [&](const std::unique_ptr<Patch2>& patch) {
        return patch->getSource() == src &&
              patch->getDestination() == dest &&
              patch->m_SourcePort.m_Index == srcPortIndex &&
              patch->m_DestinationPort.m_Index == destPortIndex;
    });

    return it != allPatches.end();
  }
};