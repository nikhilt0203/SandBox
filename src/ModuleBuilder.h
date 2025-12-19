#include "Module.h"
#include "Patcher.h"
#include "Grid.h"
#include <string>
#include <map>
#pragma once

class ModuleBuilder
{
public:
  class BankInfo
  {
  public:
    Module::Type m_Type;
    const char* m_Name;
    const char* m_Description;

    constexpr BankInfo() : m_Type(Module::Type::NONE), m_Name(""), m_Description("") {}

    constexpr BankInfo(Module::Type type, const char* name, const char* description) 
    : m_Type(type), m_Name(name), m_Description(description) {}
  };
public:
  static void init();

  static ModuleBuilder& getInstance();

  static Module* createModule(Module::Type type, int row, int col);

  static Module* buildFromString(std::string s);

  static void deleteModule(Module* m, Patcher* p);

  static void displayBank(size_t index);

  static void slideBankWindow(int amt);

  static Module::Type bankType(size_t bankIndex);

  static const char* bankName(size_t bankIndex);

  static bool canCreate(int position) { return !Grid::isModuleAt(position) && Grid::isValid(position); }

private:
  ModuleBuilder() = default;  

  static std::array<BankInfo*, 8> bankOptions;
};