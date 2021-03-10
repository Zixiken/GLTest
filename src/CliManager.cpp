#include "CliManager.hpp"

using namespace std;

unsigned short dx, dy;
tcflag_t oldLMode;
cc_t oldVMin, oldVTime;
struct termios term;
struct winsize sz;
bool initialized = false;

void setPos(unsigned short row, unsigned short col) {
    cout << "\033[" << 1+dy*row << ';' << 1+dx*col << 'H';
}

void setPos(unsigned short row, unsigned short col, unsigned short rowOffset, unsigned short colOffset) {
    cout << "\033[" << 1+rowOffset+dy*row << ';' << 1+colOffset+dx*col << 'H';
}

void setCol(unsigned short col) {
    cout << "\033[" << 1+dx*col << 'G';
}

void setRow(unsigned short row) {

}

bool initManager(unsigned short height, unsigned short width) {return initManager(height, width, 0, 0);}

bool initManager(unsigned short height, unsigned short width, unsigned short desiredRows, unsigned short desiredCols) {
    if(initialized || tcgetattr(0, &term) == -1) return false;
    oldLMode = term.c_lflag;
    oldVMin = term.c_cc[VMIN];
    oldVTime = term.c_cc[VTIME];
    ioctl(0, TIOCGWINSZ, &sz);

    dx = width ? width : 1, dy = height ? height : 1;
    if(desiredCols && sz.ws_col/dx < desiredCols) return false;
    if(desiredRows) {
	if(sz.ws_row/dy < desiredRows) return false;
	unsigned short i;
	for(i = desiredRows*height; i; i--) cout << '\n';
    }

    term.c_lflag &= ~(ICANON | ECHO);
    term.c_cc[VMIN] = 1;
    term.c_cc[VTIME] = 0;
    if(tcsetattr(0, TCSANOW, &term) == -1) return false;

    cout << "\033[?25l" << flush;

    initialized = true;
    return true;
}

void closeManager() {
    if(!initialized) return;
    term.c_lflag = oldLMode;
    term.c_cc[VMIN] = oldVMin;
    term.c_cc[VTIME] = oldVTime;
    tcsetattr(0, TCSANOW, &term);

    cout << "\033[m\033[?25h" << endl;

    initialized = false;
}

unsigned short getHeight() {return sz.ws_row;}

unsigned short getWidth() {return sz.ws_col;}

unsigned short getCellHeight() {return dy;}

unsigned short getCellWidth() {return dx;}

char waitForChar() {
    char c;
    while(cin.get(c).eof());
    return c;
}
