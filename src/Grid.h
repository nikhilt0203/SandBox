#pragma once
#include <array>

class Grid
{
public:
  static constexpr int ROWS = 8;
  static constexpr int COLS = 8;

  enum class SquareState
  {
    MODULE, WIRE, EMPTY, INVALID
  };

  static std::array<SquareState, 64> grid;

  static void init();
  
  static void update();

  static void clear();

  static void setSquare(SquareState state, int position);

  static void setSquare(SquareState state, int row, int col);

  static SquareState stateAt(int position);

  static SquareState stateAt(int row, int col) { return stateAt(toPosition(row, col)); }

  static bool isModuleAt(int position) { return stateAt(position) == SquareState::MODULE; }

  static bool isModuleAt(int row, int col) { return stateAt(toPosition(row, col)) == SquareState::MODULE; }

  static bool isValid(int position) { return stateAt(position) != SquareState::INVALID;};

  static bool isValid(int row, int col) { return isValid(toPosition(row, col)); } 

  static bool isInBank(int position) { return position >= 56 && position <= 63; }

  static bool isInBank(int row, int col) { return isInBank(toPosition(row, col)); }

  static int toPosition(int row, int col) { return row * COLS + col; }
};