#pragma once
#include <vector>
#include <string>

class Patcher;
class Module;

namespace Serialize
{
  Module* buildFromString(std::string s);
  bool savePatchToFile(std::string filePath);
  bool loadPatchFromFile(std::string filePath, Patcher* patcher);
  const std::vector<std::string>& currentPatchToString();
  int extractIntBetween(const char* startDelimiter, const char* endDelimiter, const std::string& s);
  int extractIntBetween(size_t startIndex, size_t endIndex, const std::string& s);
  float extractFloatBetween(const char* startDelimiter, const char* endDelimiter, const std::string& s);
  float extractFloatBetween(size_t startIndex, size_t endIndex, const std::string& s);
}