#include "ModuleBuilder.h"
#include "USBOutput.h"
#include "Oscillator.h"
#include "LFO.h"
#include "VCA.h"
#include "VCF.h"
#include "Delay.h"
#include "Mixer.h"
#include "Amplifier.h"
#include "Mult.h"
#include "Sequencer.h"
#include "Envelope.h"
#include "DigitalCombine.h"
#include "Bitcrusher.h"
#include "DCGenerator.h"
#include "Oscilloscope.h"
#include "Patcher.h"
#include "Grid.h"
#include <string>
#include <map>
#include <functional>


struct ModuleDependencies
{
  PortManager portManager;
  ModuleBuilder moduleBuilder;
  IDGenerator idGenerator;
  ModuleUIElement uiElement;
};

static std::map<Module::Type, std::function<Module*(int, int)>> moduleFactory = {
    { Module::Type::USBOUT,      [](int r, int c) { return new USBOutput(r, c); } },
    { Module::Type::OSCILLATOR,  [](int r, int c) { return new Oscillator(r, c); } },
    { Module::Type::LFO,         [](int r, int c) { return new LFO(r, c); } },
    { Module::Type::MIXER,       [](int r, int c) { return new Mixer(r, c); } },
    { Module::Type::VCF,         [](int r, int c) { return new VCF(r, c); } },
    { Module::Type::COMBINE,     [](int r, int c) { return new DigitalCombine(r, c); } },
    { Module::Type::BITCRUSHER,  [](int r, int c) { return new Bitcrusher(r, c); } },
    { Module::Type::SEQUENCER,   [](int r, int c) { return new Sequencer(r, c); } },
    { Module::Type::VCA,         [](int r, int c) { return new VCA(r, c); } },
    { Module::Type::MULT,        [](int r, int c) { return new Mult(r, c); } },
    { Module::Type::DCGENERATOR, [](int r, int c) { return new DCGenerator(r, c); } },
    { Module::Type::ENVELOPE,    [](int r, int c) { return new Envelope(r, c); } },
    { Module::Type::AMPLIFIER,   [](int r, int c) { return new Amplifier(r, c); } },
    { Module::Type::DELAY,       [](int r, int c) { return new Delay(r, c); } },
    { Module::Type::OSCILLOSCOPE,[](int r, int c) { return new Oscilloscope(r, c); } }
  };

static std::vector<ModuleBuilder::BankInfo> allBankOptions{{ 
    {Module::Type::OSCILLATOR,   Oscillator::INFO.NAME, Oscillator::INFO.DESCRIPTION},
    {Module::Type::LFO,          LFO::INFO.NAME, "Outputs a low-frequency waveform."},
    {Module::Type::VCF,          VCF::INFO.NAME, VCF::INFO.DESCRIPTION},
    {Module::Type::MIXER,        Mixer::INFO.NAME, Mixer::INFO.DESCRIPTION},
    {Module::Type::VCA,          VCA::INFO.NAME, VCA::INFO.DESCRIPTION},
    {Module::Type::ENVELOPE,     Envelope::NAME, ""},
    {Module::Type::SEQUENCER,    Sequencer::NAME, ""},
    {Module::Type::USBOUT,       USBOutput::NAME, ""},
    {Module::Type::MULT,         Mult::INFO.NAME, Mult::INFO.DESCRIPTION},
    {Module::Type::DELAY,        Delay::NAME, ""},
    {Module::Type::DCGENERATOR,  DCGenerator::NAME, ""},
    {Module::Type::BITCRUSHER,   Bitcrusher::NAME, ""},
    {Module::Type::AMPLIFIER,    Amplifier::INFO.NAME, ""},
    {Module::Type::COMBINE,      DigitalCombine::NAME, ""},
    {Module::Type::OSCILLOSCOPE, Oscilloscope::INFO.NAME, Oscilloscope::INFO.DESCRIPTION}
  }};

