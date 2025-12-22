#include <Arduino.h>
#include "Grid.h"
#include <set>
#pragma once

// Determines wire path for connecting modules on the grid
namespace Pathfinder
{
  struct Position
  { 
    int row{};
    int col{};

    inline bool operator==(const Position& other) { return other.row == row && other.col == col; }

    inline bool operator!=(const Position& other) { return other.row != row || other.col != col; }

    inline bool operator!() { return row == -1 && col == -1; }

    inline bool operator<(const Position& other) const { return (row != other.row) ? (row < other.row) : (col < other.col); }
  };

  struct RankedPosition
  { 
    Position position;
    int rank;

    inline bool operator>(const RankedPosition& other) { return rank > other.rank; } 

    inline bool operator<(const RankedPosition& other) { return rank < other.rank; }
  };

  inline float distance(Position a, Position b) { return hypotf(a.col - b.col, a.row - b.row); }

  inline bool isMoveBeneficial(Position startPosition, Position nextPosition, Position endPosition)
  {
    if (!Grid::isValid(startPosition.row, startPosition.col) || 
        !Grid::isValid(nextPosition.row, nextPosition.col) || 
        !Grid::isValid(endPosition.row, endPosition.col)) 
    {
      return false;
    }

    //if the next move is closer to the end position
    if (distance(nextPosition, endPosition) <= distance(startPosition, endPosition))
    {
      return true;
    }

    return false;
  }

  inline Position findBestMove(Position startPosition, Position endPosition)
  {
    static std::vector<RankedPosition> rankedPositions;
    rankedPositions.clear();
    rankedPositions.reserve(4);

    std::array<Position, 4> neighbors{{
      {startPosition.row - 1, startPosition.col},
      {startPosition.row, startPosition.col + 1},
      {startPosition.row + 1, startPosition.col},
      {startPosition.row, startPosition.col - 1}
    }};

    for (auto& neighbor: neighbors) 
    {
      if (isMoveBeneficial(startPosition, neighbor, endPosition))
      {
        Grid::Space neighborSquare = Grid::stateAt(neighbor.row, neighbor.col);

        if (neighborSquare == Grid::Space::EMPTY) { 
          rankedPositions.push_back({neighbor, 0}); 
        }
        if (neighborSquare == Grid::Space::WIRE) { 
          rankedPositions.push_back({neighbor, 1}); 
        }
        if (neighborSquare == Grid::Space::MODULE) { 
          rankedPositions.push_back({neighbor, 2}); 
        }
        if (neighborSquare == Grid::Space::INVALID) { 
          continue; 
        }
      }
    }

    if (rankedPositions.empty()) { 
      return {-1, -1};
    }

    auto bestMove = std::min_element(rankedPositions.begin(), rankedPositions.end());
    return bestMove->position;
  }

  std::vector<Position> findShortestPath(Position start, Position end)
  {
    //BFS implementation
    std::queue<std::pair<Position, std::vector<Position>>> queue;
    std::set<Position> visited;

    static constexpr size_t MAX_PATH_LENGTH = 32;

    static const std::array<Position, 4> directions{{
      {0, -1},
      {0,  1},
      {-1, 0},
      {1,  0}
    }};

    queue.push({start, {start}});

    while (!queue.empty())
    {
      auto [position, path] = queue.front();
      queue.pop();

      if (!Grid::isValid(position.row, position.col) || visited.count(position) > 0) {
        continue;
      }

      if (position != start && position != end &&
        Grid::stateAt(position.row, position.col) == Grid::Space::MODULE) {
        continue;
      }

      if (position == end) {
        return path;
      }

      if (position != start) {
        visited.insert(position);
      }

      if (path.size() >= MAX_PATH_LENGTH) {
        continue;
      }

      for (const Position& dir : directions)
      {
        Position neighbor{position.row + dir.row, position.col + dir.col};

        auto nextPath = path;
        nextPath.push_back(neighbor);
        queue.push({neighbor, std::move(nextPath)});
      }
    }

    return {};
  }
}
