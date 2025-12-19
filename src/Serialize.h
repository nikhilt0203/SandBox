#pragma once
#include <vector>
#include <string>

class Patcher;
class Module;

namespace Serialize
{
  const std::vector<std::string>& serializeCurrentPatch();
  
  Module* buildFromString(std::string_view s);

  bool savePatchToFile(std::string_view filePath);

  bool loadPatchFromFile(std::string_view filePath, Patcher* patcher);

  int extractIntBetween(const char* startDelimiter, const char* endDelimiter, std::string_view s);

  int extractIntBetween(size_t startIndex, size_t endIndex, std::string_view s);

  float extractFloatBetween(const char* startDelimiter, const char* endDelimiter, std::string_view s);

  float extractFloatBetween(size_t startIndex, size_t endIndex, std::string_view s);
}