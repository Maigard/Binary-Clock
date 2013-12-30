#include <pebble.h>
#include <time.h>
#include "Binary Clock.h"
#include "dotsLayer.h"

extern struct Config config;
static struct tm *now;

enum timeField {
  seconds,
  minutes,
  hours
};

struct DotId {
  enum timeField field;
  int decimalDigit;
  int binaryDigit;
};

static void drawEnabledDot(struct Layer *layer, GContext *ctx)
{
  GRect bounds = layer_get_bounds(layer);
  int radius = (bounds.size.h < bounds.size.w ? bounds.size.h : bounds.size.w) / 2 - 2 ;

  graphics_context_set_fill_color(ctx, config.foreground);
  graphics_fill_circle(ctx, (GPoint){.x=bounds.size.w/2, .y=bounds.size.h/2}, radius);
}

static void drawDisabledDot(struct Layer *layer, GContext *ctx)
{
  GRect bounds = layer_get_bounds(layer);
  int strokeSize = 4;
  int radius = (bounds.size.h < bounds.size.w ? bounds.size.h : bounds.size.w) / 2 - 2 ;

  graphics_context_set_fill_color(ctx, config.foreground);
  graphics_fill_circle(ctx, (GPoint){.x=bounds.size.w/2, .y=bounds.size.h/2}, radius);
  graphics_context_set_fill_color(ctx, config.background);
  graphics_fill_circle(ctx, (GPoint){.x=bounds.size.w/2, .y=bounds.size.h/2}, radius - strokeSize);
}

static void drawDot(struct Layer *layer, GContext *ctx)
{
 struct DotId *dotId = layer_get_data(layer);
 int field;
 switch(dotId->field){
    case seconds:
      field = now->tm_sec;
      break;
    case minutes:
      field = now->tm_min;
      break;
    case hours:
      field = now->tm_hour;
      break;
    default:
      field = 0;
      break;
 }
 //app_log(APP_LOG_LEVEL_DEBUG, __FILE__, __LINE__, "%d %d 0x%x %d", field, dotId->decimalDigit, dotId->binaryDigit, ((field / dotId->decimalDigit) & dotId->binaryDigit));
 if((field / dotId->decimalDigit % (dotId->decimalDigit * 10)) & dotId->binaryDigit) {
    drawEnabledDot(layer, ctx);
 } else {
    drawDisabledDot(layer, ctx);
 }
}

struct dots_layer_data {
  int numRows;
  int numCols;
  Layer *dots[6][5];
};

static void updateTime(struct Layer *layer, GContext *ctx)
{
 time_t sinceEpoch = time(NULL);
 now = localtime(&sinceEpoch);
 char timeString[256];
 strftime(timeString, sizeof(timeString), "%D %T", now);
 //app_log(APP_LOG_LEVEL_DEBUG, __FILE__, __LINE__, "upated time: %s", timeString);
}

Layer *dots_layer_create(GRect bounds, int direction)
{
  Layer *layer = layer_create_with_data(bounds, sizeof(struct dots_layer_data));
  struct DotId *dotId;
  struct dots_layer_data *data = layer_get_data(layer);
  data->numRows = 4;
  data->numCols = 6;
  int height = bounds.size.h / data->numRows;
  int width = bounds.size.w / data->numCols;
  GSize dotSize = (GSize){.w = width, .h = height};
  int i, j;

  layer_set_update_proc(layer, updateTime);
  for(i=0;i<data->numRows;i++){
    for(j=0;j<data->numCols;j++){
      if(j % 2 && i > 2)
        continue;
      data->dots[i][j] = layer_create_with_data(
        (GRect){.size=dotSize, .origin=(GPoint){.x=bounds.size.w-(j+1)*dotSize.w, .y=bounds.size.h-(i+1)*dotSize.h}},
        sizeof(struct DotId)
      );
      layer_set_clips(data->dots[i][j], true);
      dotId = (struct DotId *)layer_get_data(data->dots[i][j]);
      switch(j) {
        case 0:
          dotId->field = seconds;
          dotId->decimalDigit = 1;
          break;
        case 1:
          dotId->field = seconds;
          dotId->decimalDigit = 10;
          break;
        case 2:
          dotId->field = minutes;
          dotId->decimalDigit = 1;
          break;
        case 3:
          dotId->field = minutes;
          dotId->decimalDigit = 10;
          break;
        case 4:
          dotId->field = hours;
          dotId->decimalDigit = 1;
          break;
        case 5:
          dotId->field = hours;
          dotId->decimalDigit = 10;
          break;
      }
      dotId->binaryDigit = 1 << i;
      layer_set_update_proc(data->dots[i][j], drawDot);
      layer_add_child(layer, data->dots[i][j]);
    }
  }
  return layer;
}

void dots_layer_destroy(Layer *layer) {
  struct dots_layer_data *data = layer_get_data(layer);
  int i, j;
  for(i=0;i<data->numCols;i++){
    for(j=0;j<data->numRows;j++)
      layer_destroy(data->dots[i][j]);
  }
  layer_destroy(layer);
}