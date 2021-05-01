#ifndef MODELS_H_   /* Include guard */
#define MODELS_H_

typedef struct Position {
	int x;
	int y;
} Position;

typedef struct Tile {
	int type;
	Position pos;
} Tile;

#endif