#include "Module.h"
#include "Grid.h"

static std::array<Grid::Space, 64> s_Grid;

void Grid::init()
{
  for (size_t i{}; i < s_Grid.size() - COLS; i++)
  {
    s_Grid[i] = Grid::Space::EMPTY;
  }

  for (size_t i = s_Grid.size() - COLS; i < s_Grid.size(); i++)
  {
    s_Grid[i] = Grid::Space::INVALID;
  }
}

Grid::Space Grid::stateAt(int position) 
{
  return s_Grid[position];
}

void Grid::updateSquare(Space state, int position)
{
  s_Grid[position] = state;
}

void Grid::updateSquare(Space state, int row, int col)
{
  if (state == Space::INVALID) { 
    return; 
  }
  s_Grid[toPosition(row, col)] = state;
}

void Grid::update()
{
  for (size_t i{}; i < s_Grid.size() - COLS; i++)
  {
    Grid::updateSquare(Grid::Space::EMPTY, i);
  }

  for (size_t i{s_Grid.size() - COLS}; i < s_Grid.size(); i++)
  {
    Grid::updateSquare(Grid::Space::INVALID, i);
  }

  for (Module* m: Module::getAllModules())
  {
    if (!m) 
    { 
      continue; 
    }
    Grid::updateSquare(Grid::Space::MODULE, m->getLEDElement().row, m->getLEDElement().col);
  }
}

void Grid::clear() 
{ 
  init(); 
}

