#pragma once
#include "Module.h"
#include "PortManager.h"
#include <memory>

class Patch
{
public:
  AudioConnection* patchCord;
  Module* source;
  int sourcePort;
  Module* destination;
  int destinationPort;

  Patch(Module* src, int srcPort, Module* dest, int destPort)
  : source(src), 
    sourcePort(srcPort), 
    destination(dest), 
    destinationPort(destPort) 
  {
    AudioStream* srcDevice = src->getOutputDevice();
    AudioStream* destDevice = dest->getInputDevice();
    if (!srcDevice || !destDevice) 
    {
      Serial.println("Invalid source or destination device for patch: " + src->getID() + String(", ") + sourcePort + String(", ") + dest->getID() + String(", ") + destinationPort);

      patchCord = nullptr;
      return;
    }
    patchCord = new AudioConnection(*srcDevice, sourcePort, *destDevice, destinationPort);
  }

  ~Patch() 
  { 
    delete patchCord; 
    patchCord = nullptr; 
  }

  std::string toString()
  {
    //Format: Patch{sourceID,sourcePort,destinationID,destinationPort}
    std::string s = "Patch{";
    s += std::to_string(source->getID()) + "," +
        std::to_string(sourcePort) + "," +
        std::to_string(destination->getID()) + "," +
        std::to_string(destinationPort) + "}";
    return s;
  }

  static Patch* buildFromString(std::string s)
  {
    //Format: Patch{sourceID,sourcePort,destinationID,destinationPort}
    int firstComma = s.find(",");
    int secondComma = s.find(",", firstComma + 1);
    int thirdComma = s.find(",", secondComma + 1);

    int srcID =    Serialize::extractIntBetween(s.find("{"), firstComma, s);
    int srcPort =  Serialize::extractIntBetween(firstComma, secondComma, s);
    int destID =   Serialize::extractIntBetween(secondComma, thirdComma, s);
    int destPort = Serialize::extractIntBetween(thirdComma, s.find("}"), s);

    Module* src = Module::getModuleWithID(srcID);
    Module* dest = Module::getModuleWithID(destID);

    if (!src || !dest)
    {
      Serial.println("Can't create patch, source or destination is invalid");
      Serial.printf("%d, %d, %d, %d\n", srcID, srcPort, destID, destPort);
      return nullptr;
    }
  

    return new Patch(src, srcPort, dest, destPort);
  }
};