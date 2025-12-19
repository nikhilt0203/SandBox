#include "SDManager.h"
#include "SD.h"

void SDManager::init() 
{
  if (!SD.begin(BUILTIN_SDCARD)) 
  {
    Serial.println("SD card failed.");
    while (true) {} // stop if SD is missing
  }
  Serial.println("SD initialized successfully.");
}

bool SDManager::write(const std::vector<std::string>& contents, std::string filePath)
{
  SD.remove(filePath.c_str());
  File fileOut = SD.open(filePath.c_str(), FILE_WRITE);
  if (!fileOut) 
  {
    Serial.print("Failed to open file for writing: ");
    Serial.println(filePath.c_str());
    return false;
  }

  for (const std::string& s : contents)
    fileOut.println(s.c_str());

  fileOut.flush();
  fileOut.close();
  return true;
}

const std::vector<std::string>& SDManager::getFileContents(std::string filePath)
{
  static std::vector<std::string> contents;
  contents.clear();

  if (!SD.exists(filePath.c_str()))
  {
    Serial.print("File does not exist: ");
    Serial.println(filePath.c_str());
    return contents;
  }

  File fileIn = SD.open(filePath.c_str(), FILE_READ);
  if (!fileIn) 
  {
    Serial.print("Failed to open file: ");
    Serial.println(filePath.c_str());
    return contents;
  }

  while (fileIn.available())
  {
    String line = fileIn.readStringUntil('\n');
    line.replace("\r", "");
    line.trim();
    if (line.length() == 0) 
      continue;

    contents.emplace_back(std::string(line.c_str()));
  }

  fileIn.close();
  return contents;
}