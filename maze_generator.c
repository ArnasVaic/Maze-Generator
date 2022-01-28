#include <maze_generator.h>

uint8_t WALL_TILE = '1';
uint8_t PATH_TILE = '0';

typedef struct rect_t {
	int64_t x;
	int64_t y;
	int64_t w;
	int64_t h;
} rect_t;

typedef struct vec2_t {
	int64_t x;
	int64_t y;
} vec2_t;

/// Adds two vectors
/// @param a first vector 
/// @param b second vector
/// @return sum of two vectors
vec2_t vec2_add(vec2_t a, vec2_t b) {
	vec2_t result = {a.x + b.x, a.y + b.y};
	return result;
}

/// Subtracts two vectors
/// @param a first vector 
/// @param b second vector
/// @return difference of two vectors
vec2_t vec2_sub(vec2_t a, vec2_t b) {
	vec2_t result = {a.x - b.x, a.y - b.y};
	return result;
}

/// Determines if two vectors are equal
/// @param a first vector 
/// @param b second vector
/// @return a boolean value
uint8_t vec2_eq(vec2_t a, vec2_t b) {
	return ((a.x == b.x) && (a.y == b.y));
}

/// Determines if a rectangle rect contains a point p
/// @param a rectangle
/// @param b point
/// @return a boolean value
uint8_t rect_contains_vec(rect_t a, vec2_t b) {
	uint8_t x = (b.x >= a.x) && (b.x < a.x + a.w);
	uint8_t y = (b.y >= a.y) && (b.y < a.y + a.h);
	return x && y;
}

/// Generates a random value in range [low; high]
/// If (low > high) or (|low| + |high| >= LLONG_MAX) then returns 0. 
/// Otherwise would cause overflow.
/// @param low lower bound of value range
/// @param high upper bound of value range
/// @return generated value
int64_t generate_64bit_value(int64_t low, int64_t high) {
	if((low > high) || (llabs(low) + llabs(high) <= 0)) return 0;
	int64_t mod = (1 << 15) - 1;
	int64_t value = 0;
	// First generate number in range [LLONG_MIN; LLONG_MAX]
	for (size_t i = 0; i < sizeof(int64_t); ++i) {
		value <<= 2 * CHAR_BIT; // Shift by 2 bytes
		value += rand() % mod; 	// Make sure generated value is within 2 bytes
	}
	// Adjust the value to fall in range [low; high]
	// Will never overflow because of checks made at the start
	value = low + value % (high - low + 1);
	return value;
}

// Generates perpendicular walls in a given bounding box and draws them on the grid.
/// There are some rules: 
/// 1) walls cannot touch the bounding box (with their longer side)
/// 2) walls cannot block any exits that lead out of the room contained by the bounding box
/// @param grid the grid in which walls are going to be stored
/// @param grid_size the size of the grid
/// @param rect bounding box in which the walls are going to be generated
/// @return intersection of the walls (coordinates can be -1 if one or more walls were not generated)
vec2_t generate_walls(uint8_t *const grid, const vec2_t grid_size, const rect_t rect) {
	vec2_t wall = {-1, -1};
	// If rectangle size is 2 x 2 no walls can be generated
	if((rect.w == 2) && (rect.h == 2)) return wall;
	// If either of the dimensions are 1 no walls can be generated
	if((rect.w == 1) || (rect.h == 1)) return wall;

	uint8_t vert_spot_cnt = 0; 		// available vertical spot count
	int64_t vert_x[2] = {-1, -1}; 	// x coordiantes of invalid wall spots
	size_t j = 0; // index tracking current invalid wall position
	// calculate how many spots are available for the vertical wall
	for(int64_t i = rect.x + 1; i < rect.x + rect.w - 1; ++i) {
		// position of grid tile above the wall
		const vec2_t above = {i, rect.y - 1}; 
		// position of grid tile below the wall
		const vec2_t below = {i, rect.y + rect.h};

		const rect_t grid_rect = {0, 0, grid_size.x, grid_size.y};
		const uint8_t cont_a = rect_contains_vec(grid_rect, above);
		const uint8_t cont_b = rect_contains_vec(grid_rect, below);

		const size_t ai = above.x + above.y * grid_rect.w; // index of tile above
		const size_t bi = below.x + below.y * grid_rect.w; // index of tile below

		const uint8_t cond1 = (!cont_a || (grid[ai] == WALL_TILE)); // logical implication
		const uint8_t cond2 = (!cont_b || (grid[bi] == WALL_TILE)); // this too
		if(cond1 && cond2) {
			++vert_spot_cnt; 
		} else {
			vert_x[j++] = i;
		}
	}
	// generate a vertical wall
	if(vert_spot_cnt > 0) {
		do {
			wall.x = generate_64bit_value(rect.x + 1, rect.x + rect.w - 2);
		} while((wall.x == vert_x[0]) || (wall.x == vert_x[1]));
#ifdef DEBUG_MAZE
		fprintf(stderr, "vertical wall is generated at x = %lld\n", wall.x);
#endif
	} else {
#ifdef DEBUG_MAZE
		fprintf(stderr, "vertical wall is not going to be generated\n");
#endif
	}

	// similarly generate the horizontal wall
	uint8_t horz_spot_cnt = 0; 		// available horizontal spot count
	int64_t horz_y[2] = {-1, -1}; 	// y coordiantes of invalid wall spots
	j = 0; // use to track index of current invalid wall position
	// calculate how many spots are available for the horizontal wall
	for(int64_t i = rect.y + 1; i < rect.y + rect.h - 1; ++i) {
		// position of grid tile left of the wall
		const vec2_t left = {rect.x - 1, i}; 
		// position of grid tile right of the wall
		const vec2_t right = {rect.x + rect.w, i};

		const rect_t grid_rect = {0, 0, grid_size.x, grid_size.y};
		const uint8_t cont_l = rect_contains_vec(grid_rect, left);
		const uint8_t cont_r = rect_contains_vec(grid_rect, right);

		const size_t li = left.x + left.y * grid_rect.w; 	// index of tile to the left
		const size_t ri = right.x + right.y * grid_rect.w; 	// index of tile to the right
		
		const uint8_t cond1 = (!cont_l || (grid[li] == WALL_TILE));
		const uint8_t cond2 = (!cont_r || (grid[ri] == WALL_TILE));

		if(cond1 && cond2)  {
			++horz_spot_cnt;
		} else {
			horz_y[j++] = i;
		}
	}

	// generate a horizontal wall
	if(horz_spot_cnt > 0) {
		do {
			wall.y = generate_64bit_value(rect.y + 1, rect.y + rect.h - 2);
		} while((wall.y == horz_y[0]) || (wall.y == horz_y[1]));
#ifdef DEBUG_MAZE
		fprintf(stderr, "horizontal wall is generated at y = %lld\n", wall.y);
#endif
	} else {
#ifdef DEBUG_MAZE
		fprintf(stderr, "horizontal wall is not going to be generated\n");
#endif
	}
	// 'draw' the walls
	if(wall.y != -1) {
		for(int64_t i = rect.x; i < rect.x + rect.w; ++i) {
			grid[i + wall.y * grid_size.x] = WALL_TILE;
		}
	}
	if(wall.x != -1) {
		for(int64_t i = rect.y; i < rect.y + rect.h; ++i) {
			grid[wall.x + i * grid_size.x] = WALL_TILE;
		}
	}
	return wall;
}

