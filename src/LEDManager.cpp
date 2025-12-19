#include "LEDManager.h"
#include "Colors.h"
#include "SelectionManager.h"
#include "Pathfinder.h"
#include "Grid.h"

std::array<uint32_t, 64> colorGrid = {0};

LEDManager& LEDManager::getInstance()
{ 
  static LEDManager instance;
  return instance;
}

void LEDManager::init(Adafruit_MultiTrellis* t, Patcher* p)
{
  getInstance().patcher = p;
  getInstance().trellis = t;
}

void LEDManager::changeSquareBrightness(int position, float brightness)
{
  if (colorGrid[position] == 0x000000) return;

  static int lastPosition = -1;
  if (lastPosition == position) return;
  lastPosition = position;

  placeLed(position / 8, position % 8, 
    Colors::changeBrightness(Module::getModule(position)->getColor(), brightness));
  getInstance().trellis->show();
}

void LEDManager::placeLed(int row, int col, uint32_t color)
{
  if (row > Grid::ROWS || col > Grid::COLS || !checkTrellis()) 
    return;
  getInstance().trellis->setPixelColor(col, row, color);
  colorGrid[Grid::toPosition(row, col)] = color;
}

uint32_t getWireColor(Module* m, float brightness) { return Colors::changeBrightness(m->getColor(), brightness); }

void LEDManager::displayConnection(Module* src, Module* dest, bool show)
{
  if (!checkTrellis()) 
    return;
  if (!src || !dest) 
    return;

  uint32_t color = getWireColor(src, 0.1);

  Pathfinder::Position destPos = {dest->getCol(), dest->getRow()};
  Pathfinder::Position currentPos = {src->getCol(), src->getRow()};

  while (currentPos != destPos)
  {
    Pathfinder::Position nextPos = Pathfinder::findBestMove({currentPos.x, currentPos.y}, destPos);
    if (!nextPos)
      break;
    if (nextPos.x == -1 || nextPos.y == -1)
      break;
    if (nextPos == destPos) 
      break;
    if (!Grid::isValid(nextPos.y, nextPos.x))
      break;

    int rowIndex = nextPos.y;
    int colIndex = nextPos.x;

    Grid::SquareState square = Grid::stateAt(rowIndex, colIndex);
    if (square == Grid::SquareState::EMPTY || square == Grid::SquareState::WIRE)
    {
      placeLed(rowIndex, colIndex, color);
      Grid::setSquare(Grid::SquareState::WIRE, rowIndex, colIndex);
    }
    currentPos = nextPos;
  }
  // while (rowIndex < dRow)
  // {
  //   rowIndex++;
  //   if (rowIndex == dRow && colIndex == dCol) break;
  //   if (!Grid::isModuleAt(rowIndex, colIndex))
  //   {
  //     placeLed(rowIndex, colIndex, color);
  //     Grid::setSquare(Grid::SquareState::WIRE, rowIndex, colIndex);
  //     if (Grid::stateAt(rowIndex, colIndex) == Grid::SquareState::WIRE)
  //     {
  //       Serial.println("Set square to Wire");
  //     }
  //   }
  // }

  // while (rowIndex > dRow)
  // {
  //   rowIndex--;
  //   if (rowIndex == dRow && colIndex == dCol) break;
  //   if (!Grid::isModuleAt(rowIndex, colIndex))
  //   {
  //     placeLed(rowIndex, colIndex, color);
  //     Grid::setSquare(Grid::SquareState::WIRE, rowIndex, colIndex);
  //     if (Grid::stateAt(rowIndex, colIndex) == Grid::SquareState::WIRE)
  //     {
  //       Serial.println("Set square to Wire");
  //     }
  //   }
  // }

  // while (colIndex < dCol)
  // {
  //   colIndex++;
  //   if (rowIndex == dRow && colIndex == dCol) break;
  //   if (!Grid::isModuleAt(rowIndex, colIndex))
  //   {
  //     placeLed(rowIndex, colIndex, color);
  //     Grid::setSquare(Grid::SquareState::WIRE, rowIndex, colIndex);
  //     if (Grid::stateAt(rowIndex, colIndex) == Grid::SquareState::WIRE)
  //     {
  //       Serial.println("Set square to Wire");
  //     }
  //   }
  // }

  // while (colIndex > dCol)
  // {
  //   colIndex--;
  //   if (rowIndex == dRow && colIndex == dCol) break;
  //   if (!Grid::isModuleAt(rowIndex, colIndex))
  //   {
  //     placeLed(rowIndex, colIndex, color);
  //     Grid::setSquare(Grid::SquareState::WIRE, rowIndex, colIndex);
  //     if (Grid::stateAt(rowIndex, colIndex) == Grid::SquareState::WIRE)
  //     {
  //       Serial.println("Set square to Wire");
  //     }
  //   }
  // }

  if (show) getInstance().trellis->show();
}

void LEDManager::refreshBank(bool show)
{
  if (!checkTrellis() || !checkPatcher()) return;

  if (SelectionManager::isEditMode())
  {
    for (int i = 0; i < 8; i++)
    {
      placeLed(7, i, Colors::getColor(ModuleBuilder::bankType(i)));
    }
  }
  else
  {
    for (int i = 0; i < 8; i++)
    {
      placeLed(7, i, 0x000000);
    }
  }
  if (show) getInstance().trellis->show();
}

void LEDManager::clearGrid(bool show)
{
  if (!checkTrellis()) return;

  for (int row = 0; row < Grid::ROWS; row++)
  {
    for (int col = 0; col < Grid::COLS; col++)
    {
      placeLed(row, col, 0x000000);
    }
  }
  if (show) getInstance().trellis->show();
}

void LEDManager::refreshGrid(Module* m)
{
  if (!checkTrellis() || !checkPatcher()) return;

  if (m)
  {
    placeLed(m->getRow(), m->getCol(), m->getColor());
    getInstance().trellis->show();
    return;
  }

  //clear grid
  for (int row = 0; row < Grid::ROWS; row++)
  {
    for (int col = 0; col < Grid::COLS; col++)
    {
      placeLed(row, col, 0x000000);
    }
  }

  //Update bank
  if (SelectionManager::isEditMode())
  {
    for (int i = 0; i < 8; i++)
    {
      placeLed(7, i, Colors::getColor(ModuleBuilder::bankType(i)));
    }
  }

  //Modules
  for (Module* m : Module::allModules()) 
  {
    placeLed(m->getRow(), m->getCol(), m->getColor());
    Grid::setSquare(Grid::SquareState::MODULE, m->getRow(), m->getCol());
  }

  Grid::update();

  //Connections
  for (Patch* p : Patcher::getAllPatches()) 
  {
    displayConnection(p->source, p->destination, false);
  }

  getInstance().trellis->show();
}

void LEDManager::placeModule(Module* m)
{
  if (!m)
  {
    Serial.println("Error: Cannot place module as it does not exist.");
    return;
  }
  placeLed(m->getRow(), m->getCol(), m->getColor());
  Grid::update();
}

// void LEDManager::playPatchAnimation(Module* src, Module* dest) 
// {
//   clearGrid();
//   placeLed(src->getRow(), src->getCol(), Colors::changeBrightness(src->getColor(), 0.5));
//   placeLed(dest->getRow(), dest->getCol(), Colors::changeBrightness(dest->getColor(), 0.5));
// }