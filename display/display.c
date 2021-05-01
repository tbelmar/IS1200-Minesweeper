/*
Final Datateknik Project - Minesweeper
Project written by Tomás Belmar da Costa
File written in February 2021
Last modified March 2021
*/

#include "display.h"
#include "../game/game.h"
#include "../game/models.h"
#include "../media/tiles/tiles.h"
#include "../media/assets.h"
#include "../chipkitfuncs/timerhandler.h"
#include <stdint.h>
#include <stdbool.h>
#include <pic32mx.h>

#define DISPLAY_VDD PORTFbits.RF6
#define DISPLAY_VBATT PORTFbits.RF5
#define DISPLAY_COMMAND_DATA PORTFbits.RF4
#define DISPLAY_RESET PORTGbits.RG9

#define DISPLAY_VDD_PORT PORTF
#define DISPLAY_VDD_MASK 0x40
#define DISPLAY_VBATT_PORT PORTF
#define DISPLAY_VBATT_MASK 0x20
#define DISPLAY_COMMAND_DATA_PORT PORTF
#define DISPLAY_COMMAND_DATA_MASK 0x10
#define DISPLAY_RESET_PORT PORTG
#define DISPLAY_RESET_MASK 0x200

uint8_t boardDisplay[128 * 4];
uint8_t emptyBoard[128 * 4] = { 0 };
uint8_t tempOpeningPicture[128 * 4];

void delay(int ms) {
    int i;
    for (i = ms; i > 0; i--);
}

uint8_t spi_send_receive(uint8_t data) {
    while (!(SPI2STAT & 0x08));
    SPI2BUF = data;
    while (!(SPI2STAT & 0x01));
    return SPI2BUF;
}

/*
 * Enable SPI
 */
void spi_init() {
    // Set up peripheral bus clock
    OSCCON &= ~0x180000;
    OSCCON |= 0x080000;

    // Set up output pins
    AD1PCFG = 0xFFFF;
    ODCE = 0x0;
    TRISECLR = 0xFF;
    PORTE = 0x0;

    // Output pins for display signals
    PORTF = 0xFFFF;
    PORTG = (1 << 9);
    ODCF = 0x0;
    ODCG = 0x0;
    TRISFCLR = 0x70;
    TRISGCLR = 0x200;

    // Set up input pins
    TRISDSET = (1 << 8);
    TRISFSET = (1 << 1);

    // Set up SPI as master
    SPI2CON = 0;
    SPI2BRG = 4;

    // Clear SPIROV
    SPI2STATCLR &= ~0x40;
    // Set CKP = 1, MSTEN = 1;
    SPI2CON |= 0x60;

    // Turn on SPI
    SPI2CONSET = 0x8000;
}

void renderScreen(uint8_t arr[]) {
    int i, j;

    for (i = 0; i < 4; i++) {
        DISPLAY_COMMAND_DATA_PORT &= ~DISPLAY_COMMAND_DATA_MASK;
        spi_send_receive(0x22);
        spi_send_receive(i);

        spi_send_receive(0 & 0xF);
        spi_send_receive(0x10 | ((0 >> 4) & 0xF));

        DISPLAY_COMMAND_DATA_PORT |= DISPLAY_COMMAND_DATA_MASK;

        for (j = 0; j < 128; j++)
            spi_send_receive(arr[i * 128 + j]);
    }
}

void display_start() {
    DISPLAY_COMMAND_DATA_PORT &= ~DISPLAY_COMMAND_DATA_MASK;
    delay(10);
    DISPLAY_VDD_PORT &= ~DISPLAY_VDD_MASK;
    delay(1000000);

    spi_send_receive(0xAE);
    DISPLAY_RESET_PORT &= ~DISPLAY_RESET_MASK;
    delay(10);
    DISPLAY_RESET_PORT |= DISPLAY_RESET_MASK;
    delay(10);

    spi_send_receive(0x8D);
    spi_send_receive(0x14);

    spi_send_receive(0xD9);
    spi_send_receive(0xF1);

    DISPLAY_VBATT_PORT &= ~DISPLAY_VBATT_MASK;
    delay(10000000);

    spi_send_receive(0xA1);
    spi_send_receive(0xC8);

    spi_send_receive(0xDA);
    spi_send_receive(0x20);

    spi_send_receive(0xAF);
}

void clearScreen() {
    int i;
    for (i = 0; i < 128 * 4; i++) {
        if(i%8 != 0)
            emptyBoard[i] = 0x00;
    }

    renderScreen(emptyBoard);
}

