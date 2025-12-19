#include <Audio.h>
#include "Module.h"
#include "Patch.h"
#include <vector>
#pragma once

class Patcher
{
public:
  Patcher() = default;

  bool connect(Module* src, Module* dest);
    
  bool disconnect(Module* src, Module* dest);

  void connectionHandler(Module* src, Module* dest);
  
  bool isConnected(Module* src, Module* dest) const;

  bool patchExists(Module* src, Module* dest) const { return isConnected(src, dest) || isConnected(dest, src); };

  void deleteAllPatchesWith(Module* m);

  void registerNewPatch(Patch* p);

  static const std::vector<Patch*>& getAllPatches();

  static void deleteAllPatches();
};