// Seth Donohue - RGB LED controlled by 3axis tilt of ADXL345
// This is the BT Master device code to send the proper HUE and BRIGHTNESS signals to the BT Slave device.
// Written for use on two Bluno Beetls, but should work on most arduino devices if the serial is setup properly
// TODO: HOW to get axis to ZERO out so this MASTER device can be place on any surface, be
// TODO: MOVE ALL FastLED code to slave file, BT-rgb-led.ino
// TODO: ADD serial write to send a chunk of data to Slave device
//        - ref: uttp://www.martyncurrey.com/arduino-to-arduino-by-bluetooth/

// DONE: ADD toggle input to allow adustment of axis individually.
//      - may need  different buttons? Answer: Yup.
//      - Can use one button to cycle through and show a signal LED that tells which axis you are adjusting
//        - OR have the LED's flash a count for X, Y or Z axis.
//        - Solution: USe one momentary button for X and one for Y. LED strip indicates if adustment is working or not.
// DONE: HOW? To get Z-axis to be adjustable? Answer: Not posible at this time.
// TODO: Use Z Axis and hand up-down motion to turn off an on lights?
// DONE: Use Single tap for high fives to turn LED's white quickly and fade away.
// TODO: Use Double Tap to cycle through LED patterns.

/***************************************************************/
// Controller Inputs

// Used to convert radians to degrees, set to 1/1 to keep output to radians
int degreeToRadControl = PI / PI;
boolean DEBUG = false;
int UPDATES_PER_SECOND = 10;

// Axis Adjustment Toggle and pins
int XadjustmentAllowed = 1; // TODO: set back to 0
int YadjustmentAllowed = 1; // TODO: set back to 0
const int XtogglePin = 9;
const int YtogglePin = 8;

// Accelerometer declarations and imports
#include <Wire.h> //Call the I2C library built in Arduino
#include <SparkFun_ADXL345.h> // SparkFun ADXL345 Library

// TODO: Is the below needed or does Wire do the same thing?
ADXL345 adxl = ADXL345(); // USE FOR I2C COMMUNICATION

//Set the address of the register
#define Register_ID 0
#define Register_2D 0x2D
#define Register_X0 0x32
#define Register_X1 0x33
#define Register_Y0 0x34
#define Register_Y1 0x35
#define Register_Z0 0x36
#define Register_Z1 0x37

int ADXAddress = 0x53; //I2C address
int X0, X1, X_out;
int Y0, Y1, Y_out;
int Z1, Z0, Z_out;
double Xg, Yg, Zg;
double Xangle, Yangle, Zangle;
int BRIGHTNESS;
int singleHUE;

// TODO: IS this where we add a calibration to set the axis back if the device is not orienteded flat?
// if someInputPin === HIGH ) {
//  -set xCalibration = X_angle(double Xg, double Yg, double Zg, int degreeControl)???
//}

// Calculating radians and then converting to degrees with atan(param) * (180/PI);
//    adding 1.5 radians to end to make range from 0rad to 3rad instead of -1.5rad to 1.5rad
double X_angle(double Xg, double Yg, double Zg, int degreeControl)
{
  return (atan(Xg / (sqrt((Yg * Yg) + (Zg * Zg)))) * degreeControl) + 1.5;
}

double Y_angle(double Xg, double Yg, double Zg, int degreeControl)
{
  return (atan(Yg / (sqrt((Xg * Xg) + (Zg * Zg)))) * degreeControl) + 1.5;
}

double Z_angle(double Xg, double Yg, double Zg, int degreeControl)
{
  return (atan(Zg / (sqrt((Yg * Yg) + (Xg * Xg)))) * degreeControl) + 1.5;
}

