
#include "ModuleBuilder.h"
#include "SDManager.h"
#include "LEDManager.h"

bool Serialize::savePatchToFile(std::string filePath)
{
  return SDManager::write(currentPatchToString(), filePath);
}

bool Serialize::loadPatchFromFile(std::string filePath, Patcher* patcher)
{ 
  const std::vector<std::string>& fileContents = SDManager::getFileContents(filePath);
  if (fileContents.empty()) 
  {
    Serial.println("Error: Patch file is empty or does not exist.");
    return false;
  }

  AudioNoInterrupts();
  Patcher::deleteAllPatches();
  Module::deleteAllModules();
  Grid::clear();
  for (const std::string& s: fileContents)
  {
    if (s.empty()) 
      continue;

    if (s.substr(0, s.find("{")) == "Patch")
    {
      if (Patch* p = Patch::buildFromString(s))
      {
        patcher->registerNewPatch(p);
      }
      Serial.println("Loaded " + String(s.c_str()));
    }
    else
    {
      if (Module* m = ModuleBuilder::buildFromString(s))
      {
        LEDManager::placeModule(m);
      }
      Serial.println("Loaded " + String(s.c_str()));
    }
  }
  AudioInterrupts();
  return true;
}

const std::vector<std::string>& Serialize::currentPatchToString()
{
  size_t numModules = Module::getAllModules().size();
  size_t numPatches = Patcher::getAllPatches().size();

  static std::vector<std::string> serializedPatch;
  serializedPatch.clear();
  serializedPatch.reserve(numModules + numPatches);

  for (Module* m: Module::getAllModules())
  {
    if (!m) continue;
    serializedPatch.push_back(m->toString());
  }

  for (Patch* p: Patcher::getAllPatches())
  {
    if (!p) continue;
    serializedPatch.push_back(p->toString());
  }

  return serializedPatch;
}

bool isNumeric(char c) { return (c >= '0' && c <= '9') || c == '.'; }

int Serialize::extractIntBetween(const char* startDelimiter, const char* endDelimiter, const std::string& s)
{
  size_t start = s.find(startDelimiter);
  if (start == std::string::npos)
    return 0;
    
  size_t end = s.find(endDelimiter, start);
  if (end == std::string::npos)
    return 0;

  if (end <= start + 1 || end == start)
    return 0;

  return extractIntBetween(start, end, s);
}

int Serialize::extractIntBetween(size_t startIndex, size_t endIndex, const std::string& s)
{
  startIndex += 1;

  if (startIndex >= endIndex || 
      endIndex >= s.length() || 
      startIndex >= s.length())
    return 0;

  std::string value = s.substr(startIndex, endIndex - startIndex);

  if (value.empty()) 
    return 0;

  for (char c : value)
  {
    if (!isNumeric(c))
      return 0;
  }

  return std::stoi(value);
}

float Serialize::extractFloatBetween(const char* startDelimiter, const char* endDelimiter, const std::string& s)
{
  size_t start = s.find(startDelimiter);
  if (start == std::string::npos)
    return 0.0f;
    
  size_t end = s.find(endDelimiter, start);
  if (end == std::string::npos)
    return 0.0f;

  if (start + 1 >= end || start == end)
    return 0.0f;

  return extractFloatBetween(start, end, s);
}

float Serialize::extractFloatBetween(size_t startIndex, size_t endIndex, const std::string& s)
{
  startIndex += 1;

  if (startIndex >= endIndex || 
      endIndex >= s.length() || 
      startIndex >= s.length())
    return 0.0f;


  std::string value = s.substr(startIndex, endIndex - startIndex);

  if (value.empty()) 
    return 0.0f;

  for (char c : value)
  {
    if (!isNumeric(c))
      return 0.0f;
  }

  return std::stof(value);
}