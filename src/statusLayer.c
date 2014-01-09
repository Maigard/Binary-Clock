#include <pebble.h>
#include <time.h>
#include "Binary Clock.h"
#include "statusLayer.h"

extern struct Config config;

static BatteryChargeState batteryLevel;
static TextLayer *batteryTextLayer;
static char batteryText[16];
static TextLayer *dateTextLayer;
static char dateText[16];
static BitmapLayer *bluetoothLayer;
static InverterLayer *bluetoothInverter;

void bluetoothConnectionHandler(bool connected) {
  layer_set_hidden((Layer *)bluetoothLayer, connected);
}

static void updateStatus(struct Layer *layer, GContext *ctx)
{
  snprintf(batteryText, sizeof(batteryText), "%d%%", batteryLevel.charge_percent);
  text_layer_set_text_color(batteryTextLayer, config.foreground);
  text_layer_set_background_color(batteryTextLayer, config.background);
  text_layer_set_text(batteryTextLayer, batteryText);

  time_t now;
  time(&now);
  if(config.date == MMDDYY) {
    strftime(dateText, sizeof(dateText), "%m/%d/%y", localtime(&now));
  } else {
    strftime(dateText, sizeof(dateText), "%d/%m/%y", localtime(&now));
  }
  text_layer_set_text_color(dateTextLayer, config.foreground);
  text_layer_set_background_color(dateTextLayer, config.background);
  text_layer_set_text(dateTextLayer, dateText);

  layer_set_hidden((Layer *)bluetoothInverter, config.background);
}

static void updateBatteryStatus(BatteryChargeState charge)
{
  memcpy(&batteryLevel, &charge, sizeof(BatteryChargeState));
  layer_mark_dirty((Layer *)batteryTextLayer);
}

Layer *status_layer_create(GRect bounds)
{
  Layer *layer = layer_create(bounds);
  static uint8_t bluetoothImg[20*20];

  batteryTextLayer = text_layer_create((GRect){
    .size = (GSize){
      .w = 25,
      .h = bounds.size.h
    },
    .origin = (GPoint){
      .x = 0,
      .y = 0,
    }
  });
  BatteryChargeState battery = battery_state_service_peek();
  memcpy(&batteryLevel, &battery, sizeof(BatteryChargeState));
  battery_state_service_subscribe(updateBatteryStatus);   
  layer_add_child(layer, (Layer *)batteryTextLayer);

  GSize dateSize = graphics_text_layout_get_content_size  ("00/00/0000",
    fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD),
    bounds,
    GTextOverflowModeTrailingEllipsis,
    GTextAlignmentCenter
  );
  APP_LOG(APP_LOG_LEVEL_DEBUG, "date size: %d x %d @ (%d,%d)", dateSize.w, dateSize.h, bounds.size.w / 2 - dateSize.w / 2, 0);
  dateTextLayer = text_layer_create((GRect){
    .size = dateSize,
    .origin = (GPoint){
      .x = bounds.size.w / 2 - dateSize.w / 2,
      .y = 0,
    }
  });
  layer_add_child(layer, (Layer *)dateTextLayer);

  GRect bluetoothBounds = (GRect){
    .size = (GSize) {.w = 20, .h=20},
    .origin = (GPoint) {.x = bounds.size.w - 20, .y = 0}
  };
  bluetoothLayer = bitmap_layer_create(bluetoothBounds);
  resource_load(resource_get_handle(RESOURCE_ID_BLUETOOTH), bluetoothImg, sizeof(bluetoothImg));
  bitmap_layer_set_bitmap(bluetoothLayer, gbitmap_create_with_data(bluetoothImg));
  layer_add_child(layer, (Layer *)bluetoothLayer);
  layer_set_hidden((Layer *)bluetoothLayer, ! bluetooth_connection_service_peek());
  bluetooth_connection_service_subscribe(bluetoothConnectionHandler);

  bluetoothInverter = inverter_layer_create((GRect){
    .size = bluetoothBounds.size,
    .origin = (GPoint) {.x = 0, .y = 0}
  });
  layer_add_child((Layer *)bluetoothLayer, (Layer *)bluetoothInverter);
  layer_set_hidden((Layer *)bluetoothInverter, config.background);

  layer_set_update_proc(layer, updateStatus);
  return layer;
}
