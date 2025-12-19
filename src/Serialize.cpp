
#include "ModuleBuilder.h"
#include "SDManager.h"
#include "LEDManager.h"

static std::vector<std::string> s_SerializedPatch;

bool Serialize::savePatchToFile(std::string_view filePath) 
{
  return SDManager::write(serializeCurrentPatch(), filePath);
}

bool Serialize::loadPatchFromFile(std::string_view filePath, Patcher* patcher)
{ 
  if (!SD.exists(filePath.data())) {
    return false;
  }

  const std::vector<std::string>& fileContents = SDManager::getFileContents(filePath);

  Patcher::deleteAllPatches();
  Module::deleteAllModules();
  Grid::clear();

  if (fileContents.empty()) {
    return true;
  }

  for (const std::string& s: fileContents)
  {
    if (s.empty())  {
      continue;
    }

    if (s.substr(0, s.find("{")) == "Patch")
    {
      if (Patch* p = Patch::buildFromString(s)) {
        patcher->registerNewPatch(p);
      }
    }
    else if (Module* m = ModuleBuilder::buildFromString(s)) {
      LEDManager::placeModule(m);
    }
  }
  return true;
}

const std::vector<std::string>& Serialize::serializeCurrentPatch()
{
  size_t numModules = Module::getAllModules().size();
  size_t numPatches = Patcher::getAllPatches().size();

  s_SerializedPatch.clear();
  s_SerializedPatch.reserve(numModules + numPatches);

  for (Module* m: Module::getAllModules())
  {
    if (m) { 
      s_SerializedPatch.emplace_back(m->serialize());
    }
  }

  for (Patch* p: Patcher::getAllPatches())
  {
    if (p) { 
      s_SerializedPatch.emplace_back(p->serialize());
    }
  }

  return s_SerializedPatch;
}

bool isNumeric(char c) { return (c >= '0' && c <= '9') || c == '.'; }

int Serialize::extractIntBetween(const char* startDelimiter, const char* endDelimiter, std::string_view s)
{
  Serial.println("Extracting int between delimiters: " + String(startDelimiter) + " and " + String(endDelimiter) + " in string: " + String(s.data()));
  size_t start = s.find(startDelimiter);

  if (start == std::string::npos) { 
    return 0; 
  }
    
  size_t end = s.find(endDelimiter, start);

  if (end == std::string::npos) { 
    return 0; 
  }

  if (end <= start + 1 || end == start) { 
    return 0; 
  }

  return extractIntBetween(start, end, s);
}

int Serialize::extractIntBetween(size_t startIndex, size_t endIndex, std::string_view s)
{
  startIndex += 1;

  if (startIndex >= endIndex || endIndex >= s.length() || startIndex >= s.length()) { 
    return 0; 
  }

  std::string_view value = s.substr(startIndex, endIndex - startIndex);

  if (value.empty()) { 
    return 0; 
  }

  for (char c : value)
  {
    if (!isNumeric(c)) { 
      return 0; 
    }
  }

  return std::stoi(value.data());
}

float Serialize::extractFloatBetween(const char* startDelimiter, const char* endDelimiter, std::string_view s)
{
  size_t start = s.find(startDelimiter);

  if (start == std::string::npos) {
    return 0.0f;
  }
    
  size_t end = s.find(endDelimiter, start);

  if (end == std::string::npos) {
    return 0.0f;
  }

  if (start + 1 >= end || start == end) {
    return 0.0f;
  }

  return extractFloatBetween(start, end, s);
}

float Serialize::extractFloatBetween(size_t startIndex, size_t endIndex, std::string_view s)
{
  startIndex += 1;

  if (startIndex >= endIndex || endIndex >= s.length() || startIndex >= s.length()) {
    return 0.0f;
  }

  std::string_view value = s.substr(startIndex, endIndex - startIndex);

  if (value.empty()) {
    return 0.0f;
  }

  for (char c : value)
  {
    if (!isNumeric(c)) {
      return 0.0f;
    }
  }

  return std::stof(value.data());
}