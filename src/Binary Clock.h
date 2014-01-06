#pragma once

#define CONFIG 1

#define VERTICAL 1
#define BIGENDIAN 2
#define LITTLEENDIAN 3

enum ConfigFields { 
	foreground,
	layout,
	statusBar,
	decimalDigits,
};

struct Config {
	GColor background;
	GColor foreground;
	int numRows;
	int numCols;
    int layout;
    bool statusBar;
    bool decimalDigits;
};