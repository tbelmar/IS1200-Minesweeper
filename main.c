/*
Final Datateknik Project - Minesweeper
Project written by Tomás Belmar da Costa
File written in February 2021
Last modified March 2021
*/

#include <stdint.h>
#include <stdbool.h>
#include <pic32mx.h>
#include "game/game.h"
#include "display/display.h"
#include "chipkitfuncs/iohandler.h"
#include "chipkitfuncs/timerhandler.h"

int moving = 0;
int flagging = 0;
int opening = 0;

void user_isr() {}

int main() {
	int i, j;
	// Screen setup
	spi_init();
	display_start();

	// Timer init
	timerInit();
	startTimer();

	// Set up inputs/outputs
	ioInit();

	// Do opening animation
	openingAnim();
	while (getbtns() == 0);

	// Open menu
	clearScreen();

	// Clear screen after menu
	clearScreen();
	setupGame();
	
	openTile(playerPosition);
	convertBoardToDisplay();
	renderBoard();

	restartTimer();

	while (!gameOver) {
		do10TimesPerSecond();

		int btnInfo = getbtns();
		int flag = (getsw() >> 2) & 1;
		int open = (getsw() >> 3) & 1;

		if (flagging != flag) {
			flagging = flag;
			flagTile(playerPosition);
			convertBoardToDisplay();
			renderBoard();
		}

		if (opening != open) {
			opening = open;
			openTile(playerPosition);
			convertBoardToDisplay();
			renderBoard();
		}
		
		if (moving && btnInfo == 0)
			moving = 0;

		if (!moving && btnInfo != 0) {
			moving = 1;
			switch (btnInfo) {
				case 0x1:
					movePlayer(4);
					break;
				case 0x2:
					movePlayer(3);
					break;
				case 0x4:
					movePlayer(6);
					break;
				case 0x8:
					movePlayer(1);
					break;
			}
			clearScreen();
			convertBoardToDisplay();
			renderBoard();
		}
	}

	restartTimer();
	while (seconds != 3)
		do10TimesPerSecond();

	if (won) {
		drawWinningTime(gameTime-3);
		renderScreen(winningPicture);
	}
	else
		renderScreen(losingPicture);
	return 1;
}