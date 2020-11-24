#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "display.h"
#include "measurements.h"
#include "userinput.h"

Adafruit_SSD1306 display = Adafruit_SSD1306();

extern Measurements measurements;

void initializeDisplay() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  display.display();

  // text display big!
  display.setTextSize(1);
  display.setTextColor(WHITE);

  display.clearDisplay();
  display.setCursor(0,0);
  display.print("Starting...");
  display.display();
}

void tDisplayCallback() {
  display.clearDisplay();
  display.setCursor(0,0);

  // Dummy code to force state change for now
  if (measurements.range > 100) {
    setState(2);
  }

  switch (getState()) {
    case STATE_CALIBRATION:
      display.println("Calibrating");
      break;

    default:
      if (measurements.status == 0) {
        display.print(measurements.range);
        display.print("mm\n");
      } else {
        display.print("n/a\n");
      }

      display.print(measurements.temperature);
      display.print("C\n");
      display.print(measurements.humidity);
      display.print("%\n");
      break;
  }
  display.println(getState());
  display.display();
}
