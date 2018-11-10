// Seth Donohue - RGB LED controlled by 3axis tilt of ADXL345
// This is the BT Master device code to send the proper HUE and BRIGHTNESS signals to the BT Slave device.
// Written for use on two Bluno Beetls, but should work on most arduino devices if the serial is setup properly

/*************************************************************************/
// Controller Inputs
#include <SparkFun_ADXL345.h> // SparkFun ADXL345 Library

// TODO: Is the below needed or does Wire do the same thing?

// FastLED Strip definitions
#include <FastLED.h>

#define LED_PIN 5
#define NUM_LEDS 60
#define LED_TYPE WS2812
#define COLOR_ORDER GRB
int BRIGHTNESS = 100;
int singleHUE = 75;

CRGB leds[NUM_LEDS];

#define UPDATES_PER_SECOND 10 // TODO: ASK; Does this need to be the same on Master & Slave?

// TODO: DEFINE the altSerial here...
// #include <AltSoftSerial.h>
// AltSoftSerial BTserial;

// Function declarations
// RGB Strip FILL ALL base don Hue
void fillAllLEDs(int hue)
{
  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i].setHue(hue);
  }
}

void fadeOut(int delay)
{
  for (int j = 254; j >= 0; j--)
  {
    FastLED.setBrightness(j);
    FastLED.delay(delay);
  }
}

void flash(int cycles, int hue, int brightness, int speed)
{
  ;
  fillAllLEDs(hue);
  FastLED.show();
  for (int i = 0; i < cycles; i++)
  {
    FastLED.setBrightness(brightness);
    FastLED.show();
    delay(speed);
    FastLED.setBrightness(0);
    FastLED.show();
    delay(speed);
  }
}

// FASTLed Patterns  - taken from https://www.tweaking4all.com/hardware/arduino/adruino-led-strip-effects/
// Rainbow - https://github.com/Resseguie/FastLED-Patterns/blob/master/fastled-patterns.ino
void rainbow(int cycles, int speed)
{
  if (cycles == 0)
  {
    for (int i = 0; i < NUM_LEDS; i++)
    {
      //	   leds[i] = Wheel(((i*256/NUM_LEDS)) & 255);
      leds[i].setHue((i * 256) / NUM_LEDS);
    }
    FastLED.show();
  }
  else
  {
    for (int i = 0; i < 256 * cycles; i++)
    {
      for (int j = 0; j < NUM_LEDS; j++)
      {
        // leds[i] = Wheel(((j*256) + i) & 255);
        leds[j].setHue((j * 256) + i);
      }
      FastLED.show();
      FastLED.delay(speed);
    }
  }
}

// Flash to white and Fade out
// TODO: Figure out mathematical solution to a proper fade, currently I just experiment and the
//       delay is roughly equivalent to the cycles to have a linear decrease in brigthness.
void whiteFlash(int flashLength, int fadeTime)
{
  // fadeTime is in seconds and is multipled times 100 to workk with our step/delay of 10ms
  // example: fadeTime = 5, 5 * 100 = 500, 500 * 10ms delay  = 5000ms total time.
  FastLED.setBrightness(255);
  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i].setRGB(255, 255, 255);
  }
  FastLED.show();
  FastLED.delay(1000);
  //  FastLED.delay(flashLength);

  fadeOut(10);
  FastLED.setBrightness(0);
}

// Bootup Loop
void bootupLoop()
{
  Serial.println('+Bootup Sequence');
  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i].setRGB(255, 255, 255);
    FastLED.show();
    FastLED.delay(50);
  }
  rainbow(1, 0);
  fadeOut(5);
  flash(3, 100, 25, 200);
}

/**************************************************************************/
void setup()
{
  // -------------- RGB-Accelerometer LED Color Control Setup --------------
  delay(3000); // power-up safety delay

  // RGB STRIP setup
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);

  // Serial Monitor Debug Setup
  Serial.begin(9600); //Set the baud rate of serial monitor as 9600bps
  delay(100);
  delay(100);
  Serial.println("Serial Monitor Test...");
  Serial.print("Brightness: ");
  Serial.print(BRIGHTNESS);
  Serial.print(" singleHUE: ");
  Serial.print(singleHUE);
  Serial.print(" LED_PIN: ");
  Serial.print(BRIGHTNESS);
  Serial.print(" NUM_LEDS: ");
  Serial.println(singleHUE);

  bootupLoop();
}

/***************************************************************************/
void loop() // run over and over again
{
  fillAllLEDs(singleHUE);
  FastLED.show();
  FastLED.setBrightness(BRIGHTNESS);
  Serial.print("\tBrightness=");
  Serial.print(BRIGHTNESS);
  Serial.print("\tHue=");
  Serial.println(singleHUE);

  // -------------- TAP Detection Main Program --------------
  int x, y, z;
  adxl.readAccel(&x, &y, &z); // Read the accelerometer values and store them in variables declared above x,y,z
  byte interrupts = adxl.getInterruptSource();

  // Free Fall Detection
  // if (Received Data String IS "FREEFALL") {
  //   Serial.println("*** FREE FALL ***");
  //   // TODO: Cycle through colors when freefalling
  //   // Tracer to End of Arms.
  //   FastLED.setBrightness(0);
  //   FastLED.delay(100);

  //   for (int i = 2; i < NUM_LEDS - 1; i++)
  //   {
  //     leds[i - 2].setRGB(0, 0, 0);
  //     leds[i - 1].setHue(100);
  //     leds[i].setHue(175);
  //     leds[i + 1].setHue(250);
  //     Serial.print(i);
  //     Serial.print(" ");
  //     FastLED.setBrightness(150);
  //     FastLED.show();
  //     FastLED.delay(50);
  //   }
  // }

  // Inactivity
  // if(adxl.triggered(interrupts, ADXL345_INACTIVITY)){
  //   Serial.println("*** INACTIVITY ***");
  //    // TODO: Can I use this to fade out the LED's?
  //   FastLED.setBrightness(25);
  // }

  // Activity
  // if(adxl.triggered(interrupts, ADXL345_ACTIVITY)){
  //   Serial.println("*** ACTIVITY ***");
  //    // TODO: Can I use this to fade in the LED's?
  // }

  // Double Tap Detection
  // if(adxl.triggered(interrupts, ADXL345_DOUBLE_TAP)){
  //   Serial.println("*** DOUBLE TAP ***");
  //   // TODO: USE This to Cycle Through LED patterns
  //   //       - Find Pattern Library!
  //   FastLED.setBrightness(100);
  //   rainbow(5, 5);
  // }

  // Tap Detection
  // if(Received Data String IS "TAP"){
  //   Serial.println("*** TAP ***");
  //     // DONE: Flash White, Full Brightness, then fade out
  //   whiteFlash(100, 5);
  // }

  // Adjust the value to change the refresh rate.
  FastLED.delay(1000 / UPDATES_PER_SECOND);
}
