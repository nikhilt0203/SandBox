#pragma once
#include <array>

class Grid
{
public:
  static constexpr int ROWS = 8;

  static constexpr int COLS = 8;

  enum class Space
  {
    MODULE, WIRE, EMPTY, INVALID
  };

public:
  static void init();
  
  static void update();

  static void clear();

  static void updateSquare(Space state, int position);

  static void updateSquare(Space state, int row, int col);

  static Space stateAt(int position);

  static Space stateAt(int row, int col) { return stateAt(toPosition(row, col)); }

  static bool isModuleAt(int position) { return stateAt(position) == Space::MODULE; }

  static bool isModuleAt(int row, int col) { return stateAt(toPosition(row, col)) == Space::MODULE; }

  static bool isValid(int position) { return stateAt(position) != Space::INVALID;};

  static bool isValid(int row, int col) { return isValid(toPosition(row, col)); } 

  static bool isInBounds(int position) { return position >= 0 && position < ROWS * COLS; };

  static bool isInBounds(int row, int col) { return isInBounds(toPosition(row, col)); }

  static bool isInBank(int position) { return position >= 56 && position <= 63; }

  static bool isInBank(int row, int col) { return isInBank(toPosition(row, col)); }

  static int toPosition(int row, int col) { return row * COLS + col; }
};