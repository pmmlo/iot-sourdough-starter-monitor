#include "Adafruit_VL6180X.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#include "measurements.h"
#include "userinput.h"

#define DHTPIN 10
#define DHTTYPE DHT22

Measurements measurements;

Adafruit_VL6180X vl = Adafruit_VL6180X();
DHT_Unified dht(DHTPIN, DHTTYPE);

void initializeMeasurements() {
  if (! vl.begin()) {
    Serial.println("Failed to find sensor");
    while (1);
  }
  Serial.println("VL6180X sensor found!");

  dht.begin();
  sensor_t dht_sensor;
  dht.temperature().getSensor(&dht_sensor);
}

void tMeasureCallback() {
  static int jarHeightMm = 0;

  measurements.range = vl.readRange();
  measurements.status = vl.readRangeStatus();

  sensors_event_t event;
  dht.temperature().getEvent(&event);
  measurements.temperature = event.temperature;
  dht.humidity().getEvent(&event);
  measurements.humidity = event.relative_humidity;

  measurements.rise_percent = measurements.range - jarHeightMm;

  Serial.print(measurements.range);
  Serial.print("mm, ");
  Serial.print(measurements.temperature);
  Serial.print("C, ");
  Serial.print(measurements.humidity);
  Serial.print("%, ");
  Serial.print(measurements.rise_percent);
  Serial.print("%\n");

  if (getState() == STATE_CALIBRATION) {
    jarHeightMm = measurements.range;
  }
}
