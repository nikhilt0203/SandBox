#pragma once
#include "IDGenerator.h"
#include "Serialize.h"
#include "DisplayManager.h"
#include "PortManager.h"
#include "ModuleUIElement.h"
#include "Port.h"
#include <vector>
#include <memory>
#include <Audio.h>
#include <string>

class Module
{
private:
  static std::vector<Module*> allModules;

protected:
  AudioStream* m_Device;
  const DisplayManager& m_DisplayManager;
  IDGenerator m_ID;
  ModuleUIElement m_UIElement;
  PortManager m_Ports;
  int m_Row;
  int m_Col;
  uint32_t m_Color;
  bool m_DisplayUpdated;

  Module(AudioStream* device, const char* name, size_t maxSources, size_t maxDestinations, int row, int col);
  virtual ~Module();

public:
  enum class Type
  {
    NONE, OSCILLATOR, LFO, MIXER, COMBINE, MULT, 
    VCF, VCA, BITCRUSHER, SEQUENCER, SEQUENCERSTEP, 
    ENVELOPE, AMPLIFIER, USBOUT, DCGENERATOR, DELAY,
    OSCILLOSCOPE, /**Keep at end**/ NUM_TYPES
  };
  
  struct ModuleInfo
  {
    Type TYPE;
    const char* NAME;
    const char* DESCRIPTION;
    std::initializer_list<const char*> PARAMETER_NAMES;
    std::initializer_list<const char*> INPUT_PORT_NAMES;
    std::initializer_list<const char*> OUTPUT_PORT_NAMES;
  };


  Type m_Type;

  static void updateModules();
  static Module* getModule(int position);
  static Module* getModuleWithID(int moduleID);
  static void deleteModule(Module* m);
  static void deleteAllModules();
  static const std::vector<Module*>& getAllModules();

  void addSource(Module* src);
  void addDestination(Module* dest);
  void disconnectFrom(Module* other);
  void disconnectAll();

  virtual void update();
  virtual AudioStream* getInputDevice() const;
  virtual AudioStream* getOutputDevice() const;
  virtual int getOpenInputPort() const;
  virtual int getOpenOutputPort() const;
  virtual void changeParameter(int parameterNum, int amt);
  virtual void pressFallingEdge();
  virtual void pressRisingEdge();
  virtual std::string toString();

  bool inputsFull() const;
  bool outputsFull() const;
  bool isDisplayUpdated() const;

  const std::string& getName() const;
  int getRow() const;
  int getCol() const;
  int getPosition() const;
  int getID() const;
  const std::vector<Port>& getOutputs() const;
  const std::vector<Port>& getInputs() const;
  const ModuleUIElement& getUIElement() const;
  uint32_t getColor() const;

  void setPosition(int row, int col);
  void setName(const char* name);
  void setID(int moduleID);
  void setColor(uint32_t hexValue);
  void setColor(Type type);
  void markDisplayUpdated(bool isUpdated);
};