/**************************************************************************/
void setup()
{
  delay(3000); // power-up safety delay

  //initialize the X-toggle pin as input
  pinMode(XtogglePin, INPUT);

  //initialize the Y-toggle pin as input
  pinMode(YtogglePin, INPUT);

  Serial.begin(115200); //Set the baud rate of serial monitor

  if (DEBUG)
  {
    Serial.print("Serial started at: ");
    Serial.println(115200);
  }

  //ADXL345
  delay(100);
  Wire.begin(); //Initialize I2C
  delay(100);
  Wire.beginTransmission(ADXAddress);
  Wire.write(Register_2D);
  Wire.write(8);
  Wire.endTransmission();

  if (DEBUG) {
    Serial.println(__FILE__);
    Serial.println(__DATE__);
    Serial.print("Updates / Sec: ");
    Serial.println(UPDATES_PER_SECOND);

    Serial.println("Accelerometer Test ");
    Serial.print("Degree to Radian Control set to: ");
    Serial.println(degreeToRadControl);
  }

  // -------------- TAP Detection Setup --------------
  adxl.powerOn(); // Power on the ADXL345

  adxl.setRangeSetting(16); // Give the range settings
                            // Accepted values are 2g, 4g, 8g or 16g
                            // Higher Values = Wider Measurement Range
                            // Lower Values = Greater Sensitivity

  adxl.setSpiBit(0); // Configure the device to be in 4 wire SPI mode when set to '0' or 3 wire SPI mode when set to 1
                     // Default: Set to 1
                     // SPI pins on the ATMega328: 11, 12 and 13 as reference in SPI Library

  adxl.setActivityXYZ(1, 0, 0);  // Set to activate movement detection in the axes "adxl.setActivityXYZ(X, Y, Z);" (1 == ON, 0 == OFF)
  adxl.setActivityThreshold(75); // 62.5mg per increment   // Set activity   // Inactivity thresholds (0-255)

  adxl.setInactivityXYZ(1, 0, 0);  // Set to detect inactivity in all the axes "adxl.setInactivityXYZ(X, Y, Z);" (1 == ON, 0 == OFF)
  adxl.setInactivityThreshold(75); // 62.5mg per increment   // Set inactivity // Inactivity thresholds (0-255)
  adxl.setTimeInactivity(5);      // How many seconds of no activity is inactive?

  adxl.setTapDetectionOnXYZ(0, 0, 1); // Detect taps in the directions turned ON "adxl.setTapDetectionOnX(X, Y, Z);" (1 == ON, 0 == OFF)

  // Set values for what is considered a TAP and what is a DOUBLE TAP (0-255)
  adxl.setTapThreshold(50);     // 62.5 mg per increment
  adxl.setTapDuration(15);      // 625 μs per increment
  adxl.setDoubleTapLatency(80); // 1.25 ms per increment
  adxl.setDoubleTapWindow(200); // 1.25 ms per increment

  // Set values for what is considered FREE FALL (0-255)
  adxl.setFreeFallThreshold(7); // (5 - 9) recommended - 62.5mg per increment
  adxl.setFreeFallDuration(30); // (20 - 70) recommended - 5ms per increment

  // Setting all interupts to take place on INT1 pin
  //adxl.setImportantInterruptMapping(1, 1, 1, 1, 1);     // Sets "adxl.setEveryInterruptMapping(single tap, double tap, free fall, activity, inactivity);"
  // Accepts only 1 or 2 values for pins INT1 and INT2. This chooses the pin on the ADXL345 to use for Interrupts.
  // This library may have a problem using INT2 pin. Default to INT1 pin.

  // Turn on Interrupts for each mode (1 == ON, 0 == OFF)
  adxl.InactivityINT(1);
  adxl.ActivityINT(1);
  adxl.FreeFallINT(1);
  adxl.doubleTapINT(1);
  adxl.singleTapINT(1);
}
/***************************************************************************/
void loop() // run over and over again
{
  // -------------- RGB-Accelerometer Control Main Program --------------
  // X-Axis reading...
  Wire.beginTransmission(ADXAddress);
  Wire.write(Register_X0);
  Wire.write(Register_X1);
  Wire.endTransmission();
  Wire.requestFrom(ADXAddress, 2);
  if (Wire.available() <= 2)
    ;
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
  if (Wire.available() <= 2)
    ;
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
  if (Wire.available() <= 2)
    ;
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

  // TODO: IS this where we add a calibration to set the axis back if the device is not orienteded flat?
  /* if ( someInputPin === HIGH ) {
    -store xCalibration value to use later, must be a GLOBAL value so that it persists outside of main loop.
      = X_angle(Xg, Yg, Zg, degreeToRadControl);

  }
  */
  Xangle = X_angle(Xg, Yg, Zg, degreeToRadControl);
  Yangle = Y_angle(Xg, Yg, Zg, degreeToRadControl);
  Zangle = Z_angle(Xg, Yg, Zg, degreeToRadControl);

  // Read the state of the toggle pins and check if the buttons are pressed
  // if it is the state is HIGH
  if (digitalRead(XtogglePin) == HIGH) { // TODO: ASK; Can we use Tap Detection here instead of button input?
  // RGB STRIP Hue setting based on X-Axis ONLY
  }

  if (digitalRead(YtogglePin) == HIGH) {
    // RGB STRIP BRIGHTNESS setting based on Y-Axis ONLY
  }
  
  BRIGHTNESS = (255 * (Yangle / 3)); // Radians
  singleHUE = (255 * (Xangle / 3)); // Radians

  // RGB STRIP
  // TODO: ADD; This is where we need to SEND a signla over Serial to the Slave device
  //        then we can run the fillAllLEDs on the Slave device code, not here.
  //        We may want to actually send the message at the END of the main loop as we can
  //        send more info that way, if it is a flash, or a rainbox, or just a hue change, etc...
  // fillAllLEDs(singleHUE);
  // FastLED.show();
  // FastLED.setBrightness(BRIGHTNESS);
  if (DEBUG) {
    //  Serial.print("X-Allowed=");
    //  Serial.print(XadjustmentAllowed);
    //  Serial.print("\tY-Allowed=");
    //  Serial.print(YadjustmentAllowed);
    Serial.print("\tBrightness=");
    Serial.print(BRIGHTNESS);
    Serial.print("\tHue=");
    Serial.println(singleHUE);
  } else {
    // Format: "<XXX_XXX>" which can be split at underscore.
    Serial.print("<");
    Serial.print(BRIGHTNESS);
    Serial.print("_");
    Serial.print(singleHUE);
    Serial.print(">");
    // Serial.write(BRIGHTNESS);
  }

  // -------------- TAP Detection Main Program --------------
  int x, y, z;
  adxl.readAccel(&x, &y, &z); // Read the accelerometer values and store them in variables declared above x,y,z
  byte interrupts = adxl.getInterruptSource();

  // Free Fall Detection
  if (adxl.triggered(interrupts, ADXL345_FREE_FALL))
  {
    Serial.println("*** FREE FALL ***");
    // TODO: This is where we attach data onto the message for special stuf other than just Hue or Brightness
    // Tracer to End of Arms.
  }

  // Inactivity
  // if(adxl.triggered(interrupts, ADXL345_INACTIVITY)){
  //   Serial.println("*** INACTIVITY ***");
  //    // TODO: Can I use this to fade out the LED's?
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
  // if(adxl.triggered(interrupts, ADXL345_SINGLE_TAP)){
  //   Serial.println("*** TAP ***");
  //     // DONE: Flash White, Full Brightness, then fade out
  //   whiteFlash(100, 5);
  // }

  // Adjust the value to change the refresh rate.
  delay(1000 / UPDATES_PER_SECOND);
}
