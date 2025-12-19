#include "ModulePorts.h"
#include "Module.h"

ModulePortArray::ModulePortArray(const Module& parent, size_t numPorts)
: m_ParentModule(parent),
  m_NumPorts(numPorts)
{
  m_Ports.reserve(m_NumPorts);

  for (size_t i{}; i < m_NumPorts; i++)
  {
    ModulePort port;
    port.m_Index = i;
    port.m_Device = std::make_pair(m_ParentModule.getDevice(), i);
    m_Ports.push_back(port);
  }
}
  