/*
Written by Tomás Belmar da Costa
File written in February 2021
Last modified February 2021
*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

// Config variables
int sizeHorizontal = 16;
int sizeVertical = 30;
int nOfBombs = 99;

char flagSymbol = 43;
char tileSymbol = 254;
char bombSymbol = 35;

// variables used during runtime
int playerPosition[2];
int* board[16];
bool* tileInfo[16];
bool* flagInfo[16];
int tilesOpened = 0;
bool gameOver = false;

void movePlayer(int direction) {
	int xDiff[8] = { -1, 0, 1, -1, 1, -1, 0, 1 };
	int yDiff[8] = { -1, -1, -1, 0, 0, 1, 1, 1 };

	playerPosition[0] += xDiff[direction];
	playerPosition[1] += yDiff[direction];
}

// Checks if a position surrounds another position
bool surrounds(int pos[2], int checkPos[2]) {
	int i, j;
	int xDiff[9] = { -1, 0, 1, -1, 0, 1, -1, 0, 1 };
	int yDiff[9] = { -1, -1, -1, 0, 0, 0, 1, 1, 1 };

	for (i = 0; i < 9; i++) {
		if ((checkPos[0] == pos[0] + xDiff[i]) && (checkPos[1] == pos[1] + yDiff[i]))
			return true;						// returns true if checkPos is adjacent to pos
	}

	return false;
}

bool checkBoundary(int pos[2]) {
	return !(pos[0] < 0 || pos[1] < 0 || pos[0] >= sizeHorizontal || pos[1] >= sizeVertical);
}

int getTile(int pos[2]) {
	int x = pos[0];
	int y = pos[1];
	return board[x][y];
}

void printGame() {
	int i, j;
	printf("    ");
	for (i = 0; i < sizeHorizontal; i++)
		printf("%3d", i);
	printf("\n    ");
	for (i = 0; i < sizeHorizontal; i++)
		printf("%s", "---");
	printf("\n");

	for (i = 0; i < sizeVertical; i++) {
		printf("%2d |", i);
		for (j = 0; j < sizeHorizontal; j++) {
			int pos[2] = { j, i };
			if (flagInfo[j][i])
				printf("%3c", flagSymbol);
			else if (!tileInfo[j][i])
				printf("%3c", tileSymbol);
			else {
				switch (getTile(pos)) {
				case 9:
					printf("%3c", bombSymbol);
					break;
				case 0:
					printf("%3c", ' ');
					break;
				default:
					printf("%3d", getTile(pos));
				}
			}
		}
		printf("\n");
	}
}

void printBoard() {
	int i, j;

	printf("    ");
	for (i = 0; i < sizeHorizontal; i++)
		printf("%3d", i);
	printf("\n    ");
	for (i = 0; i < sizeHorizontal; i++)
		printf("%s", "---");
	printf("\n");

	for (i = 0; i < sizeVertical; i++) {
		printf("%2d |", i);
		for (j = 0; j < sizeHorizontal; j++) {
			int pos[2] = { j, i };
			if (getTile(pos) == 9) printf("%3c", bombSymbol);
			else if (getTile(pos) == 0) printf("%3c", ' ');
			else printf("%3d", getTile(pos));
		}
		printf("\n");
	}
}

void loseGame() {
	printGame();
	printf("you lost :(\n");
	exit(0);
}

void winGame() {
	printGame();
	printf("you won!\n");
	exit(0);
}

void openTile(int pos[2]) {
	if (tileInfo[pos[0]][pos[1]] || flagInfo[pos[0]][pos[1]])
		return;

	int i;
	int xDiff[8] = { -1, 0, 1, -1, 1, -1, 0, 1 };
	int yDiff[8] = { -1, -1, -1, 0, 0, 1, 1, 1 };

	tileInfo[pos[0]][pos[1]] = 1;

	tilesOpened++;

	if (getTile(pos) == 9)
		loseGame();
	else if (tilesOpened == (sizeHorizontal * sizeVertical - nOfBombs + 1))
		winGame();
	else if (getTile(pos) == 0) {
		for (i = 0; i < 8; i++) {
			int newPos[2] = { pos[0] + xDiff[i], pos[1] + yDiff[i] };
			if (checkBoundary(newPos))
				openTile(newPos);
		}
	}
}

void flagTile(int pos[2]) {
	if (tileInfo[pos[0]][pos[1]])
		return;
	flagInfo[pos[0]][pos[1]] = !flagInfo[pos[0]][pos[1]];
}

// Returns the number of surrounding bombs in a position
int surroundingBombs(int pos[2], int bombs[nOfBombs][2], int nOfBombs) {
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

// SETS THE GAME UP, AND THEN STARTS IT
void startGame() {
	int i, j;
	for (i = 0; i < 16; i++) {
		*(board+i) = (int*)malloc(30 * sizeof(int));
		*(tileInfo+i) = (bool*)malloc(30 * sizeof(bool));
		*(flagInfo+i) = (bool*)malloc(30 * sizeof(bool));
	}

	// holds position of bombs
	int bombs[nOfBombs][2];

	for (i = 0; i < sizeHorizontal; i++) {
		for (j = 0; j < sizeVertical; j++) {
			*(*(board+i)+j) = -1;
			*(*(tileInfo + i) + j) = 0;
			*(*(flagInfo + i) + j) = 0;
		}
	}

	int centerX = sizeHorizontal / 2;
	int centerY = sizeVertical / 2;

	// sets player position in center
	playerPosition[0] = centerX;
	playerPosition[1] = centerY;

	// places bombs in random places
	int bombsPlaced = 0;

	//srand(time(0));
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

int main() {
	startGame();
	int pos[2] = { 8, 15 };
	openTile(pos);
	printBoard();
	printGame();
	while (!gameOver) {
		int xCoord;
		int yCoord;
		int flag = 0;
		printf("Flagging? (1 = yes, 0 = no)\n");
		scanf("%d", &flag);

		printf("Enter the tile's coordinate:");
		printf("\nX: ");
		scanf("%d", &pos[0]);
		printf("Y: ");
		scanf("%d", &pos[1]);
		printf("\n");
		if (flag == 1)
			flagTile(pos);
		else
			openTile(pos);
		printGame();
	}
	return 1;
}