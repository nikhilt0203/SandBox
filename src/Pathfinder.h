#include <Arduino.h>
#include "Grid.h"

namespace Pathfinder
{
  struct Position
  { 
    int x;
    int y; 

    inline bool operator== (const Position& other) { return other.x == x && other.y == y; }

    inline bool operator!= (const Position& other) { return other.x != x || other.y != y; }

    inline bool operator! () { return x == -1 && y == -1; }
  };

  struct RankedNextMove 
  { 
    int rank; 
    Position nextPosition;

    inline bool operator>(const RankedNextMove& other) { return rank > other.rank; }

    inline bool operator<(const RankedNextMove& other) { return rank < other.rank; }
  };

  inline float distance(Position a, Position b) { return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2)); }

  inline bool isMoveBeneficial(Position startPosition, Position nextPosition, Position endPosition)
  {
    if (!Grid::isValid(startPosition.y, startPosition.x) ||
        !Grid::isValid(nextPosition.y, nextPosition.x) ||
        !Grid::isValid(endPosition.y, endPosition.x))
      return false;

    if (distance(nextPosition, endPosition) <= distance(startPosition, endPosition))
      return true;
    else if (abs(nextPosition.x - endPosition.x) < abs(startPosition.x - endPosition.x))
      return true;
    else if (abs(nextPosition.y - endPosition.y) < abs(startPosition.y - endPosition.y))
      return true;

    return false;
  }

  inline Position findBestMove(Position startPosition, Position endPosition)
  {
    static std::vector<RankedNextMove> rankedNextMoves(4);
    rankedNextMoves.clear();
    rankedNextMoves.reserve(4);

    std::array<Position, 4> neighbors{{
      {startPosition.x - 1, startPosition.y},
      {startPosition.x + 1, startPosition.y},
      {startPosition.x, startPosition.y + 1},
      {startPosition.x, startPosition.y - 1}
    }};

    for (auto& neighbor: neighbors) 
    {
      if (isMoveBeneficial(startPosition, neighbor, endPosition))
      {
        Grid::SquareState neighborSquare = Grid::stateAt(Grid::toPosition(neighbor.y, neighbor.x));
        
        if (neighborSquare == Grid::SquareState::EMPTY) 
          rankedNextMoves.push_back({0, neighbor});

        if (neighborSquare == Grid::SquareState::WIRE) 
          rankedNextMoves.push_back({1, neighbor});

        if (neighborSquare == Grid::SquareState::MODULE)
          rankedNextMoves.push_back({2, neighbor});

        if (neighborSquare == Grid::SquareState::INVALID)
          continue;
      }
    }

    if (rankedNextMoves.empty()) 
      return {-1, -1};

    auto bestMove = std::min_element(rankedNextMoves.begin(), rankedNextMoves.end());

    return bestMove->nextPosition;
  }
}