#include "SelectionManager.h"
#include "LEDManager.h"
#include "Grid.h"

namespace SelectionManager 
{
  std::array<int, 2> m_Selections = {-1, -1};
  Mode m_Mode = Mode::VIEW;
  Timer patchTimer;
}

void SelectionManager::init()
{
  m_Selections = {-1, -1};
  m_Mode = Mode::EDIT;
  patchTimer.reset();
}

void SelectionManager::clearSelections() { SelectionManager::m_Selections.fill(-1); }

SelectionManager::Action handleFirstSelection(int position)
{
  SelectionManager::clearSelections();
  SelectionManager::m_Selections[0] = position;
  SelectionManager::m_Selections[1] = -1;

  Serial.printf("[%d, %d]\n", SelectionManager::m_Selections[0], SelectionManager::m_Selections[1]);

  if (Grid::isModuleAt(position))
  {
    SelectionManager::patchTimer.reset();
    return SelectionManager::Action::SELECT_MODULE;
  }

  if (Grid::isInBank(position))
    return SelectionManager::Action::SELECT_BANK;
  else
    return SelectionManager::Action::NONE;
}

SelectionManager::Action handleSecondSelection(int position)
{
  if (position == SelectionManager::m_Selections[0])
    return handleFirstSelection(position);

  SelectionManager::m_Selections[1] = position;

  Serial.printf("[%d, %d]\n", SelectionManager::m_Selections[0], SelectionManager::m_Selections[1]);

  if (Grid::isInBank(SelectionManager::m_Selections[0]) && !Grid::isModuleAt(position))
    return SelectionManager::Action::CREATE;

  if (Grid::isModuleAt(SelectionManager::m_Selections[0]) && Grid::isModuleAt(position))
  {
    if (!SelectionManager::patchTimer.hasReached(SelectionManager::PATCH_TIMEOUT_MS))
      return SelectionManager::Action::PATCH;
  }

  if (Grid::isInBank(SelectionManager::m_Selections[0]) && Grid::isInBank(position))
  {
    Serial.println("Both selections are in bank. Resetting selection.");
    return handleFirstSelection(position);

  }

  return handleFirstSelection(position);
}

SelectionManager::Action handleLongPress(int position)
{
  if (Grid::isModuleAt(position))
  {
    SelectionManager::m_Selections[0] = position;
    SelectionManager::m_Selections[1] = -1;
    return SelectionManager::Action::DELETE;
  }
  SelectionManager::clearSelections();
  return SelectionManager::Action::NONE;
}

SelectionManager::Action SelectionManager::select(int position, unsigned long holdTimeMs)
{
  if (m_Mode == Mode::VIEW)
  {
    clearSelections();
    m_Selections[0] = position;
    return Action::NONE;
  }

  if (holdTimeMs > HOLD_THRESHOLD_MS)
    return handleLongPress(position);

  if (m_Selections[0] == -1)
    return handleFirstSelection(position);

  if (m_Selections[1] == -1)
    return handleSecondSelection(position);

  if (m_Selections[0] != -1 && m_Selections[1] != -1)
  {
    clearSelections();
    return handleFirstSelection(position);
  }

  return Action::NONE;
}

int SelectionManager::getCurrentSelection() 
{ 
  return (m_Selections[1] != -1) ? m_Selections[1] : m_Selections[0]; 
}

void SelectionManager::enterEditMode() 
{
  clearSelections();
  m_Mode = Mode::EDIT; 
}

void SelectionManager::exitEditMode() 
{ 
  clearSelections();
  m_Mode = Mode::VIEW; 
}

int SelectionManager::getPreviousSelection() { return m_Selections[0]; }

bool SelectionManager::isEditMode() { return m_Mode == Mode::EDIT; }