# ⚙️ Maze-Generator ⚙️

A maze generation library written in C
## Building on Windows with `MinGW`

### Building instructions:
`Maze-Generator` GitHub repo: https://github.com/LionasBaslis2sis/Maze-Generator
```ps
$ git clone https://github.com/LionasBaslis2sis/Maze-Generator
$ cd Maze-Generator
$ mingw32-make  # run `mingw32-make debug` for debug build
```

### Debug build:
In debug mode, `generate_maze()` function will print additional debug info to `stderr`.

### Linking to `Maze-Generator`:
Linking is straightforward
```ps
$ gcc -o output main.c -lmaze_gen
```

## Example:
Example code how to generate a maze
```c
#include <stdlib.h> // for srand()
#include <string.h> // for memset()
#include <time.h>	// for time()
#include "maze_generator.h"

int main(void) {
	srand(time(NULL)); 		// generate_maze() uses rand()
	uint8_t grid[11 * 11]; 	// the grid is 11 x 11

	// change wall & path tiles to something different
	WALL_TILE = '#';
	PATH_TILE = '.'; 

	// fill grid with path tiles (generate_maze() generates walls)
	memset(grid, PATH_TILE, sizeof(grid));

	// generate maze
	generate_maze(grid, 11, 11);
	
	// print it to stdout
	fprint_grid(stdout, grid, 11, 11, 1 /* add spaces when printing */);
}
```