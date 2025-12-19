#pragma once
#include "Module.h"
#include "Grid.h"
#include <array>
#include <stack>
#include "Timer.h"

namespace SelectionHandler
{
  enum class Action 
  { 
    SELECT_MODULE, 
    SELECT_GRID, 
    SELECT_BANK, 
    PATCH, 
    CREATE, 
    DELETE, 
    NONE 
  };

  enum class Mode 
  {
    EDIT, VIEW 
  };

  static constexpr unsigned int HOLD_THRESHOLD_MS = 500;
  static constexpr unsigned int PATCH_TIMEOUT_MS = 1500;

  void init();

  Action select(int position, unsigned int holdTimeMs);

  void clearSelections();

  int currentSelection();

  int previousSelection();

  void enterEditMode();

  void exitEditMode();

  bool isEditMode();

  void lock();

  void unlock();

  bool isLocked();
}
