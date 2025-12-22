#include "modules/USBOutput.h"
#include "modules/Oscillator.h"
#include "modules/LFO.h"
#include "modules/VCA.h"
#include "modules/VCF.h"
#include "modules/Delay.h"
#include "modules/Mixer.h"
#include "modules/Amplifier.h"
#include "modules/Mult.h"
#include "modules/Sequencer.h"
#include "modules/Envelope.h"
#include "modules/DigitalCombine.h"
#include "modules/Bitcrusher.h"
#include "modules/DCGenerator.h"
#include "modules/Oscilloscope.h"
#include "modules/NoiseGenerator.h"
#include "Grid.h"
#include "UIElements.h"
#include <string>
#include <map>
#include <functional>
#include "ModuleBuilder.h"

void registerModules()
{
  //Defines the order that the modules appear in the bank.
  
  ModuleBuilder::registerModule(Oscillator::INFO,     [](){ return new Oscillator(); });
  ModuleBuilder::registerModule(LFO::INFO,            [](){ return new LFO(); });
  ModuleBuilder::registerModule(VCF::INFO,            [](){ return new VCF(); });
  ModuleBuilder::registerModule(Mixer::INFO,          [](){ return new Mixer(); });
  ModuleBuilder::registerModule(VCA::INFO,            [](){ return new VCA(); });
  ModuleBuilder::registerModule(Envelope::INFO,       [](){ return new Envelope(); });
  ModuleBuilder::registerModule(Sequencer::INFO,      [](){ return new Sequencer(); });
  ModuleBuilder::registerModule(USBOutput::INFO,      [](){ return new USBOutput(); });
  ModuleBuilder::registerModule(Oscilloscope::INFO,   [](){ return new Oscilloscope(); });
  ModuleBuilder::registerModule(Mult::INFO,           [](){ return new Mult(); });
  ModuleBuilder::registerModule(DCGenerator::INFO,    [](){ return new DCGenerator(); });
  ModuleBuilder::registerModule(Delay::INFO,          [](){ return new Delay(); });
  ModuleBuilder::registerModule(Amplifier::INFO,      [](){ return new Amplifier(); });
  ModuleBuilder::registerModule(Bitcrusher::INFO,     [](){ return new Bitcrusher(); });
  ModuleBuilder::registerModule(DigitalCombine::INFO, [](){ return new DigitalCombine(); });
  ModuleBuilder::registerModule(NoiseGenerator::INFO, [](){ return new NoiseGenerator(); });
}

static std::map<ModuleConfig::Type, std::function<Module*()>> moduleFactory;

static std::vector<ModuleBuilder::BankInfo> allBankOptions;

std::array<ModuleBuilder::BankInfo*, Grid::COLS> ModuleBuilder::bankOptions;

void ModuleBuilder::init()
{
  Grid::init();
  registerModules();

  for (size_t i{}; i < bankOptions.size(); i++)
  {
    bankOptions[i] = &allBankOptions[i % allBankOptions.size()];
  }
}

ModuleBuilder& ModuleBuilder::getInstance()
{
  static ModuleBuilder instance;
  return instance;
}

void placeModule(Module* m, int row, int col)
{
  if (!m) 
  { 
    return; 
  }
  m->getLEDElement().setPosition(row, col);
  Grid::updateSquare(Grid::Space::MODULE, row, col);
}

Module* ModuleBuilder::createModule(ModuleConfig::Type type, int row, int col)
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

  if (newModule = it->second(); newModule)
  {
    placeModule(newModule, row, col);

    if (newModule->getType() == ModuleConfig::Type::SEQUENCER) 
    {
      Sequencer::createSteps(static_cast<Sequencer&>(*newModule));
    }

    AudioInterrupts();
    return newModule;
  }

  AudioInterrupts();
  return nullptr;
}

