#ifndef TIMERHANDLER_H_
#define TIMERHANDLER_H_

#include "timerhandler.c"

void timerInit();
void startTimer();
void restartTimer();
void do10TimesPerSecond();

extern int timeoutcount;
extern int seconds;


#endif