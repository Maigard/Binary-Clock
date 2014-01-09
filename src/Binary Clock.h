#pragma once

#define CONFIG 1

#define DDMMYY 1
#define MMDDYY 2

enum ConfigFields { 
	foreground,
	date,
	statusBar,
	decimalDigits,
};

struct Config {
	GColor background;
	GColor foreground;
	int numRows;
	int numCols;
    int date;
    bool statusBar;
    bool decimalDigits;
};