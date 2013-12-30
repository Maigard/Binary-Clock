#pragma once
#include <pebble.h>

#define HORIZONTAL_DOTS 1

Layer *dots_layer_create(GRect frame, int direction);
void dots_layer_destroy();
