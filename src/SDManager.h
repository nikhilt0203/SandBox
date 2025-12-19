#pragma once
#include <string>
#include <vector>

namespace SDManager
{
  void init();
  bool write(const std::vector<std::string>& patch, std::string filePath);
  const std::vector<std::string>& getFileContents(std::string filePath);
}