void addBankOption(Module::Type type, const char* name, const char* description)
{
  allBankOptions.push_back({type, name, description});
}

std::array<ModuleBuilder::BankInfo*, 8> ModuleBuilder::bankOptions;

void ModuleBuilder::init()
{
  Grid::init();
  for (size_t i = 0; i < bankOptions.size(); i++)
    bankOptions[i] = &allBankOptions[i];
}

int numModuleTypes;
void init(std::initializer_list<Module::Type> types)
{
  numModuleTypes = types.size();
}

ModuleBuilder& ModuleBuilder::getInstance()
{
  static ModuleBuilder instance;
  return instance;
}

Module* ModuleBuilder::createModule(Module::Type type, int row, int col)
{
  Module* newModule = nullptr;
  int position = Grid::toPosition(row, col);

  if (!canCreate(position))
  {
    Serial.printf("Error: Cannot create module at (%d, %d)\n", row, col);
    return nullptr;
  }
  
  auto it = moduleFactory.find(type);

  if (it == moduleFactory.end())
  {
    Serial.println("Error: Invalid module type.");
    return nullptr;
  }

  AudioNoInterrupts();
  newModule = it->second(row, col);
  AudioInterrupts();
  return newModule;
}

Module* ModuleBuilder::buildFromString(std::string s) 
{
  std::string name = s.substr(0, s.find("{"));
  
  if (name == USBOutput::SERIALIZATION_NAME) 
    return USBOutput::buildFromString(s);

  if (name == Oscillator::INFO.NAME) 
    return Oscillator::buildFromString(s);

  if (name == LFO::NAME) 
    return LFO::buildFromString(s);

  if (name == Mixer::INFO.NAME) 
    return Mixer::buildFromString(s);

  if (name == VCF::INFO.NAME) 
    return VCF::buildFromString(s);

  if (name == DigitalCombine::NAME) 
    return DigitalCombine::buildFromString(s);

  if (name == Bitcrusher::NAME) 
    return Bitcrusher::buildFromString(s);

  if (name == Sequencer::NAME) 
    return Sequencer::buildFromString(s);

  if (name == Delay::NAME) 
    return Delay::buildFromString(s);

  if (name == VCA::INFO.NAME) 
    return VCA::buildFromString(s);

  if (name == Amplifier::INFO.NAME)
    return Amplifier::buildFromString(s);

  if (name == Mult::INFO.NAME)
    return Mult::buildFromString(s);

  if (name == Envelope::NAME)
    return Envelope::buildFromString(s);

  if (name == DCGenerator::NAME)
    return DCGenerator::buildFromString(s);

  return nullptr;
}

void ModuleBuilder::deleteModule(Module* m, Patcher* p)
{
  if (!m) return;

  p->deletePatchesWith(m);

  Module::deleteModule(m);
}

Module::Type ModuleBuilder::bankType(size_t bankIndex) 
{ 
  if (bankIndex >= bankOptions.size())
  {
    return Module::Type::NONE;
  }
  return bankOptions[bankIndex]->m_Type; 
}

const char* ModuleBuilder::bankName(size_t bankIndex) 
{ 
  if (bankIndex >= bankOptions.size())
  {
    return nullptr;
  }
  return bankOptions[bankIndex]->m_Name; 
}

void ModuleBuilder::displayBank(size_t index)
{
  DisplayManager::displayBank(Colors::getColor(bankType(index)), bankName(index));
}

void ModuleBuilder::slideBankWindow(int amt)
{
  static size_t startIndex = 0;
  const size_t numOptions = allBankOptions.size();

  startIndex = (startIndex + amt + numOptions) % numOptions;

  for (size_t i = 0; i < bankOptions.size(); i++)
  {
    bankOptions[i] = &allBankOptions[(startIndex + i) % numOptions];
  }
}

void addBuilderFunc(Module::Type type, std::function<Module*(int, int)> func)
{
  moduleFactory[type] = func;
}