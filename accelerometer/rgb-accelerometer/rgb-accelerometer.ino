// Seth Donohue - RGB LED controlled by 3axis tilt of ADXL345
// TODO: ADD toggle input to allow adustment of axis individually.
//      - may need  different buttons?
//      - Can use one button to cycle through and show a signal LED that tells which axis you are adjusting
//        - OR have the LED's flash a count for X, Y or Z axis.
// TODO: HOW? To get Z-axis to be adjustable?
// TODO: Use Z Axis and hand up-down motion to turn off an on lights?
/*************************************************************************/
// Controller Inputs

// Used to convert radians to degrees, set to 1/1 to keep output to radians
int degreeToRadControl = PI/PI;

// Axis Adjustment Toggle and pins
int XadjustmentAllowed = 0;
int YadjustmentAllowed = 0;
const int XtogglePin = 9;
const int YtogglePin = 8;

// Accelerometer declarations and imports
#include <Wire.h>  //Call the I2C library built in Arduino
//Set the address of the register
#define Register_ID 0
#define Register_2D 0x2D
#define Register_X0 0x32
#define Register_X1 0x33
#define Register_Y0 0x34
#define Register_Y1 0x35
#define Register_Z0 0x36
#define Register_Z1 0x37

int ADXAddress = 0x53;  //I2C address
// int reading = 0;
// int val = 0;
int X0, X1, X_out;
int Y0, Y1, Y_out;
int Z1, Z0, Z_out;
double Xg, Yg, Zg;
double Xangle, Yangle, Zangle;
int singleHUE;

// FastLED Strip definitions

#include <FastLED.h>

#define LED_PIN 7
#define NUM_LEDS 20
int BRIGHTNESS = 100;
#define LED_TYPE WS2812
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

#define UPDATES_PER_SECOND 100

// Function declarations

// RGBPalette16 currentPalette;
// TBlendType currentBlending;

// extern CRGBPalette16 myRedWhiteBluePalette;
// extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;

// RGB Strip FILL ALL base don Hue
void fillAllLEDs(int hue)
{
  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i].setHue(hue);
  }
}

// Calculating radians and then converting to degrees with atan(param) * (180/PI);
//    adding 1.5 radians to end to make range from 0rad to 3rad instead of -1.5rad to 1.5rad
double X_angle(double Xg, double Yg, double Zg, int degreeControl) {
  return (atan(Xg/(sqrt((Yg*Yg) + (Zg*Zg))))*degreeControl) + 1.5;
}

double Y_angle(double Xg, double Yg, double Zg, int degreeControl) {
  return (atan(Yg/(sqrt((Xg*Xg) + (Zg*Zg))))*degreeControl) + 1.5;
}

double Z_angle(double Xg, double Yg, double Zg, int degreeControl) {
  return (atan(Zg/(sqrt((Yg*Yg) + (Xg*Xg))))*degreeControl) + 1.5;
}


// Toggle function for signal input
//    - Write a function that accepts a signal input with a button
//    - ON: axis adjustment changes RGB
//    - OFF: axis adjustment disabled
//
//int toggleAxisAdjustment(adjustmentAllowed) {
//  if (adjustmentAllowed === 0)
//}

/**************************************************************************/
void setup()
{

  delay(3000); // power-up safety delay

  // RGB STRIP setup
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);

  //initialize the X-toggle pin as input
  pinMode(XtogglePin,INPUT);

  //initialize the Y-toggle pin as input
  pinMode(YtogglePin,INPUT);

  //ADXL345
  Serial.begin(9600);//Set the baud rate of serial monitor as 9600bps
  delay(100);
  Wire.begin();  //Initialize I2C
  delay(100);
  Wire.beginTransmission(ADXAddress);
  Wire.write(Register_2D);
  Wire.write(8);
  Wire.endTransmission();
  Serial.println("Accelerometer Test ");
  Serial.print("Degree to Radian Control set to: ");
  Serial.println(degreeToRadControl);

} 
/***************************************************************************/
void loop() // run over and over again 
{     
  //ADXL345
  // X-Axis reading...
  Wire.beginTransmission(ADXAddress);
  Wire.write(Register_X0);
  Wire.write(Register_X1);
  Wire.endTransmission();
  Wire.requestFrom(ADXAddress, 2);
  if (Wire.available() <= 2);
  {
    X0 = Wire.read();
    X1 = Wire.read();
    X1 = X1 << 8;
    X_out = X0 + X1;
  }

  // Y-Axis reading...
  Wire.beginTransmission(ADXAddress);
  Wire.write(Register_Y0);
  Wire.write(Register_Y1);
  Wire.endTransmission();
  Wire.requestFrom(ADXAddress, 2);
  if (Wire.available() <= 2);
  {
    Y0 = Wire.read();
    Y1 = Wire.read();
    Y1 = Y1 << 8;
    Y_out = Y0 + Y1;
  }
  // Z-Axis reading...
  Wire.beginTransmission(ADXAddress);
  Wire.write(Register_Z0);
  Wire.write(Register_Z1);
  Wire.endTransmission();
  Wire.requestFrom(ADXAddress, 2);
  if (Wire.available() <= 2);
  {
    Z0 = Wire.read();
    Z1 = Wire.read();
    Z1 = Z1 << 8;
    Z_out = Z0 + Z1;
  }

  //Convert the output result into the acceleration g, accurate to 2 decimal points.
  Xg = X_out / 256.00; 
  Yg = Y_out / 256.00;
  Zg = Z_out / 256.00;

  Xangle = X_angle(Xg, Yg, Zg, degreeToRadControl);
  Yangle = Y_angle(Xg, Yg, Zg, degreeToRadControl);
  Zangle = Z_angle(Xg, Yg, Zg, degreeToRadControl);

  // Read the state of the toggle pins and check if the buttons are pressed
  // if it is the state is HIGH
  if (digitalRead(XtogglePin) == HIGH)
  {
    XadjustmentAllowed = 1;
    // RGB STRIP Hue setting based on ADXL345 X-Axis ONLY
    singleHUE = (255 * (Xangle / 3));

    // RGB STRIP
    fillAllLEDs(singleHUE);
    // fill_solid(&(leds[i]), 10 /*number of leds*/, CHSV(224, 187, 255));
    FastLED.show();
  } else {
    XadjustmentAllowed = 0;
  }

  if (digitalRead(YtogglePin) == HIGH)
  {
    YadjustmentAllowed = 1;
    BRIGHTNESS = (255 * (Yangle / 3));
    FastLED.setBrightness(BRIGHTNESS);
  }
  else
  {
    YadjustmentAllowed = 0;
  }

  Serial.print("X-Allowed=");
  Serial.print(XadjustmentAllowed);
  Serial.print("\tY-Allowed=");
  Serial.print(YadjustmentAllowed);
  Serial.print("\tBrightness=");
  Serial.print(BRIGHTNESS);
  Serial.print("\tHue=");
  Serial.println(singleHUE);

  // Adjust the value to change the refresh rate.
  FastLED.delay(1000 / UPDATES_PER_SECOND);
} 
