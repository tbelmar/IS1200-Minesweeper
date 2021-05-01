/*
Final Datateknik Project - Minesweeper
Project written by Tomás Belmar da Costa
File written in February 2021
Last modified March 2021
*/

#include <pic32mx.h>
#include "timerhandler.h"

int timeoutcount = 0;
int seconds = 0;

void timerInit() {
	T2CON = 0x0070;
	PR2 = 0x3D09;
	TMR2 = 0;
}

void startTimer() {
	T2CON = 0x8070;
}

void restartTimer() {
	TMR2 = 0;
	IFS(0) = 0;
	seconds = 0;
	timeoutcount = 0;
}

void do10TimesPerSecond() {
	while (((IFS(0) >> 8) & 0x1) != 1);

	IFS(0) = 0;

	if (timeoutcount++ % 10 == 0) {
		seconds++;
		PORTE &= ~0xFF;
		PORTE |= seconds;
	}
}