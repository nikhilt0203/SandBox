#include "SDManager.h"
#include "SD.h"

#define MAX_FILES 100

static std::vector<std::string> s_FileContents{};
static std::vector<std::string> s_DirectoryContents{};

void SDManager::init() 
{
  if (!SD.begin(BUILTIN_SDCARD)) 
  {
    Serial.println("SD card failed.");
    while (true) {}
  }

  Serial.println("SD initialized successfully.");
}

bool SDManager::write(const std::vector<std::string>& contents, std::string_view filePath)
{
  SD.remove(filePath.data());
  File fileOut = SD.open(filePath.data(), FILE_WRITE);

  if (!fileOut) 
  {
    Serial.print("Failed to open file: ");
    Serial.println(filePath.data());
    return false;
  }

  if (contents.empty()) 
  {
    fileOut.close();
    return false;
  }

  for (const std::string& s : contents)
  {
    fileOut.println(s.data());
  }

  fileOut.flush();
  fileOut.close();
  return true;
}

const std::vector<std::string>& SDManager::getFileContents(std::string_view filePath)
{
  s_FileContents.clear();

  if (!SD.exists(filePath.data()))
  {
    Serial.print("File does not exist: ");
    Serial.println(filePath.data());
    return s_FileContents;
  }

  File fileIn = SD.open(filePath.data(), FILE_READ);
  if (!fileIn) 
  {
    Serial.print("Failed to open file: ");
    Serial.println(filePath.data());
    return s_FileContents;
  }

  while (fileIn.available())
  {
    String line = fileIn.readStringUntil('\n');
    line.replace("\r", "");
    line.trim();

    if (line.length() == 0) 
    { 
      continue; 
    }
    s_FileContents.emplace_back(line.c_str());
  }

  fileIn.close();
  return s_FileContents;
}

const std::vector<std::string>& SDManager::getDirectoryContents(std::string_view directoryPath)
{
  s_DirectoryContents.clear();

  if (!SD.exists(directoryPath.data()))
  {
    Serial.print("File does not exist: ");
    Serial.println(directoryPath.data());
    return s_DirectoryContents;
  }

  File directory = SD.open(directoryPath.data(), FILE_READ);
  File file = directory.openNextFile();

  while (file) 
  {
    s_DirectoryContents.emplace_back(file.name());
    file = directory.openNextFile();
  }

  return s_DirectoryContents;
}