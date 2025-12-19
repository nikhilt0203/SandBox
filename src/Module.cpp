#include "Colors.h"
#include "Module.h"

std::vector<Module*> Module::allModules;

void Module::setColor(Module::Type type) { m_Color = Colors::getColor(type); }

Module::Module(AudioStream* device, const char* name, size_t maxSources, size_t maxDestinations, int row, int col) 
: m_Device(device), 
  m_DisplayManager(DisplayManager::getInstance()), 
  m_ID(name),
  m_Ports(maxSources, maxDestinations),
  m_Row(row),
  m_Col(col),
  m_Color(0),
  m_DisplayUpdated(true)
{
  setName(name);
  allModules.push_back(this);
}

Module::~Module()
{
  disconnectAll();
  m_Device = nullptr;
}

void Module::updateModules()
{
  for (Module* m: allModules)
  {
    m->update();
  }
}

Module* Module::getModule(int position)
{
  if (position < 0 || position > 55) return nullptr;

  for (Module* m : allModules)
  {
    if (m->getPosition() == position)
    {
      return m;
    }
  }
  return nullptr;
}

Module* Module::getModuleWithID(int moduleID)
{
  for (Module* m : allModules)
  {
    if (!m || m->m_Type == Type::SEQUENCERSTEP) 
      continue;
    if (m->getID() == moduleID)
      return m;
  }
  return nullptr;
}

void Module::deleteModule(Module* m)
{
  for (auto it = allModules.begin(); it != allModules.end(); ++it)
  {
    if (*it == m)
    {
      delete *it;
      allModules.erase(it);
      break;
    }
  }
}

void Module::deleteAllModules()
{
  std::vector<Module*> modules = allModules;
  allModules.clear();

  for (Module* m : modules)
  {
    if (!m) continue;
    if (m->m_Type == Type::SEQUENCERSTEP) continue;
    delete m;
  }
  
  //IDGenerator::resetAllIDs();
}

void Module::addSource(Module* src)
{
  if (!src)
  {
    Serial.println("Error: Source module does not exist and can't be connected.");
    return;
  }

  for (int i = 0; i < m_Ports.maxInputs(); i++)
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

  for (int i = 0; i < m_Ports.maxOutputs(); i++)
  {
    if (m_Ports.isOutputFree(i))
    {
      m_Ports.connectOutput(dest, i);
      return;
    }
  }
}

void Module::disconnectFrom(Module* other)
{
  if (!other)
  {
    Serial.println("Error: Module does not exist and cannot be disconnected from.");
    return;
  }

  Serial.printf("\nDisconnecting %s from %s\n", this->getName().c_str(), other->getName().c_str());
  for (int i = 0; i < m_Ports.maxInputs(); i++)
  {
    if (m_Ports.inputModuleAt(i) == other)
    {
      m_Ports.disconnectInput(i);
      break;
    }
  }

  for (int i = 0; i < m_Ports.maxOutputs(); i++)
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
  for (int i = 0; i < m_Ports.maxInputs(); i++) 
  {
    if (Module* m = m_Ports.inputModuleAt(i))
    {
      m->disconnectFrom(this);
    }
  }

  for (int i = 0; i < m_Ports.maxOutputs(); i++) 
  {
    if (Module* m = m_Ports.outputModuleAt(i))
    {
      m->disconnectFrom(this);
    }
  }
  m_Ports.disconnectAll();
} 

void Module::setName(const char* name) { m_ID.setName(name); }

void Module::setID(int moduleID) { m_ID.setIDNum(moduleID); }

void Module::setPosition(int row, int col)
{
  if (row > 7 || row < 0 || col > 7 || col < 0)
  {
    Serial.println("Error: Invalid position.");
    return;
  }

  m_Row = row;
  m_Col = col;
}

AudioStream* Module::getInputDevice() const
{
  if (!m_Device)
  {
    Serial.println("Error: Device is invalid.");
    return nullptr;
  }
  return m_Device;
}

//By default the output and input devices are the same
AudioStream* Module::getOutputDevice() const { return getInputDevice(); }

int Module::getOpenInputPort() const { return m_Ports.getOpenInputPort(); }

int Module::getOpenOutputPort() const{ return m_Ports.getOpenOutputPort(); }

bool Module::inputsFull() const { return m_Ports.inputsFull(); }

bool Module::outputsFull() const { return m_Ports.outputsFull(); }

const std::string& Module::getName() const { return m_ID.getName(); }

int Module::getRow() const { return m_Row; }

int Module::getCol() const { return m_Col; }

int Module::getPosition() const { return m_Col + m_Row * 8; }

int Module::getID() const { return m_ID.getIDNum(); }

const std::vector<Port>& Module::getOutputs() const { return m_Ports.getOutputs(); }

const std::vector<Port>& Module::getInputs() const { return m_Ports.getInputs(); }

const ModuleUIElement& Module::getUIElement() const { return m_UIElement; }

bool Module::isDisplayUpdated() const { return m_DisplayUpdated; }

uint32_t Module::getColor() const { return m_Color; }

void Module::setColor(uint32_t hexValue) 
{ 
  m_Color = hexValue; 
  m_UIElement.updateColor(m_Color);
  m_DisplayUpdated = false;
}

void Module::markDisplayUpdated(bool isUpdated) { m_DisplayUpdated = isUpdated; }

void Module::changeParameter(int parameterNum, int amt) {}

void Module::pressRisingEdge() 
{
  m_DisplayManager.displayMe(this);
 //DisplayManager::print(m_UIElement.getDisplayElement().parameters);
}

void Module::pressFallingEdge() {}

void Module::update() {}

std::string Module::toString() { return ""; }