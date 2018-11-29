#include <Keyboard.h>

#include "FastLED.h"
#define DATA_PIN 7
#define CLK_PIN 8
#define LED_TYPE APA102
#define COLOR_ORDER BRG
#define NUM_LEDS 144
CRGB leds[NUM_LEDS];

void setup()
{
  delay(3000); // 3 second delay for recovery
  FastLED.addLeds<LED_TYPE, DATA_PIN, CLK_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(25);
}

void loop()
{
  // Fill leds with rainbow
  fill_rainbow(leds, NUM_LEDS, millis());
  FastLED.show();
  delay(2000);

  // Turn leds off
  fill_solid(leds, NUM_LEDS, CRGB::Black); // Set all leds black
  FastLED.show();
  delay(1000);
}