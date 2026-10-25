#include <Adafruit_NeoPixel.h>

#define LED_PIN     3
#define LED_COUNT   1
#define BRIGHTNESS  10

Adafruit_NeoPixel strip(
  LED_COUNT,
  LED_PIN,
  NEO_GRB + NEO_KHZ800
);

uint32_t colors[] = {
  strip.Color(255, 0, 0),     // Merah
  strip.Color(0, 255, 0),     // Hijau
  strip.Color(0, 0, 255),     // Biru
  strip.Color(255, 255, 0),   // Kuning
  strip.Color(0, 255, 255),   // Cyan
  strip.Color(255, 0, 255),   // Magenta
  strip.Color(255, 255, 255)  // Putih
};

const int COLOR_COUNT = sizeof(colors) / sizeof(colors[0]);
unsigned long previousMillis = 0;
unsigned long effectMillis = 0;

int effect = 0;
int colorIndex = 0;
int brightnessValue = 0;
int fadeDirection = 1;
bool ledState = false;

void setup() {
  strip.begin();
  strip.setBrightness(BRIGHTNESS);
  strip.clear();
  strip.show();
}

void loop() {
  unsigned long now = millis();

  switch (effect) {
    case 0:
      if (now - previousMillis >= 500) {
        previousMillis = now;

        strip.setPixelColor(0, colors[colorIndex]);
        strip.show();
        colorIndex = (colorIndex + 1) % COLOR_COUNT;
      }
      break;

    case 1:
      if (now - previousMillis >= 150) {
        previousMillis = now;
        ledState = !ledState;

        if (ledState) {
          strip.setPixelColor(0, strip.Color(255, 0, 0));
        } else {
          strip.clear();
        }
        strip.show();
      }
      break;

    case 2:
      if (now - previousMillis >= 10) {
        previousMillis = now;
        brightnessValue += fadeDirection * 5;

        if (brightnessValue >= 255) {
          brightnessValue = 255;
          fadeDirection = -1;
        }

        if (brightnessValue <= 0) {
          brightnessValue = 0;
          fadeDirection = 1;
        }

        strip.setPixelColor(
          0,
          strip.Color(0, 0, brightnessValue)
        );
        strip.show();
      }
      break;

    case 3:
      if (now - previousMillis >= 100) {
        previousMillis = now;
        ledState = !ledState;

        if (ledState) {
          strip.setPixelColor(0, strip.Color(255, 255, 255));
        } else {
          strip.clear();
        }
        strip.show();
      }
      break;

    case 4:
      if (now - previousMillis >= 20) {
        previousMillis = now;
        static uint16_t rainbowPosition = 0;
        strip.setPixelColor(
          0,
          strip.gamma32(
            strip.ColorHSV(rainbowPosition)
          )
        );
        strip.show();
        rainbowPosition += 256;
      }
      break;

    case 5:
      if (now - previousMillis >= 300) {
        previousMillis = now;
        strip.setPixelColor(
          0,
          strip.Color(random(0, 256),random(0, 256),random(0, 256))
        );

        strip.show();
      }
      break;

    case 6:
      if (now - previousMillis >= 200) {
        previousMillis = now;
        ledState = !ledState;

        if (ledState) {
          strip.setPixelColor(0, strip.Color(255, 0, 0));
        } else {
          strip.setPixelColor(0, strip.Color(0, 0, 255));
        }
        strip.show();
      }
      break;
  }

  if (now - effectMillis >= 5000) {
    effectMillis = now;
    effect = (effect + 1) % 7;
    previousMillis = now;
    brightnessValue = 0;
    fadeDirection = 1;
    ledState = false;
    strip.clear();
    strip.show();
  }
}