/// This function 'connects' rooms that were formed by generate_walls() 
/// with the minimal ammount of passages/exits
/// @param grid the grid in which exits are going to be stored
/// @param grid_size size of the grid
/// @param rect bounding box in which exits are going to be generated
/// @param wall return value of generate_walls
/// @param h_exit coordinates of the horizontal exit (out)
/// @param v_exit coordinates of the vertical exit (out)
/// @param t_exit coordinates of the third exit (out)
void generate_exits(
	uint8_t *const grid,
	const vec2_t grid_size,
	const rect_t rect,
	const vec2_t wall,
	vec2_t *const h_exit,
	vec2_t *const v_exit,
	vec2_t *const t_exit
) {
	// do nothing if walls do not exist
	if((wall.x == -1) && (wall.y == -1)) return;
	
	h_exit->x = wall.x; // horizontal exit is in the vertical wall thus x coordinates match
	v_exit->y = wall.y; // same logic here

	t_exit->x = wall.x; // < one of these will be overwritten
	t_exit->y = wall.y; // <

	// generate a horizontal exit if the vertical wall exists
	uint8_t vwall_exists = (wall.x != -1);
	if(vwall_exists) {
		do {
			h_exit->y = generate_64bit_value(rect.y, rect.y + rect.h - 1);
		} while(h_exit->y == wall.y); // exit cannot be at the intersection
		// 'draw' it on the grid
		grid[h_exit->x + h_exit->y * grid_size.x] = PATH_TILE;
#ifdef DEBUG_MAZE
		fprintf(stderr, "horizontal exit is generated at (%lld, %lld)\n", h_exit->x, h_exit->y);
#endif
	} else {
#ifdef DEBUG_MAZE
		fprintf(stderr, "horizontal exit will not be generated\n");
#endif
	}
	// generate vertical exit if the horizontal wall exists
	uint8_t hwall_exists = (wall.y != -1);
	if(hwall_exists) {
		do {
			v_exit->x = generate_64bit_value(rect.x, rect.x + rect.w - 1);
		} while(v_exit->x == wall.x); // exit cannot be at the intersection
		// 'draw' it on the grid
		grid[v_exit->x + v_exit->y * grid_size.x] = PATH_TILE;
#ifdef DEBUG_MAZE
		fprintf(stderr, "vertical exit is generated at (%lld, %lld)\n", v_exit->x, v_exit->y);
#endif
	} else {
#ifdef DEBUG_MAZE
		fprintf(stderr, "vertical exit will not be generated\n");
#endif
	}
	// generate third exit only if both walls exist
	// note: third exit must always be generated on the opposite side of the intersection
	// than the other exit on that wall
	if(vwall_exists && hwall_exists)  {
		const uint8_t orientation = rand() % 2; // 0 for vertical, 1 for horizontal
		int64_t low, high;
		if(orientation == 0) {
			// vertical exit on the left so make third exit on the right
			if(v_exit->x < wall.x) {
				low = wall.x + 1;
				high = rect.x + rect.w - 1;
			} else {
				low = rect.x;
				high = wall.x - 1;
			}
			t_exit->x = generate_64bit_value(low, high);
		} else {
			//horizontal exit above horizontal wall so make third exit below
			if(h_exit->y < wall.y) {
				low = wall.y + 1;
				high = rect.y + rect.h - 1;
			} else {
				low = rect.y;
				high = wall.y - 1;
			}
			t_exit->y = generate_64bit_value(low, high);
		}
		grid[t_exit->x + t_exit->y * grid_size.x] = PATH_TILE;
#ifdef DEBUG_MAZE
		fprintf(stderr, "third (%s) exit is generated at (%lld, %lld)\n", orientation?"horizontal":"vertical" , t_exit->x, t_exit->y);
#endif
	} else {
#ifdef DEBUG_MAZE
		fprintf(stderr, "third exit will not be generated\n");
#endif
	}
}

