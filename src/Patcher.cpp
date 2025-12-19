#include "Patcher.h"
#include "DisplayManager.h"
#include "UIElements.h"

static std::vector<Patch*> allPatches{};

bool Patcher::connect(Module *src, Module *dest)
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
    return false;
  }

  if (src->outputsFull())
  {
    return false;
  }

  Port srcPort = src->getOpenOutputPort();
  Port destPort = dest->getOpenInputPort();

  if (!srcPort.isFree() || !destPort.isFree())
  {
    Serial.println("No available ports to connect the modules.");
    return false;
  }

  DisplayManager::draw<PatchDisplayPage>(src, srcPort.m_ModulePortNum, dest, destPort.m_ModulePortNum, true);

  allPatches.push_back(new Patch(src, srcPort, dest, destPort));
  src->addDestination(dest);
  dest->addSource(src);
  
  return true;
}

bool Patcher::disconnect(Module *src, Module *dest)
{
  if (!src || !dest)
  {
    Serial.println("Source or destination does not exist.");
    return false;
  }

  if (!isConnected(src, dest))
  {
    return false;
  }
                                                                
  for (auto it = allPatches.begin(); it != allPatches.end(); ++it)
  {
    if ((*it)->source == src && (*it)->destination == dest)
    {
      src->disconnectFrom(dest);
      dest->disconnectFrom(src);

      DisplayManager::draw<PatchDisplayPage>(
          src, (*it)->deviceSourcePort, 
          dest, (*it)->deviceDestinationPort, false);

      delete *it;
      allPatches.erase(it);
      return true;
    }
  }
  return false;
}

void Patcher::connectionHandler(Module *src, Module *dest)
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
  {
    disconnect(src, dest);
  }
  else if (isConnected(dest, src))
  {
    disconnect(dest, src);
  }
  else
  {
    connect(src, dest);
  }
}

bool Patcher::isConnected(Module* src, Module* dest) const
{
  if (!src || !dest)
  {
    Serial.println("Source or destination does not exist.");
    return false;
  }

  for (Patch* p : allPatches)
  {
    if (p->source == src && p->destination == dest)
    {
      return true;
    }
  }
  return false;
}

void Patcher::deleteAllPatchesWith(Module* m)
{
  if (!m)
  {
    Serial.println("Module does not exist.");
    return;
  }

  for (auto it = allPatches.begin(); it != allPatches.end();)
  {
    Patch* patch = *it;

    if (!patch)
    {
      continue;
    }

    Module* other = nullptr;

    if (patch->source == m)
    {
      other = patch->destination;
    }
    else if (patch->destination == m)
    {
      other = patch->source;
    }

    if (other)
    {
      other->disconnectFrom(m);
    }
    else
    {
      it++;
      continue;
    }

    delete *it;
    it = allPatches.erase(it);
  }
}

void Patcher::registerNewPatch(Patch* p)
{
  p->source->addDestination(p->destination);
  p->destination->addSource(p->source);
  allPatches.push_back(p);
}

const std::vector<Patch*>& Patcher::getAllPatches()
{
  return allPatches;
}

void Patcher::deleteAllPatches()
{
  for (Patch* p : allPatches)
  {
    if (!p)
    {
      continue;
    }
    if (p->source)
    {
      p->source->disconnectFrom(p->destination);
    }
    if (p->destination)
    {
      p->destination->disconnectFrom(p->source);
    }
    delete p;
  }

  allPatches.clear();
}