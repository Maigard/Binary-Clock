#include <pebble.h>
#include "Binary Clock.h"
#include "dotsLayer.h"

struct Config config;
static Window *window;

static void drawBackground(struct Layer *layer, GContext *ctx)
{
  graphics_context_set_fill_color(ctx, config.background);
  graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);
}

void window_update(struct tm *tick_time, TimeUnits units_changed)
{
  Layer *window_layer = window_get_root_layer(window);
  layer_mark_dirty(window_layer)
}

Layer *dotsLayer;
static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  layer_set_update_proc(window_layer, drawBackground);
  dotsLayer = dots_layer_create((GRect){
    .size=(GSize){.w=bounds.size.w, .h=bounds.size.h-40},
    .origin=(GPoint){.x=0, .y=20}
  }, HORIZONTAL_DOTS);
  layer_add_child(window_layer, dotsLayer);
  tick_timer_service_subscribe(SECOND_UNIT, window_update);
}

static void window_unload(Window *window) {
  //dots_layer_destroy();
  layer_destroy(dotsLayer);
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
