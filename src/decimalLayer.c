#include <pebble.h>
#include <time.h>
#include "Binary Clock.h"
#include "decimalLayer.h"

extern struct Config config;

static TextLayer *digitLayers[6];
static char digitString[6][2];

static void updateDecimals(struct Layer *layer, GContext *ctx)
{
  char timeText[7];
  time_t now;
  int i;

  time(&now);
  strftime(timeText, sizeof(timeText), "%H%M%S", localtime(&now));
  for(i = 0; i < 6; i++) {
    text_layer_set_text_color(digitLayers[i], config.foreground);
    text_layer_set_background_color(digitLayers[i], config.background);
    digitString[i][0] = timeText[i];
    text_layer_set_text(digitLayers[i], digitString[i]);
    layer_mark_dirty((Layer *)digitLayers[i]);
  }
}

Layer *decimal_layer_create(GRect bounds)
{
  Layer *layer = layer_create(bounds);
  int i;

  memset(digitString, 0, sizeof(digitString));
  for(i=0;i< 6;i++) {
    digitLayers[i] = text_layer_create((GRect){
      .size = (GSize) {
        .w = bounds.size.w / 6,
        .h = bounds.size.h + 4
      },
      .origin = (GPoint) {
        .y = 0,
        .x = bounds.size.w / 6 * i,
      }
    });
    digitString[i][1] = '\0';
    text_layer_set_text_alignment(digitLayers[i], GTextAlignmentCenter);
    text_layer_set_font(digitLayers[i], fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
    layer_add_child(layer, (Layer *)digitLayers[i]);
  }

  layer_set_update_proc(layer, updateDecimals);
  return layer;
}