Module* ModuleBuilder::buildFromString(std::string_view s) 
{
  Serial.println("Building module from string: " + String(s.data()));
  std::string_view name = s.substr(0, s.find("{"));

  Module* module = nullptr;

  if (name == USBOutput::INFO.NAME) 
  {
    module = USBOutput::buildFromString(s);
  }
  else if (name == Oscillator::INFO.NAME) 
  {
    module = Oscillator::buildFromString(s);
  }
  else if (name == LFO::INFO.NAME) 
  {
    module = LFO::buildFromString(s);
  }
  else if (name == Mixer::INFO.NAME) 
  {
    module = Mixer::buildFromString(s);
  }
  else if (name == VCF::INFO.NAME) 
  {
    module = VCF::buildFromString(s);
  }
  else if (name == DigitalCombine::INFO.NAME) 
  {
    module = DigitalCombine::buildFromString(s);
  }
  else if (name == Bitcrusher::INFO.NAME) 
  {
    module = Bitcrusher::buildFromString(s);
  }
  else if (name == Sequencer::INFO.NAME) 
  {
    module = Sequencer::buildFromString(s);
  }
  else if (name == Delay::INFO.NAME) 
  {
    module = Delay::buildFromString(s);
  }
  else if (name == VCA::INFO.NAME) 
  {
    module = VCA::buildFromString(s);
  }
  else if (name == Amplifier::INFO.NAME) 
  {
    module = Amplifier::buildFromString(s);
  }
  else if (name == Mult::INFO.NAME) 
  {
    module = Mult::buildFromString(s);
  }
  else if (name == Envelope::INFO.NAME) 
  {
    module = Envelope::buildFromString(s);
  }
  else if (name == DCGenerator::INFO.NAME) 
  {
    module = DCGenerator::buildFromString(s);
  }
  else if (name == Oscilloscope::INFO.NAME) 
  {
    module = Oscilloscope::buildFromString(s);
  }
  else if (name == NoiseGenerator::INFO.NAME) 
  {
    module = NoiseGenerator::buildFromString(s);
  }
  else if (!module) 
  {
    return nullptr;
  }

  return module;
}

void ModuleBuilder::deleteModule(Module* m)
{
  if (!m) {
    return;
  }

  if (m->getType() == ModuleConfig::Type::SEQUENCER) 
  {
    Sequencer::deleteSteps(static_cast<Sequencer&>(*m));
  }

  if (m->getType() != ModuleConfig::Type::SEQUENCERSTEP) 
  {
    Grid::updateSquare(Grid::Space::EMPTY, m->getLEDElement().row, m->getLEDElement().col);
    Module::deleteModule(m);
  }
}

ModuleConfig::Type ModuleBuilder::bankType(size_t bankIndex) 
{ 
  if (bankIndex >= bankOptions.size()) 
  {
    return ModuleConfig::Type::NONE;
  }
  return bankOptions[bankIndex]->type;
}

const char* ModuleBuilder::bankName(size_t bankIndex) 
{ 
  if (bankIndex >= bankOptions.size()) {
    return "";
  }
  return bankOptions[bankIndex]->name;
}

void ModuleBuilder::displayBank(int position)
{
  size_t index = position - (Grid::ROWS - 1) * Grid::COLS;
  
  if (index >= bankOptions.size()) {
    return;
  }

  auto& currentBank = bankOptions[index];
  uint16_t bankColor = Colors::to565(Colors::getColor(currentBank->type));

  DisplayManager::draw<BankDisplayPage>(
    currentBank->name,
    currentBank->description,
    bankColor);
}

void ModuleBuilder::slideBankWindow(int amt)
{
  static size_t startIndex = 0;
  const size_t numOptions = allBankOptions.size();

  if (numOptions == 0) {
    return;
  }

  startIndex = (startIndex + amt + numOptions) % numOptions;

  for (size_t i{}; i < bankOptions.size(); i++)
  {
    bankOptions[i] = &allBankOptions[(startIndex + i) % numOptions];
  }
}

void ModuleBuilder::registerModule(ModuleConfig::Info info, std::function<Module*()> creatorFunc)
{
  moduleFactory[info.TYPE] = creatorFunc;
  allBankOptions.push_back({info.TYPE, info.NAME, info.DESCRIPTION});
}
