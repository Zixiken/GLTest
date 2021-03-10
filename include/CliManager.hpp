//ECMA-48
#include <termios.h>
#include <iostream>
#include <sys/ioctl.h>

#define INVERSE "\033[7m"
#define NORMAL "\033[27m"

bool initManager(unsigned short height, unsigned short width);
bool initManager(unsigned short height, unsigned short width, unsigned short desiredRows, unsigned short desiredCols);
void closeManager();
void setPos(unsigned short row, unsigned short col);
void setPos(unsigned short row, unsigned short col, unsigned short rowOffset, unsigned short colOffset);
void setCol(unsigned short col);
void setRow(unsigned short row);
unsigned short getHeight();
unsigned short getWidth();
unsigned short getCellHeight();
unsigned short getCellWidth();
char waitForChar();
