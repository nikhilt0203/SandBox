#include "Module.h"
#include "Grid.h"

std::array<Grid::SquareState, 64> Grid::grid;

void Grid::init()
{
  for (size_t i = 0; i < grid.size() - COLS; i++)
    grid[i] = Grid::SquareState::EMPTY;

  for (size_t i = grid.size() - COLS; i < grid.size(); i++)
    grid[i] = Grid::SquareState::INVALID;
}

Grid::SquareState Grid::stateAt(int position) 
{
  return grid[position];
}

void Grid::setSquare(SquareState state, int position)
{
  Grid::grid[position] = state;
}

void Grid::setSquare(SquareState state, int row, int col)
{
  if (state == SquareState::INVALID) 
    return;
  Grid::grid[toPosition(row, col)] = state;
}

void Grid::update()
{
  for (size_t i = 0; i < grid.size() - COLS; i++)
  {
    Grid::setSquare(Grid::SquareState::EMPTY, i);
  }

  for (size_t i = grid.size() - COLS; i < grid.size(); i++)
  {
    Grid::setSquare(Grid::SquareState::INVALID, i);
  }

  for (Module* m: Module::getAllModules())
  {
    Grid::setSquare(Grid::SquareState::MODULE, m->getPosition());
  }
}

void Grid::clear() { init(); }

