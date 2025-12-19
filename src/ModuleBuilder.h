#include "Module.h"
#include "Patcher.h"
#include "ModuleConfig.h"
#include "Grid.h"
#include <string>
#include <map>
#pragma once

class ModuleBuilder
{
public:
  struct BankInfo
  {
    const ModuleConfig::Type type;
    const char* const name;
    const char* const description;

    constexpr BankInfo(ModuleConfig::Type type, const char* name, const char* description) 
    : type(type), name(name), description(description) {}

    constexpr BankInfo() 
    : type(ModuleConfig::Type::NONE), name(""), description("") {}
  };

public:
  static void init();

  static ModuleBuilder& getInstance();

  static Module* createModule(ModuleConfig::Type type, int row, int col);

  static Module* buildFromString(std::string_view s);

  static void deleteModule(Module* m);

  static void displayBank(int position);

  static void slideBankWindow(int amt);

  static ModuleConfig::Type bankType(size_t bankIndex);

  static const char* bankName(size_t bankIndex);

  static bool canCreate(int position) { return Grid::isValid(position) && !Grid::isModuleAt(position); }

  static void registerModule(ModuleConfig::Info info, std::function<Module*()> creatorFunc);

private:
  ModuleBuilder() = default;  

  static std::array<BankInfo*, 8> bankOptions;
};