void openingAnim() {
    int i;

    for (i = 0; i < 128 * 4; i++)
        tempOpeningPicture[i] = openingPicture[i];
    
    for (i = 128 * 3; i < 128 * 4 - 14; i++) {
        tempOpeningPicture[i] = 0xFF;
        renderScreen(tempOpeningPicture);
        delay(50000);
    }

    for (i = 128 * 2 + 113; i < 128 * 2 + 118; i++) {
        tempOpeningPicture[i] = 0xFF;
        renderScreen(tempOpeningPicture);
        delay(50000);
    }

    for (i = 128 + 118; i > 128 + 109; i--) {
        tempOpeningPicture[i] = 0xFF;
        renderScreen(tempOpeningPicture);
        delay(50000);
    }

    for (i = 109; i > 100; i--) {
        tempOpeningPicture[i] = 0xFF;
        renderScreen(tempOpeningPicture);
        delay(50000);
    }
}

void drawMap(uint8_t map[], int screenPos[]) {
    int i;

    int screenX = screenPos[0];
    int screenY = screenPos[1];

    if (screenX > 15 || screenX < 0 || screenY > 3 || screenY < 0)
        return;

    int offset = 128 * screenY + 8 * screenX;

    for (i = 0; i < 8; i++)
        boardDisplay[offset + i] = map[i];
}

void drawTile(int tilePos[2], int screenPos[]) {
    int i;
    
    int tileX = tilePos[0];
    int tileY = tilePos[1];
    int screenX = screenPos[0];
    int screenY = screenPos[1];

    if (screenX > 15 || screenX < 0 || screenY > 3 || screenY < 0)
        return;

    int offset = 128 * screenY + 8 * screenX;

    if (!checkBoundary(tilePos)) {
        for (i = 0; i < 8; i++)
            boardDisplay[offset + i] = mapEmpty[i];
        return;
    }

    switch (getTile(tilePos)) {
        case 1:
            drawMap(map1, screenPos);
            break;
        case 2:
            drawMap(map2, screenPos);
            break;
        case 3:
            drawMap(map3, screenPos);
            break;
        case 4:
            drawMap(map4, screenPos);
            break;
        case 5:
            drawMap(map5, screenPos);
            break;
        case 6:
            drawMap(map6, screenPos);
            break;
        case 7:
            drawMap(map7, screenPos);
            break;
        case 8:
            drawMap(map8, screenPos);
            break;
        case 9:
            drawMap(mapBomb, screenPos);
            break;
        default:
            drawMap(mapEmpty, screenPos);
    }

    if (!tileInfo[tileX][tileY] && !flagInfo[tileX][tileY] && playerPosition[0] == tileX && playerPosition[1] == tileY)
        drawMap(mapPlayer, screenPos);
    else if(flagInfo[tileX][tileY])
        drawMap(mapFlag, screenPos);
    else if(!tileInfo[tileX][tileY])
        drawMap(mapFilled, screenPos);
}

void drawWinningTime(int t) {
    int i, j;

    int digits[4];

    digits[0] = (t / 60) / 10;
    digits[1] = (t / 60) % 10;
    digits[2] = (t % 60) / 10;
    digits[3] = (t % 60) % 10;

    int offsets[4] = {128*2 + 63, 128*2 + 71, 128*2 + 81, 128*2 + 89};

    for (i = 0; i < 4; i++) {
            switch (digits[i]) {
                case 1:
                    for (j = 0; j < 8; j++)
                        winningPicture[offsets[i] + j] = map1[j];
                    break;
                case 2:
                    for (j = 0; j < 8; j++)
                        winningPicture[offsets[i] + j] = map2[j];
                    break;
                case 3:
                    for (j = 0; j < 8; j++)
                        winningPicture[offsets[i] + j] = map3[j];
                    break;
                case 4:
                    for (j = 0; j < 8; j++)
                        winningPicture[offsets[i] + j] = map4[j];
                    break;
                case 5:
                    for (j = 0; j < 8; j++)
                        winningPicture[offsets[i] + j] = map5[j];
                    break;
                case 6:
                    for (j = 0; j < 8; j++)
                        winningPicture[offsets[i] + j] = map6[j];
                    break;
                case 7:
                    for (j = 0; j < 8; j++)
                        winningPicture[offsets[i] + j] = map7[j];
                    break;
                case 8:
                    for (j = 0; j < 8; j++)
                        winningPicture[offsets[i] + j] = map8[j];
                    break;
                case 9:
                    for (j = 0; j < 8; j++)
                        winningPicture[offsets[i] + j] = map9[j];
                    break;
                default:
                    for (j = 0; j < 8; j++)
                        winningPicture[offsets[i] + j] = map0[j];
            }
    }

    winningPicture[128 * 2 + 79] = 0x00;
    winningPicture[128 * 2 + 80] = 0xDB;
}

void renderBoard() {
    renderScreen(boardDisplay);
}

void boardTransition() {

}

void convertBoardToDisplay() {
    int i, j;

    int pX = playerPosition[0];
    int pY = playerPosition[1];

    for (i = -7; i < 9; i++) {
        for (j = -1; j < 3; j++) {
            int tempPos[2] = { pX + i, pY + j };
            int boardPos[2] = { i + 7, j + 1 };
            int emptyPos[2] = { -1, -1 };
            drawTile(tempPos, boardPos);
        }
    }
}

void displayGame() {
    
}