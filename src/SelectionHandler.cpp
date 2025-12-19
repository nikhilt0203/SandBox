#include "SelectionHandler.h"
#include "LEDManager.h"
#include "Grid.h"

namespace SelectionHandler 
{
  static std::array<int, 2> m_Selections;
  Mode m_Mode;
  Timer patchTimer{};
  bool m_Locked;
  Module* m_LockedModule;
}

void SelectionHandler::init()
{
  m_Selections = {-1, -1};
  m_Mode = Mode::EDIT;
  m_Locked = false;
  m_LockedModule = nullptr;
}

void SelectionHandler::clearSelections() 
{ 
  SelectionHandler::m_Selections.fill(-1); 
}

SelectionHandler::Action handleFirstSelection(int position)
{
  SelectionHandler::clearSelections();
  SelectionHandler::m_Selections[0] = position;
  SelectionHandler::m_Selections[1] = -1;

  Serial.printf("[%d, %d]\n", SelectionHandler::m_Selections[0], SelectionHandler::m_Selections[1]);

  if (Grid::isModuleAt(position))
  {
    SelectionHandler::patchTimer.start();
    return SelectionHandler::Action::SELECT_MODULE;
  }

  return Grid::isInBank(position) ? SelectionHandler::Action::SELECT_BANK : SelectionHandler::Action::NONE;
}

SelectionHandler::Action handleSecondSelection(int position)
{
  int currentSelectionPosition = SelectionHandler::m_Selections[0];
  if (position == currentSelectionPosition) 
  { 
    return handleFirstSelection(position); 
  }

  SelectionHandler::m_Selections[1] = position;

  //Serial.printf("[%d, %d]\n", SelectionHandler::m_Selections[0], SelectionHandler::m_Selections[1]);

  if (Grid::isInBank(currentSelectionPosition) && Grid::isInBank(position))
  {
    return handleFirstSelection(position);
  }

  if (Grid::isInBank(currentSelectionPosition) && !Grid::isModuleAt(position)) 
  {
    return SelectionHandler::Action::CREATE;
  }

  if (Grid::isModuleAt(currentSelectionPosition) && Grid::isModuleAt(position)) 
  {
    if (!SelectionHandler::patchTimer.hasReached(SelectionHandler::PATCH_TIMEOUT_MS)) 
    {
      return SelectionHandler::Action::PATCH;
    }
  }

  return handleFirstSelection(position);
}

SelectionHandler::Action handleLongPress(int position)
{
  if (Grid::isModuleAt(position))
  {
    SelectionHandler::m_Selections[0] = position;
    SelectionHandler::m_Selections[1] = -1;
    return SelectionHandler::Action::DELETE;
  }

  SelectionHandler::clearSelections();
  return SelectionHandler::Action::NONE;
}

SelectionHandler::Action SelectionHandler::select(int position, unsigned int holdTimeMs)
{
  if (m_Mode == Mode::VIEW)
  {
    clearSelections();
    m_Selections[0] = position;
    return Action::NONE;
  }

  if (holdTimeMs > HOLD_THRESHOLD_MS) 
  { 
    return handleLongPress(position); 
  }

  if (m_Selections[0] == -1) 
  { 
    return handleFirstSelection(position); 
  }

  if (m_Selections[1] == -1) 
  { 
    return handleSecondSelection(position); 
  }

  if (m_Selections[0] != -1 && m_Selections[1] != -1)
  {
    clearSelections();
    return handleFirstSelection(position);
  }

  return Action::NONE;
}

int SelectionHandler::currentSelection() 
{ 
  return (m_Selections[1] != -1) ? m_Selections[1] : m_Selections[0]; 
}

void SelectionHandler::enterEditMode() 
{
  clearSelections();
  m_Mode = Mode::EDIT; 
}

void SelectionHandler::exitEditMode() 
{ 
  clearSelections();
  m_Mode = Mode::VIEW; 
}

int SelectionHandler::previousSelection() 
{ 
  return m_Selections[0]; 
}

bool SelectionHandler::isEditMode() 
{ 
  return m_Mode == Mode::EDIT; 
}

void SelectionHandler::lock()
{
  m_Locked = true;
}

void SelectionHandler::unlock()
{
  m_Locked = false;
}

bool SelectionHandler::isLocked()
{
  return m_Locked;
}
