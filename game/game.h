#ifndef GAME_H_
#define GAME_H_

#include "game.c"
#include <stdint.h>
#include "../config.h"

void movePlayer(int direction);
bool surrounds(int pos[], int checkPos[]);
bool checkBoundary(int pos[]);
int getTile(int pos[]);
void setTile(int pos[], int n);
void openTile(int pos[]);
void flagTile(int pos[]);
int surroundingBombs(int pos[], int bombs[][2], int nOfBombs);
void setupGame();

extern int board[SIZE_HORIZONTAL][SIZE_VERTICAL];
extern bool tileInfo[SIZE_HORIZONTAL][SIZE_VERTICAL];
extern bool flagInfo[SIZE_HORIZONTAL][SIZE_VERTICAL];
extern int playerPosition[2];
extern bool gameOver;
extern bool won;
extern int gameTime;

#endif