/// Calls generate_walls() and generate_exits() and then recursively calls
/// subdivide() on rooms that were created by generate_walls()
/// @param grid the array in which maze is going to be stored
/// @param grid_size the size of the grid
/// @param rect bounding box to subdivide
/// @param iter number of iteration. Used for debugging
void subdivide(uint8_t *const grid, const vec2_t grid_size, const rect_t rect, const size_t iter) {
#ifdef DEBUG_MAZE
	fprintf(stderr, "subdivide() called (iteration #%lld):\n", iter);
	fprintf(stderr, "bounding box position: (%lld, %lld), size: (%lld, %lld)\n", rect.x, rect.y, rect.w, rect.h);
#endif
	const vec2_t wall = generate_walls(grid, grid_size, rect);
	if((wall.x == -1) && (wall.y == -1))  {
#ifdef DEBUG_MAZE
		fprintf(stderr, "no walls were generated\n\n");
#endif
		return;
	}
#ifdef DEBUG_MAZE
	fprintf(stderr, "grid after generating walls:\n");
	fprint_grid(grid, grid_size.x, grid_size.y, 1);
#endif
	vec2_t e[3]; // coordinates of exists
	generate_exits(grid, grid_size, rect, wall, &e[0], &e[1], &e[2]);
#ifdef DEBUG_MAZE
	fprintf(stderr, "grid after generating exits:\n");
	fprint_grid(grid, grid_size.x, grid_size.y, 1);
	fprintf(stderr, "\n");
#endif
	// if there is only one wall, subdivide needs to be called 2 times instead of 4.

	// no vertical wall
	if((wall.x == -1) && (wall.y != -1)) {
		const rect_t rects[2] = {
			{rect.x, rect.y, rect.w, wall.y - rect.y},
			{rect.x, wall.y + 1, rect.w, rect.y + rect.h - wall.y - 1}
		};
		subdivide(grid, grid_size, rects[0], iter + 1);
		subdivide(grid, grid_size, rects[1], iter + 1);
	}
	// no horizontal wall
	if((wall.x != -1) && (wall.y == -1)) {
		const rect_t rects[2] = {
			{rect.x, rect.y, wall.x - rect.x, rect.h},
			{wall.x + 1, rect.y, rect.x + rect.w - wall.x - 1, rect.h}
		};
		subdivide(grid, grid_size, rects[0], iter + 1);
		subdivide(grid, grid_size, rects[1], iter + 1);
	}
	// both walls are present
	if((wall.x != -1) && (wall.y != -1)) {
		const rect_t rects[4] = {
			{rect.x, rect.y, wall.x - rect.x, wall.y - rect.y},
			{wall.x + 1, rect.y, rect.x + rect.w - wall.x - 1, wall.y - rect.y},
			{rect.x, wall.y + 1, wall.x - rect.x, rect.y + rect.h - wall.y - 1},
			{wall.x + 1, wall.y + 1, rect.x + rect.w - wall.x - 1, rect.y + rect.h - wall.y - 1}
		};
		subdivide(grid, grid_size, rects[0], iter + 1);
		subdivide(grid, grid_size, rects[1], iter + 1);
		subdivide(grid, grid_size, rects[2], iter + 1);
		subdivide(grid, grid_size, rects[3], iter + 1);
	}
}

void generate_maze(uint8_t *const grid, const size_t width, const size_t height) {
	const vec2_t size = {width, height};
	const rect_t rect = {0, 0, width, height};
	subdivide(grid, size, rect, 0);
}

void fprint_grid(FILE *os, uint8_t *const grid, const size_t width, const size_t height, const uint8_t spaces) {
	for(int64_t i = 0; i < width; ++i) {
		for(int64_t j = 0; j < height; ++j) {
			putc(grid[j + i * width], os);
			if(spaces) putc(' ', os);
		}
		putc('\n', os);
	}
}