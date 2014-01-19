#include <pebble.h>
#include <time.h>
#include "Binary Clock.h"
#include "statusLayer.h"

extern struct Config config;

static GBitmap *batteryBitmap;
static GBitmap *chargingBatteryBitmap;
static Layer *batteryLayer;
static BatteryChargeState curCharge;
static InverterLayer *batteryInverter;
static TextLayer *dateTextLayer;
static BitmapLayer *bluetoothLayer;
static InverterLayer *bluetoothInverter;

static void bluetoothConnectionHandler(bool connected) {
  GBitmap *img = (GBitmap *)bitmap_layer_get_bitmap(bluetoothLayer);
  if(img) {
    gbitmap_destroy(img);
  }
  if(connected) {
    bitmap_layer_set_bitmap(bluetoothLayer, gbitmap_create_with_resource(RESOURCE_ID_BLUETOOTH));
  } else {
    bitmap_layer_set_bitmap(bluetoothLayer, gbitmap_create_with_resource(RESOURCE_ID_NOBLUETOOTH));
  }
  layer_mark_dirty((Layer *)bluetoothLayer);
}

static void updateStatus(struct Layer *layer, GContext *ctx)
{
  static char dateText[16];
  time_t now;
  time(&now);
  if(config.date == MMDDYY) {
    strftime(dateText, sizeof(dateText), "%a %m/%d/%y", localtime(&now));
  } else {
    strftime(dateText, sizeof(dateText), "%a %d/%m/%y", localtime(&now));
  }
  text_layer_set_text_color(dateTextLayer, config.foreground);
  text_layer_set_background_color(dateTextLayer, config.background);
  text_layer_set_text(dateTextLayer, dateText);

  layer_set_hidden((Layer *)batteryInverter, config.background);
  layer_set_hidden((Layer *)bluetoothInverter, config.background);
}

static void updateBattBar(struct Layer *layer, GContext *ctx)
{
    graphics_context_set_fill_color(ctx, config.foreground);
    graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornersAll);
}

static void updateBatteryLayer(struct Layer *layer, GContext *ctx)
{
  static BitmapLayer *batteryBitmapLayer = NULL;
  static Layer *battBar = NULL;
  if(batteryBitmapLayer == NULL) {
    batteryBitmapLayer = bitmap_layer_create(layer_get_frame(layer));
    layer_add_child(layer, (Layer *)batteryBitmapLayer);
    battBar = layer_create((GRect){
      .origin = (GPoint) {
        .x = 0,
        .y = 0
      },
      .size = (GSize) {
        .w = 0,
        .h = 0
      }
    });
    layer_add_child((Layer *)batteryBitmapLayer, battBar);
  }
  if(curCharge.is_charging) {
    bitmap_layer_set_bitmap(batteryBitmapLayer, chargingBatteryBitmap);
    if(battBar != NULL) {
      layer_remove_from_parent(battBar);
      layer_destroy(battBar);
      battBar = NULL;
    }
  } else {
    bitmap_layer_set_bitmap(batteryBitmapLayer, batteryBitmap);
    if(battBar != NULL) {
      layer_remove_from_parent(battBar);
      layer_destroy(battBar);
    }
    battBar = layer_create((GRect) {
      .origin = (GPoint) {
        .x = 4,
        .y = 8
      },
      .size = (GSize) {
        .w = (21 * curCharge.charge_percent) / 100,
        .h = 4
      }
    });
    layer_set_update_proc(battBar, updateBattBar);
    layer_add_child(layer, battBar);
  }
}

static void updateBatteryStatus(BatteryChargeState charge)
{
  memcpy(&curCharge, &charge, sizeof(BatteryChargeState));
  layer_mark_dirty((Layer *)batteryLayer);
}

Layer *status_layer_create(GRect bounds)
{
  Layer *layer = layer_create(bounds);

  batteryBitmap = gbitmap_create_with_resource(RESOURCE_ID_BATTERY);
  chargingBatteryBitmap = gbitmap_create_with_resource(RESOURCE_ID_CHARGINGBATTERY);

  GRect batteryBounds = batteryBitmap->bounds;
  batteryLayer = layer_create(batteryBounds);
  battery_state_service_subscribe(updateBatteryStatus);   
  updateBatteryStatus(battery_state_service_peek());
  layer_set_update_proc(batteryLayer, updateBatteryLayer);
  layer_add_child(layer, batteryLayer);

  batteryInverter = inverter_layer_create((GRect){
    .size = batteryBounds.size,
    .origin = (GPoint) {.x = 0, .y = 0}
  });
  layer_add_child(batteryLayer, (Layer *)batteryInverter);
  layer_set_hidden((Layer *)batteryInverter, config.background);

  GRect bluetoothBounds = (GRect){
    .size = (GSize) {.w = 20, .h=20},
    .origin = (GPoint) {.x = bounds.size.w - 20, .y = 0}
  };
  bluetoothLayer = bitmap_layer_create(bluetoothBounds);
  bluetooth_connection_service_subscribe(bluetoothConnectionHandler);
  bluetoothConnectionHandler(bluetooth_connection_service_peek());
  layer_add_child(layer, (Layer *)bluetoothLayer);

  bluetoothInverter = inverter_layer_create((GRect){
    .size = bluetoothBounds.size,
    .origin = (GPoint) {.x = 0, .y = 0}
  });
  layer_add_child((Layer *)bluetoothLayer, (Layer *)bluetoothInverter);
  layer_set_hidden((Layer *)bluetoothInverter, config.background);

  dateTextLayer = text_layer_create((GRect){
    .size = (GSize) {
      .h = bounds.size.h,
      .w = bounds.size.w - batteryBounds.size.w - bluetoothBounds.size.w
    },
    .origin = (GPoint){
      .x = batteryBounds.size.w,
      .y = 0,
    }
  });
  text_layer_set_text_alignment(dateTextLayer, GTextAlignmentCenter);
  layer_add_child(layer, (Layer *)dateTextLayer);

  layer_set_update_proc(layer, updateStatus);
  return layer;
}
