// Seth Donohue - RGB LED controlled by 3axis tilt of ADXL345
// This is the BT SLave device code to receive the proper HUE and BRIGHTNESS signals from the BT Master device.
// Written for use on two Bluno Beetles, but should work on most arduino devices if the serial is setup properly.

/*************************************************************************/

// #include <string.h>
// #include <stddef.h>

// FastLED Strip definitions
#include <FastLED.h>

boolean DEBUG = false;
int UPDATES_PER_SECOND = 10;
int BRIGHTNESS = 100;
int singleHUE = 75;

#define DATA_PIN 5
#define CLOCK_PIN 5
#define NUM_LEDS 10
#define LED_TYPE APA102
#define COLOR_ORDER RGB

CRGB leds[NUM_LEDS];

// Incoming Data definitions
// const byte maxDataLength = 7; // maxDataLength is the maximum length allowed for received data.
// char receivedChars[maxDataLength + 1];
// boolean newData = false; // newData is used to determine if there is a new command

const byte numChars = 32;
char receivedChars[numChars];
char tempChars[numChars];
boolean newData = false;

// ---------- LED Function declarations ----------

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
  Serial.println("+Bootup Sequence");
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

// ---------- Incoming Data Function declarations ----------

void recvWithStartEndMarkers()
{
  static boolean recvInProgress = false;
  static byte ndx = 0;
  char startMarker = '<';
  char endMarker = '>';
  char rc;

  while (Serial.available() > 0 && newData == false)
  {
    rc = Serial.read();

    if (recvInProgress == true)
    {
      if (rc != endMarker)
      {
        receivedChars[ndx] = rc;
        ndx++;
        if (ndx >= numChars)
        {
          ndx = numChars - 1;
        }
      }
      else
      {
        receivedChars[ndx] = '\0'; // terminate the string
        recvInProgress = false;
        ndx = 0;
        newData = true;
      }
    }
    else if (rc = startMarker)
    {
      recvInProgress = true;
    }
  }
}

void parseData()
{ // split the data into its parts

  char *strtokIndx; // this is used by strtok() as an index

  strtokIndx = strtok(tempChars, "_"); // get the first part
  BRIGHTNESS = atoi(strtokIndx);       // convert this part to an integer

  strtokIndx = strtok(NULL, "_"); // this continues where the previous call left off
  singleHUE = atoi(strtokIndx);   // convert this part to an integer
}

void showParsedData()
{
  Serial.print("Brightness: ");
  Serial.println(BRIGHTNESS);
  Serial.print("Hue ");
  Serial.println(singleHUE);
}

/**************************************************************************/
void setup()
{
  // -------------- RGB-Accelerometer LED Color Control Setup --------------
  delay(500); // power-up safety delay

  // RGB STRIP setup
  FastLED.addLeds<APA102, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);

  // Serial Monitor Debug Setup
  Serial.begin(115200); //Set the baud rate of serial monitor
  Serial.println("This program expects 2 pieces of data - INT Brightness, INT SingleHue");
  Serial.println("Enter data in this format   <123_456>");
  Serial.println("<Arduino is Ready>");
  Serial.println();

  if (DEBUG)
  {
    Serial.print("Serial started at: ");
    Serial.println(115200);

    Serial.println(__FILE__);
    Serial.println(__DATE__);
    Serial.print("Updates / Sec: ");
    Serial.println(UPDATES_PER_SECOND);
  }

  delay(100);
  // Serial.print("Brightness: ");
  // Serial.print(BRIGHTNESS);
  // Serial.print(" singleHUE: ");
  // Serial.print(singleHUE);
  // Serial.print(" DATA_PIN: ");
  // Serial.print(BRIGHTNESS);
  // Serial.print(" NUM_LEDS: ");
  // Serial.println(singleHUE);

  // bootupLoop();
}

/***************************************************************************/
void loop() // run over and over again
{

  // // Adjust the value to change the refresh rate.
  // FastLED.delay(1000 / UPDATES_PER_SECOND);
  recvWithStartEndMarkers();
  if (newData == true)
  {
    // Serial.print(tempChars);
    Serial.println(receivedChars);
    strcpy(tempChars, receivedChars);
    // this temporary copy is necessary to protect the original data
    //   because strtok() used in parseData() replaces the commas with \0
    parseData();
    showParsedData();
    newData = false;
  }
  // delay(250);
  // Set LED
  FastLED.setBrightness(BRIGHTNESS);
  fillAllLEDs(singleHUE);
  // FastLED.show();
}

