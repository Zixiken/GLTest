/*
 * CliManager.hpp
 * 0.1
 * Michael Zanga
 *
 * This is a basic TUI management library that divides the terminal area into
 * "cells." A POSIX-compliant terminal supporting the ECMA-48 standard should
 * be able to run this library.
 *
 * This was an exploration into what is necessary to make a functional TUI.
 * It uses functions in termios.h and ioctl.h to get terminal dimensions and
 * change input mode settings for proper input handling, and uses ECMA-48
 * control sequences written to the terminal to move the cursor and modify
 * printed text.
 */

//These macros can be used when printing text to invert the fore/background colors.
#define INVERSE "\033[7m"
#define NORMAL "\033[27m"

/**
 * Initializes the manager by setting up data structures and reconfiguring the
 * terminal. This also makes space for content with newlines if desiredRows is
 * nonzero. A width or height of zero is changed to one instead of stopping
 * the initialization and returning false.
 *
 * TODO: Save old contents of terminal on init so the whole thing can be
 * cleared, and no hacky newlines need to be used.
 *
 * Parameters:
 *   height: The cell height
 *   width: The cell width
 *   desiredRows: The desired number of rows, default 0
 *   desiredCols: The desired number of columns, default 0
 *
 * Returns:
 *   false if initManager has already been called, terminal attributes could
 *   not be retrieved or set, or if the terminal size is too small to fit the
 *   desired number of cells. True otherwise.
 */
bool initManager(unsigned short height, unsigned short width);
bool initManager(unsigned short height, unsigned short width, unsigned short desiredRows, unsigned short desiredCols);

// Resets the terminal attributes that were changed during initialization,
// as well as the cursor mode.
void closeManager();

// Set the cursor position to the top-left corner of the cell at the specified
// row and column (starting from zero, from the top-left).
void setPos(unsigned short row, unsigned short col);

// Set the cursor position to the specified offset in the cell at the specified
// row and column (starting from zero, from the top-left).
void setPos(unsigned short row, unsigned short col, unsigned short rowOffset, unsigned short colOffset);

// Set the cursor x position to the right side of the cell at the specified
// column in the current row (starting from zero, from the left).
void setCol(unsigned short col);

/*
 * Set the cursor y position to the top side of the cell at the specified row
 * in the current column (starting from zero, from the top).
 * TODO: implement
 */
void setRow(unsigned short row);

// Get the terminal height in rows.
unsigned short getHeight();

// Get the terminal width in columns.
unsigned short getWidth();

// Get the cell height in rows.
unsigned short getCellHeight();

// Get the cell width in columns.
unsigned short getCellWidth();

// Blocks until a character is read on stdin, and returns the character.
char waitForChar();
