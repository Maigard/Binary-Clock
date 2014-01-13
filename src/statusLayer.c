#include <pebble.h>
#include <time.h>
#include "Binary Clock.h"
#include "statusLayer.h"

extern struct Config config;

static BitmapLayer *batteryBitmapLayer;
static InverterLayer *batteryInverter;
static TextLayer *dateTextLayer;
static char dateText[16];
static BitmapLayer *bluetoothLayer;
static InverterLayer *bluetoothInverter;

void bluetoothConnectionHandler(bool connected) {
  if(connected) {
    bitmap_layer_set_bitmap(bluetoothLayer, gbitmap_create_with_resource(RESOURCE_ID_BLUETOOTH));
  } else {
    bitmap_layer_set_bitmap(bluetoothLayer, gbitmap_create_with_resource(RESOURCE_ID_NOBLUETOOTH));
  }
}

static void updateStatus(struct Layer *layer, GContext *ctx)
{
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

static void updateBatteryStatus(BatteryChargeState charge)
{
  uint32_t batteryLevel;
  if(charge.is_charging)
    batteryLevel = 5;
  else if(charge.charge_percent > 80)
    batteryLevel = 4;
  else if(charge.charge_percent > 60)
    batteryLevel = 3;
  else if(charge.charge_percent > 40)
    batteryLevel = 2;
  else if(charge.charge_percent > 20)
    batteryLevel = 1;
  else
    batteryLevel = 0;
  GBitmap *baseBitmap = gbitmap_create_with_resource(RESOURCE_ID_BATTERY);
  bitmap_layer_set_bitmap(batteryBitmapLayer, gbitmap_create_as_sub_bitmap(
    baseBitmap,
    (GRect) {
      .size = (GSize) {
        .w = baseBitmap->bounds.size.w / 6,
        .h = baseBitmap->bounds.size.h
      },
      .origin = (GPoint) {
        .x = baseBitmap->bounds.size.w / 6 * batteryLevel,
        .y = 0
      }
    }));
}

Layer *status_layer_create(GRect bounds)
{
  Layer *layer = layer_create(bounds);

  GSize batterySize = gbitmap_create_with_resource(RESOURCE_ID_BATTERY)->bounds.size;
  GRect batteryBitmapBounds = (GRect) {
    .size = (GSize) {
      .w = batterySize.w / 6,
      .h = batterySize.h
    },
    .origin = (GPoint){
      .x = 0,
      .y = 0,
    }
  };
  batteryBitmapLayer = bitmap_layer_create(batteryBitmapBounds);
  battery_state_service_subscribe(updateBatteryStatus);   
  updateBatteryStatus(battery_state_service_peek());
  layer_add_child(layer, (Layer *)batteryBitmapLayer);

  batteryInverter = inverter_layer_create((GRect){
    .size = batteryBitmapBounds.size,
    .origin = (GPoint) {.x = 0, .y = 0}
  });
  layer_add_child((Layer *)batteryBitmapLayer, (Layer *)batteryInverter);
  layer_set_hidden((Layer *)batteryInverter, config.background);

  GRect bluetoothBounds = (GRect){
    .size = (GSize) {.w = 20, .h=20},
    .origin = (GPoint) {.x = bounds.size.w - 20, .y = 0}
  };
  bluetoothLayer = bitmap_layer_create(bluetoothBounds);
  if(bluetooth_connection_service_peek()) {
    bitmap_layer_set_bitmap(bluetoothLayer, gbitmap_create_with_resource(RESOURCE_ID_BLUETOOTH));
  } else {
    bitmap_layer_set_bitmap(bluetoothLayer, gbitmap_create_with_resource(RESOURCE_ID_NOBLUETOOTH));
  }
  layer_add_child(layer, (Layer *)bluetoothLayer);
  bluetooth_connection_service_subscribe(bluetoothConnectionHandler);

  bluetoothInverter = inverter_layer_create((GRect){
    .size = bluetoothBounds.size,
    .origin = (GPoint) {.x = 0, .y = 0}
  });
  layer_add_child((Layer *)bluetoothLayer, (Layer *)bluetoothInverter);
  layer_set_hidden((Layer *)bluetoothInverter, config.background);

  dateTextLayer = text_layer_create((GRect){
    .size = (GSize) {
      .h = bounds.size.h,
      .w = bounds.size.w - batteryBitmapBounds.size.w - bluetoothBounds.size.w
    },
    .origin = (GPoint){
      .x = batteryBitmapBounds.size.w,
      .y = 0,
    }
  });
  text_layer_set_text_alignment(dateTextLayer, GTextAlignmentCenter);
  layer_add_child(layer, (Layer *)dateTextLayer);

  layer_set_update_proc(layer, updateStatus);
  return layer;
}
