#include "Module.h"
#include "Grid.h"

static std::array<Grid::SquareState, 64> s_Grid;

void Grid::init()
{
  for (size_t i{}; i < s_Grid.size() - COLS; i++)
  {
    s_Grid[i] = Grid::SquareState::EMPTY;
  }

  for (size_t i = s_Grid.size() - COLS; i < s_Grid.size(); i++)
  {
    s_Grid[i] = Grid::SquareState::INVALID;
  }
}

Grid::SquareState Grid::stateAt(int position) 
{
  return s_Grid[position];
}

void Grid::updateSquare(SquareState state, int position)
{
  s_Grid[position] = state;
}

void Grid::updateSquare(SquareState state, int row, int col)
{
  if (state == SquareState::INVALID) { 
    return; 
  }
  s_Grid[toPosition(row, col)] = state;
}

void Grid::update()
{
  for (size_t i{}; i < s_Grid.size() - COLS; i++)
  {
    Grid::updateSquare(Grid::SquareState::EMPTY, i);
  }

  for (size_t i{s_Grid.size() - COLS}; i < s_Grid.size(); i++)
  {
    Grid::updateSquare(Grid::SquareState::INVALID, i);
  }

  for (Module* m: Module::getAllModules())
  {
    if (!m) 
    { 
      continue; 
    }
    Grid::updateSquare(Grid::SquareState::MODULE, m->getLEDElement().row, m->getLEDElement().col);
  }
}

void Grid::clear() 
{ 
  init(); 
}

