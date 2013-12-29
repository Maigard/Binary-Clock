#include <pebble.h>
#include "Binary Clock.h"
#define NUMROWS 6
#define NUMCOLS 3

struct Config config;
static Window *window;
static Layer *dots[NUMROWS][NUMCOLS];


static void drawBackground(struct Layer *layer, GContext *ctx)
{
  graphics_context_set_fill_color(ctx, config.background);
  graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);
}

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

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  int height = bounds.size.h / NUMROWS;
  int width = bounds.size.w / NUMCOLS;
  GSize dotSize = (GSize){.w = width, .h = height};
  int i, j;
  bool enabled = true;

  layer_set_update_proc(window_layer, drawBackground);
  for(i=0;i<NUMROWS;i++){
    for(j=0;j<NUMCOLS;j++){
      dots[i][j] = layer_create((GRect){.size=dotSize, .origin=(GPoint){.x=j*dotSize.w, .y=i*dotSize.h}});
      layer_set_clips(dots[i][j], true);
      if(enabled){
        layer_set_update_proc(dots[i][j], drawEnabledDot);
        enabled = false;
      } else {
        layer_set_update_proc(dots[i][j], drawDisabledDot);
        enabled = true;
      }
      layer_add_child(window_layer, dots[i][j]);
    }
  }
}

static void window_unload(Window *window) {
  int i, j;
  for(i=0;i<NUMCOLS;i++){
    for(j=0;j<NUMROWS;j++)
      layer_destroy(dots[i][j]);
  }
}

static void init(void) {
  config = (struct Config){.background = GColorBlack, .foreground = GColorWhite};
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}
