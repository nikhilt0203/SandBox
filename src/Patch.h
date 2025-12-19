#pragma once
#include "Module.h"
#include "PortManager.h"
#include <memory>

class Patch
{
public:
  std::unique_ptr<AudioConnection> patchCord;
  Module* source;
  int deviceSourcePort;
  int moduleSourcePort;
  Module* destination;
  int deviceDestinationPort;
  int moduleDestinationPort;
  std::string name{};

  Patch(Module* src, Port srcPort, Module* dest, Port destPort)
  : source(src), 
    deviceSourcePort(srcPort.m_DevicePortNum), 
    moduleSourcePort(srcPort.m_ModulePortNum),
    destination(dest), 
    deviceDestinationPort(destPort.m_DevicePortNum),
    moduleDestinationPort(destPort.m_ModulePortNum)
  {
    AudioStream* srcDevice = src->getOutputDevice();
    AudioStream* destDevice = dest->getInputDevice();

    if (!srcPort.isFree() || !destPort.isFree()) 
    {
      Serial.println("Invalid source or destination port for patch: " + String(src->getID()) + String(", ") + String(deviceSourcePort) + String(", ") + String(dest->getID()) + String(", ") + String(deviceDestinationPort));
      patchCord = nullptr;
      return;
    }

    if (!srcDevice) 
    {
      Serial.println("Invalid source device for patch: " + String(src->getID()) + String(", ") + String(deviceSourcePort) );
      patchCord = nullptr;
      return;
    }

    if (!destDevice) 
    {
      Serial.println("Invalid destination device for patch: " + String(dest->getID()) + String(", ") + String(deviceDestinationPort));
      patchCord = nullptr;
      return;
    }

    Serial.printf("Creating patch from module %s port %d to module %s port %d\n", src->getName().data(), srcPort.m_ModulePortNum, dest->getName().data(), destPort.m_ModulePortNum);
    AudioNoInterrupts();
    patchCord = std::make_unique<AudioConnection>(*srcDevice, deviceSourcePort, *destDevice, deviceDestinationPort);
    AudioInterrupts();
  }

  Patch(Module* src, int srcPort, Module* dest, int destPort)
  : source(src), 
    deviceSourcePort(srcPort), 
    destination(dest), 
    deviceDestinationPort(destPort)
  {
    AudioStream* srcDevice = src->getOutputDevice();
    AudioStream* destDevice = dest->getInputDevice();

    if (srcPort == -1 || destPort == -1) 
    {
      Serial.println("Invalid source or destination port for patch: " + String(src->getID()) + String(", ") + String(deviceSourcePort) + String(", ") + String(dest->getID()) + String(", ") + String(deviceDestinationPort));
      patchCord = nullptr;
      return;
    }

    if (!srcDevice) 
    {
      Serial.println("Invalid source device for patch: " + String(src->getID()) + String(", ") + String(deviceSourcePort) );
      patchCord = nullptr;
      return;
    }

    if (!destDevice) 
    {
      Serial.println("Invalid destination device for patch: " + String(dest->getID()) + String(", ") + String(deviceDestinationPort));
      patchCord = nullptr;
      return;
    }
    
    Serial.printf("Creating patch from module %s port %d to module %s port %d\n", src->getName().data(), srcPort, dest->getName().data(), destPort);
    AudioNoInterrupts();
    patchCord = std::make_unique<AudioConnection>(*srcDevice, deviceSourcePort, *destDevice, deviceDestinationPort);
    AudioInterrupts();
  }

  std::string_view serialize()
  {
    //Format: Patch{sourceID,deviceSourcePort,destinationID,deviceDestinationPort}
    name = "Patch{";
    name += std::to_string(source->getID()) + "," +
        std::to_string(deviceSourcePort) + "," +
        std::to_string(destination->getID()) + "," +
        std::to_string(deviceDestinationPort) + "}";
    return name;
  }

  static Patch* buildFromString(std::string_view s)
  {
    //Format: Patch{sourceID,deviceSourcePort,destinationID,deviceDestinationPort}
    int firstComma = s.find(",");
    int secondComma = s.find(",", firstComma + 1);
    int thirdComma = s.find(",", secondComma + 1);

    int srcID =    Serialize::extractIntBetween(s.find("{"), firstComma, s);
    int srcPort =  Serialize::extractIntBetween(firstComma, secondComma, s);
    int destID =   Serialize::extractIntBetween(secondComma, thirdComma, s);
    int destPort = Serialize::extractIntBetween(thirdComma, s.find("}"), s);

    Module* src = Module::getModule(srcID);
    Module* dest = Module::getModule(destID);

    if (!src || !dest)
    {
      Serial.println("Cant create patch, source or destination is invalid");
      Serial.printf("%d, %d, %d, %d\n", srcID, srcPort, destID, destPort);
      return nullptr;
    }
  

    return new Patch(src, srcPort, dest, destPort);
  }
};