/*
Final Datateknik Project - Minesweeper
Project written by Tomás Belmar da Costa
File written in February 2021
Last modified March 2021
*/

#include <stdbool.h>
#include <stdlib.h>
#include <pic32mx.h>
#include "game.h"
#include "../chipkitfuncs/timerhandler.h"
#include "../config.h"

void* stdout = (void*)0;

// Config variables
int sizeHorizontal = SIZE_HORIZONTAL;
int sizeVertical = SIZE_VERTICAL;
int nOfBombs = NUMBER_OF_BOMBS;

char flagSymbol = 43;
char tileSymbol = 254;
char bombSymbol = 35;

// variables used during runtime
int playerPosition[2];
int board[SIZE_HORIZONTAL][SIZE_VERTICAL];
bool tileInfo[SIZE_HORIZONTAL][SIZE_VERTICAL];
bool flagInfo[SIZE_HORIZONTAL][SIZE_VERTICAL];
int tilesOpened = 0;
bool gameOver = false;
bool won;
int gameTime;

// Checks if a position surrounds another position
bool surrounds(int pos[], int checkPos[]) {
	int i, j;
	int xDiff[9] = { -1, 0, 1, -1, 0, 1, -1, 0, 1 };
	int yDiff[9] = { -1, -1, -1, 0, 0, 0, 1, 1, 1 };

	for (i = 0; i < 9; i++) {
		if ((checkPos[0] == pos[0] + xDiff[i]) && (checkPos[1] == pos[1] + yDiff[i]))
			return true;						// returns true if checkPos is adjacent to pos
	}

	return false;
}

bool checkBoundary(int pos[]) {
	return !(pos[0] < 0 || pos[1] < 0 || pos[0] >= sizeHorizontal || pos[1] >= sizeVertical);
}

void movePlayer(int direction) {
	int xDiff[8] = { -1, 0, 1, -1, 1, -1, 0, 1 };
	int yDiff[8] = { -1, -1, -1, 0, 0, 1, 1, 1 };

	int newPos[2] = { playerPosition[0] + xDiff[direction], playerPosition[1] + yDiff[direction] };

	if (checkBoundary(newPos)) {
		playerPosition[0] += xDiff[direction];
		playerPosition[1] += yDiff[direction];
	}
}

int getTile(int pos[]) {
	return board[pos[0]][pos[1]];
}

void setTile(int pos[], int n) {
	board[pos[0]][pos[2]] = n;
}

void loseGame() {
	gameOver = true;
	won = false;
}

void winGame() {
	gameTime = seconds;
	gameOver = true;
	won = true;
}

void flagTile(int pos[]) {
	if (gameOver) return;

	if (tileInfo[pos[0]][pos[1]])
		return;
	flagInfo[pos[0]][pos[1]] = !flagInfo[pos[0]][pos[1]];
}

// Returns the number of surrounding bombs in a position
int surroundingBombs(int pos[], int bombs[][2], int nOfBombs) {
	int i;

	if (getTile(pos) == 9)
		return 9;

	int bombCount = 0;

	for (i = 0; i < nOfBombs; i++) {
		if (surrounds(pos, bombs[i]))
			bombCount++;
	}

	return bombCount;
}

void openTile(int pos[]) {
	if (gameOver || tileInfo[pos[0]][pos[1]] || flagInfo[pos[0]][pos[1]])
		return;

	int i;
	int xDiff[8] = { -1, 0, 1, -1, 1, -1, 0, 1 };
	int yDiff[8] = { -1, -1, -1, 0, 0, 1, 1, 1 };

	tileInfo[pos[0]][pos[1]] = 1;

	tilesOpened++;

	if (getTile(pos) == 9)
		loseGame();
	else if (tilesOpened == (sizeHorizontal * sizeVertical - nOfBombs))
		winGame();
	else if (getTile(pos) == 0) {
		for (i = 0; i < 8; i++) {
			int newPos[2] = { pos[0] + xDiff[i], pos[1] + yDiff[i] };
			if (checkBoundary(newPos))
				openTile(newPos);
		}
	}
}

// SETS THE GAME UP
void setupGame() {
	int i, j;

	// holds position of bombs
	int bombs[nOfBombs][2];					

	for (i = 0; i < sizeHorizontal; i++) {
		for (j = 0; j < sizeVertical; j++) {
			board[i][j] = -1;
			tileInfo[i][j] = 0;
			flagInfo[i][j] = 0;	
		}
	}
	
	int centerX = sizeHorizontal / 2;
	int centerY = sizeVertical / 2;

	// sets player position in center
	playerPosition[0] = centerX;
	playerPosition[1] = centerY;

	// places bombs in random places
	int bombsPlaced = 0;

	srand(TMR2);
	while (bombsPlaced < nOfBombs) {
		int bombPos[2];

		bombPos[0] = rand() % sizeHorizontal;
		bombPos[1] = rand() % sizeVertical;


		if (surrounds(playerPosition, bombPos) || board[bombPos[0]][bombPos[1]] == 9)
			continue;

		board[bombPos[0]][bombPos[1]] = 9;

		bombs[bombsPlaced][0] = bombPos[0];
		bombs[bombsPlaced][1] = bombPos[1];
		bombsPlaced++;
	}

	for (i = 0; i < sizeHorizontal; i++) {
		for (j = 0; j < sizeVertical; j++) {
			int pos[2] = { i, j };
			board[pos[0]][pos[1]] = surroundingBombs(pos, bombs, nOfBombs);
		}
	}
}