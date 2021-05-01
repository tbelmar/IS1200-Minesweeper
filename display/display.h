#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "display.c"
#include <stdint.h>

void delay(int ms);
uint8_t spi_send_receive(uint8_t data);
void spi_init();
void renderScreen(uint8_t arr[]);
void clearScreen();
void openingAnim();
void drawMap(uint8_t map[], int screenPos[]);
void drawTile(int tilePos[2], int screenPos[]);
void drawWinningTime(int t);
void renderBoard();
void display_start();
void displayGame();

#endif