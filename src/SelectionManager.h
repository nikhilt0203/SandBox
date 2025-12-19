#pragma once
#include "Module.h"
#include "Grid.h"
#include <array>
#include <stack>
#include "Timer.h"


//Init with callback
namespace SelectionManager
{
  enum class Action { SELECT_MODULE, SELECT_GRID, SELECT_BANK, PATCH, CREATE, DELETE, NONE };
  enum class Mode { EDIT, VIEW };

  static const unsigned long HOLD_THRESHOLD_MS = 500;
  static const unsigned long PATCH_TIMEOUT_MS = 1500;

  extern std::array<int, 2> selections;
  extern Mode mode;
  extern Timer patchTimer;

  //extern void (*actionCallback)(Action);

  void init();
  Action select(int position, unsigned long holdTimeMs);
  void clearSelections();
  int getCurrentSelection();
  int getPreviousSelection();
  void enterEditMode();
  void exitEditMode();
  bool isEditMode();
}

class SelectionManager2
{
public:
  enum class Action { SELECT_MODULE, SELECT_GRID, SELECT_BANK, PATCH, CREATE, DELETE, NONE };
  enum class Mode { EDIT, VIEW };

  struct Selection
  {
    Action action;
    int position;
    unsigned long holdTimeMs = 0;

    bool operator==(const Selection& other) const
    {
      return position == other.position && action == other.action;
    }
  };

  struct DoubleSelection
  {
    Selection firstSelection;
    Selection secondSelection;

    bool operator==(const DoubleSelection& other) const
    {
      return firstSelection == other.firstSelection && secondSelection == other.secondSelection;
    }
  };

  struct Command
  {
    Action firstAction;
    Action secondAction;

    bool operator==(const Command& other) const
    {
      return firstAction == other.firstAction && secondAction == other.secondAction;
    }
  };

  static constexpr Command NONE_COMMAND = { Action::NONE, Action::NONE };
  static constexpr Command PATCH_COMMAND = { Action::SELECT_MODULE, Action::SELECT_MODULE };
  static constexpr Command CREATE_COMMAND = { Action::SELECT_BANK, Action::SELECT_GRID };
  static constexpr Command SELECT_MODULE_COMMAND = { Action::SELECT_MODULE, Action::NONE };
  static constexpr Command SELECT_BANK_COMMAND = { Action::SELECT_BANK, Action::NONE };


  static const unsigned long HOLD_THRESHOLD_MS = 500;
  static const unsigned long PATCH_TIMEOUT_MS = 1500;

  std::stack<Selection> m_Selections;
  Mode m_Mode;
  Timer m_PatchTimer;

  //extern void (*actionCallback)(Action);
public:
  void init()
  {
    m_Selections = std::stack<Selection>();
    m_Mode = Mode::EDIT;
  }

  Selection select(int position, unsigned long holdTimeMs)
  {
    m_Selections.push({Action::NONE, position, holdTimeMs});
    return m_Selections.top();
  }

  void clearSelections()
  {
    while (!m_Selections.empty())
      m_Selections.pop();
  }

  Selection getCurrentSelection()
  {
    Selection current = m_Selections.top();
    m_Selections.pop();
    return current;
  }

  int getPreviousSelection()
  {
    if (m_Selections.empty())
      return -1;
    return m_Selections.top().position;
  }

  void enterEditMode()
  {
    clearSelections();
    m_Mode = Mode::EDIT;
  }

  void exitEditMode()
  {
    clearSelections();
    m_Mode = Mode::VIEW;
  }

  bool isEditMode() { return m_Mode == Mode::EDIT; };
};