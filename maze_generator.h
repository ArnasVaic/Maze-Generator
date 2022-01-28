/// Implementation of maze generator is based on this
/// https://en.wikipedia.org/wiki/Maze_generation_algorithm#Recursive_division_method

// if DEBUG_MAZE is defined, generate_maze() will print debug information to stderr
// #define DEBUG_MAZE

#ifndef MAZE_GENERATOR_H
#define MAZE_GENERATOR_H

// system headers
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

extern uint8_t WALL_TILE;
extern uint8_t PATH_TILE;

/// Wrapper around subdivide
/// @param grid generated maze is going to be stored here
/// @param width the width of the grid
/// @param height the width of the grid
void generate_maze(uint8_t *const grid, const size_t width, const size_t height);

/// Prints a grid with some formatting and also prints a border
/// @param os output stream where grid is going to be printed
/// @param grid the grid to print
/// @param width the width of the grid
/// @param height the width of the grid
/// @param spaces boolean value that indicates whether to print spaces between characters (0 - no spaces, 1 - add spaces)
void fprint_grid(FILE *os, uint8_t *const grid, const size_t width, const size_t height, const uint8_t spaces);

#endif