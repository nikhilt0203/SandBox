#include "Colors.h"
#include "Module.h"
#include "Grid.h"

std::vector<Module*> Module::allModules{};

Module::Module(AudioStream* device, const char* name, size_t maxInputs, size_t maxOutputs) 
: m_Device(device), 
  m_ID(name),
  m_UIElement(),
  m_Ports(maxInputs, maxOutputs),
  m_DisplayManager(DisplayManager::getInstance()),
  m_InputPorts(*this, maxInputs),
  m_OutputPorts(*this, maxOutputs)
{
  allModules.push_back(this);
}

Module::~Module()
{
  disconnectAll();
  delete m_Device;
}

void Module::updateModules()
{
  for (Module* m: allModules)
  {
    if (!m) 
    { 
      continue; 
    }
    m->update();
  }
}

Module* Module::getModuleAt(int position)
{
  if (!Grid::isInBounds(position)) 
  { 
    return nullptr; 
  }

  if (Grid::isInBank(position)) 
  { 
    return nullptr; 
  }

  for (Module* m : allModules)
  {
    if (Grid::toPosition(m->getLEDElement().row, m->getLEDElement().col) == position)
    {
      return m;
    }
  }
  return nullptr;
}

Module* Module::getModule(int moduleID)
{
  for (Module* m : allModules)
  {
    if (!m) 
    { 
      continue; 
    }
    if (m->getType() == ModuleConfig::Type::SEQUENCERSTEP) 
    { 
      continue; 
    }
    if (m->getID() == moduleID) 
    { 
      return m; 
    }
  }
  return nullptr;
}

void Module::deleteModule(Module* module)
{
  if (!module) { 
    return; 
  }

  for (auto it = allModules.begin(); it != allModules.end(); ++it)
  {
    if (*it == module)
    {
      allModules.erase(it);
      return;
    }
  }
  
  delete module; 
}

void Module::deleteAllModules()
{
  std::vector<Module*> modules = allModules;
  allModules.clear();

  for (Module* m : modules)
  {
    if (!m) { 
      continue; 
    }
    if (m->getType() == ModuleConfig::Type::SEQUENCERSTEP) { 
      continue; 
    }
    delete m;
  }
  
  ModuleID::resetAllIDs();
}

void Module::addSource(Module* src)
{
  if (!src)
  {
    Serial.println("Error: Source module does not exist and can't be connected.");
    return;
  }

  for (size_t i{}; i < m_Ports.maxInputs(); i++)
  {
    if (m_Ports.isInputFree(i))
    {
      m_Ports.connectInput(src, i);
      return;
    }
  }
}

void Module::addDestination(Module* dest)
{
  if (!dest)
  {
    Serial.println("Error: Destination module does not exist and can't be connected.");
    return;
  }

  for (size_t i{}; i < m_Ports.maxOutputs(); i++)
  {
    if (m_Ports.isOutputFree(i))
    {
      m_Ports.connectOutput(dest, i);
      return;
    }
  }
}

void Module::addSource(Module* src, int portNum)
{
  if (m_InputPorts.isFree(portNum)) {
    m_InputPorts[portNum].connect(src);
  }
}

void Module::addDestination(Module* dest, int portNum)
{
  if (m_OutputPorts.isFree(portNum)) {
    m_OutputPorts[portNum].connect(dest);
  }
}

// void Module::disconnectFrom(const Module& other)
// {
//   for (ModulePort& port : m_InputPorts.getPorts())
//   {
//     if (port.m_ConnectedModule == other) { 
//       port.disconnect(); 
//     }
//   }

//   for (ModulePort& port : m_OutputPorts.getPorts())
//   {
//     if (port.m_ConnectedModule == other) { 
//       port.disconnect(); 
//     } 
//   }
// } 


void Module::disconnectFrom(Module* other)
{
  if (!other)
  {
    Serial.println("Error: Module does not exist and cannot be disconnected from.");
    return;
  }

  Serial.printf("\nDisconnecting %s from %s\n", this->getName().data(), other->getName().data());
  for (size_t i{}; i < m_Ports.maxInputs(); i++)
  {
    if (m_Ports.inputModuleAt(i) == other)
    {
      m_Ports.disconnectInput(i);
      break;
    }
  }

  for (size_t i{}; i < m_Ports.maxOutputs(); i++)
  {
    if (m_Ports.outputModuleAt(i) == other)
    {
      m_Ports.disconnectOutput(i);
      break;
    }
  } 
} 

const std::vector<Module*>& Module::getAllModules() { return allModules; }

void Module::disconnectAll() 
{ 
  for (size_t i{}; i < m_Ports.maxInputs(); i++) 
  {
    if (Module* m = m_Ports.inputModuleAt(i)) {
      m->disconnectFrom(this);
    }
  }

  for (size_t i{}; i < m_Ports.maxOutputs(); i++) 
  {
    if (Module* m = m_Ports.outputModuleAt(i)) {
      m->disconnectFrom(this);
    }
  }
  m_Ports.disconnectAll();
} 

// void Module::disconnectAll() 
// { 
//   for (ModulePort& port : m_InputPorts.getPorts()) 
//   {
//     if (!port.isFree()) 
//     {
//       port.m_ConnectedModule->disconnectFrom(this);
//       port.disconnect();
//     }
//   }

//   for (ModulePort& port : m_OutputPorts.getPorts()) 
//   {
//     if (!port.isFree()) 
//     {
//       port.m_ConnectedModule->disconnectFrom(this);
//       port.disconnect();
//     }
//   }
// } 

AudioStream* Module::getInputDevice() const
{
  if (!m_Device)
  {
    Serial.println("Error: Device is invalid.");
    return nullptr;
  }
  return m_Device;
}