#pragma once
#include "Serialize.h"
#include "ModuleID.h"
#include "ModuleConfig.h"
#include "DisplayManager.h"
#include "PortManager.h"
#include "ModuleUIElement.h"
#include "Port.h"
#include <vector>
#include <memory>
#include <Audio.h>
#include <string>
#include "ModulePorts.h"

class Module
{
public:
  static Module* getModule(int moduleID);
  static Module* getModuleAt(int position);
  static const std::vector<Module*>& getAllModules();

  static void updateModules();

  static void deleteModule(Module* m);
  static void deleteAllModules();

public:
  void addSource(Module* src);
  void addDestination(Module* dest);

  void addSource(Module* dest, int portNum);
  void addDestination(Module* dest, int portNum);

  void disconnectFrom(Module* other);
  void disconnectAll();

  //Output and input devices are the same by default, override if needed
  virtual AudioStream* getInputDevice() const;
  virtual AudioStream* getOutputDevice() const { return getInputDevice(); };
  AudioStream* getDevice() const { return m_Device; };

  virtual Port getOpenInputPort() { return m_Ports.getOpenInputPort(); };
  virtual Port getOpenOutputPort() { return m_Ports.getOpenOutputPort(); };

  virtual void update() {}
  virtual void changeParameter(int parameterNum, int amt) {}
  virtual void pressFallingEdge() {}
  virtual void pressRisingEdge() {}

  virtual std::string_view serialize() { return m_ID.getSerializationID(); }
  bool inputsFull() const { return m_Ports.inputsFull(); };
  bool outputsFull() const { return m_Ports.outputsFull(); };

  bool isDisplayUpdated() const;

  std::string_view getName() const { return m_ID.getName(); }
  int getID() const { return m_ID.getIDNum(); };
  void setID(int moduleID) { m_ID.setIDNum(moduleID); }

  ModuleConfig::Type getType() const { return m_Type; }

  const std::vector<Port>& getOutputs() const { return m_Ports.getOutputs(); }
  const std::vector<Port>& getInputs() const { return m_Ports.getInputs(); }

  ModuleUIElement& getUIElement() { return m_UIElement; }
  ModuleUIElement::LEDElement& getLEDElement() { return m_UIElement.getLEDElement(); }
  ModuleUIElement::DisplayElement& getDisplayElement() { return m_UIElement.getDisplayElement(); }

  uint32_t getColor() const { return m_UIElement.getColor(); }

  bool operator==(const Module& other) const { return getID() == other.getID(); }

private:
  static std::vector<Module*> allModules;

protected:
  AudioStream* m_Device;
  ModuleID m_ID;
  ModuleUIElement m_UIElement;
  PortManager m_Ports;
  const DisplayManager& m_DisplayManager;
  ModuleConfig::Type m_Type;
public:
  ModulePortArray m_InputPorts;
  ModulePortArray m_OutputPorts;


protected:
  Module(AudioStream* device, const char* name, size_t maxInputs, size_t maxOutputs);
  
  virtual ~Module();

};