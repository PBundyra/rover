#ifndef DIRECTIONS_H
#define DIRECTIONS_H

#include <unordered_map>
#include <string>

// Enum used in RoverState class
enum class Direction {
    SOUTH,
    WEST,
    NORTH,
    EAST
};

// Map that enables convenient printing state of a rover
static const std::unordered_map<Direction, std::string> print_dir =
        {{Direction::SOUTH, " SOUTH"},
         {Direction::WEST,  " WEST"},
         {Direction::NORTH, " NORTH"},
         {Direction::EAST,  " EAST"}};

using coordinate_t = int32_t;
using coordinates_t = std::pair<coordinate_t, coordinate_t>;

// Map that contains vector with change of rover's localization
static const std::unordered_map<Direction, coordinates_t> val =
        {{Direction::SOUTH, {0, -1}},
         {Direction::WEST, {-1, 0}},
         {Direction::NORTH, {0, 1}},
         {Direction::EAST, {1, 0}}};

#endif // DIRECTIONS_H
