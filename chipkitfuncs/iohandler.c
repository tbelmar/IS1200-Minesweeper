/*
Final Datateknik Project - Minesweeper
Project written by Tomás Belmar da Costa
File written in February 2021
Last modified March 2021
*/

#include <pic32mx.h>
#include "iohandler.h"

void ioInit() {
	TRISD |= 0xFF;
	TRISE &= ~0xFF;
	TRISF |= 0x2;
}

int getsw(void) {
	int switchInformation = PORTD >> 8;
	switchInformation &= 0xF;
	return switchInformation;
}

int getbtns(void) {
	int buttonInformation = PORTD >> 5;
	buttonInformation &= 0x7;
	buttonInformation = buttonInformation << 1;
	buttonInformation |= (PORTF >> 1 & 1);
	return buttonInformation;
}