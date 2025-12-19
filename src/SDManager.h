#pragma once
#include <string>
#include <vector>

namespace SDManager
{
  void init();

  bool write(const std::vector<std::string>& lines, std::string_view filePath);

  const std::vector<std::string>& getFileContents(std::string_view filePath);

  const std::vector<std::string>& getDirectoryContents(std::string_view directoryPath = "/");
}