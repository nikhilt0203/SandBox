#include <Audio.h>
#include "Module.h"
#include "Patch.h"
#include <vector>
#pragma once

class Patcher
{
private:
  static std::vector<Patch*> allPatches;
public:
  Patcher() {}

  bool connect(Module* src, Module* dest)
  {
    if (!src || !dest) 
    {
      Serial.println("Source or destination does not exist.");
      return false;
    }

    if (src == dest) 
    {
      Serial.println("Cannot connect a module to itself.");
      return false;
    }
    
    if (isConnected(src, dest))
    {
      Serial.println("Modules are already connected.");
      return false;
    }
    
    if (dest->inputsFull())
    {
      Serial.println("Destination inputs are full. Disconnecting its last connection.");
      disconnectLast(dest);
    }

    if (src->outputsFull())
      return false;


    allPatches.push_back(new Patch(src, src->getOpenOutputPort(), dest, dest->getOpenInputPort()));
    DisplayManager::print({ {src->getColor(), src->getName() + " "}, 
                            {0xFFFFFF, "--> "},
                            {dest->getColor(), dest->getName()} }, 2);

    src->addDestination(dest);
    dest->addSource(src);
    return true;
  }
    
  bool disconnect(Module* src, Module* dest)
  {
    if (!src || !dest) 
    {
      Serial.println("Source or destination does not exist.");
      return false;
    }

    if (!isConnected(src, dest)) 
      return false;
    
    DisplayManager::print({ {src->getColor(), src->getName() + " "}, 
                            {0xFFFFFF, "\t-/-> "},
                            {dest->getColor(), dest->getName()} }, 4);
    
    for (auto it = allPatches.begin(); it != allPatches.end(); ++it)
    {
      if ((*it)->source == src && (*it)->destination == dest)
      {
        src->disconnectFrom(dest);
        dest->disconnectFrom(src);
        delete *it;
        allPatches.erase(it);
        return true;
      }
    }
    return false;
  }

  void connectionHandler(Module* src, Module* dest)
  {
    if (!src || !dest) 
    {
      Serial.println("Source or destination does not exist.");
      return;
    }

    if (src == dest) 
    {
      Serial.println("Source and destination are the same module.");
      return;
    }

    if (isConnected(src, dest))
      disconnect(src, dest);
    else if (isConnected(dest, src))
      disconnect(dest, src);
    else
      connect(src, dest);
  }
  
  //Disconnect the last patch in the allPatches list 
  void disconnectLast(Module* dest)
  {
    if (!dest) 
    {
      Serial.println("disconnectLast(): Destination does not exist.");
      return;
    }

    if (allPatches.empty()) 
      return;
    
    for (int i = allPatches.size() - 1; i >= 0; --i)
    {
      auto& p = allPatches[i];
      if (p->destination == dest)
      {
        if (p->source) 
          p->source->disconnectFrom(p->destination);
        if (p->destination) 
          p->destination->disconnectFrom(p->source);
        delete p;
        allPatches.erase(allPatches.begin() + i);
        return;
      }
    }
  }
  
  bool isConnected(Module* src, Module* dest) const
  {
    if (!src || !dest) 
    {
      Serial.println("Source or destination does not exist.");
      return false;
    }

    for (auto& p : allPatches)
    {
      if (p->source == src && p->destination == dest)
        return true;
    }
    return false;
  }

  bool patchExists(Module* src, Module* dest) const
  {
    if (!src || !dest) 
    {
      Serial.println("Source or destination does not exist.");
      return false;
    }
    return isConnected(src, dest) || isConnected(dest, src);
  }

  void deletePatchesWith(Module* m)
  {
    if (!m) 
    {
      Serial.println("Module does not exist.");
      return;
    }
    
    for (auto it = allPatches.begin(); it != allPatches.end();)
    {
      Patch* patch = *it;
      if (patch->source == m || patch->destination == m)
      {
        Module* other = (patch->source == m) ? patch->destination : patch->source;
        if (other)
          other->disconnectFrom(m);
        delete *it;
        it = allPatches.erase(it);
        continue;
      }
      ++it;
    }
  }

  void registerNewPatch(Patch* p)
  {
    p->source->addDestination(p->destination);
    p->destination->addSource(p->source);
    allPatches.push_back(p);
  }

  static const std::vector<Patch*>& getAllPatches()
  {
    return allPatches;
  }

  static void deleteAllPatches()
  {
    for (Patch* p: allPatches)
    {
      if (!p) continue;
      if (p->source) 
        p->source->disconnectFrom(p->destination);
      if (p->destination) 
        p->destination->disconnectFrom(p->source);
      delete p;
    }
    allPatches.clear();
  }
};