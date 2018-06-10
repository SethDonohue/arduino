// Seth Donohue - RGB LED controlled by 3axis tilt of ADXL345
// TODO: ADD toggle input to allow adustment of axis individually.
//      - may need  different buttons?
//      - Can use one button to cycle through and show a signal LED that tells which axis you are adjusting
//        - OR have the LED's flash a count for X, Y or Z axis.
// TODO: HOW? To get Z-axis to be adjustable?
/*************************************************************************/
// Controller Inputs

const int redPin = 11; // R petal on RGB LED module connected to digital pin 11 
const int greenPin = 10; // G petal on RGB LED module connected to digital pin 10 
const int bluePin = 9; // B petal on RGB LED module connected to digital pin 9 
const int delayTime = 50; // milliseconds

// Used to convert radians to degrees, set to 1/1 to keep output to degrees
int degreeToRadControl = PI/PI;

// Axis Adjustment Toggle and pins
int adjustmentAllowed = 0;
const int togglePin = 12;

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
int reading = 0;
int val = 0;
int X0, X1, X_out;
int Y0, Y1, Y_out;
int Z1, Z0, Z_out;
double Xg, Yg, Zg;
double Xangle, Yangle, Zangle;
int redIn, greenIn, blueIn;

// Function declarations

// Color writing
void color (unsigned char red, unsigned char green, unsigned char blue) // the color generating function 
{ 
analogWrite(redPin, red); 
analogWrite(greenPin, green); 
analogWrite(bluePin, blue); 
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

/******************************************************/


/**************************************************************************/
void setup()
{ 
  pinMode(togglePin,INPUT);//initialize the toggle pin as input 
  
  // RGB
  pinMode(redPin, OUTPUT); // sets the redPin to be an output 
  pinMode(greenPin, OUTPUT); // sets the greenPin to be an output 
  pinMode(bluePin, OUTPUT); // sets the bluePin to be an output 

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
  // Read the state of the toggle pin and check if the buttong is pressed
  // if it is the state is HIGH
  if (digitalRead(togglePin) == HIGH) {
    adjustmentAllowed = 1;
      
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
   
    Xg = X_out / 256.00; //Convert the output result into the acceleration g, accurate to 2 decimal points.
    Yg = Y_out / 256.00;
    Zg = Z_out / 256.00;
  
    Xangle = X_angle(Xg, Yg, Zg, degreeToRadControl);
    Yangle = Y_angle(Xg, Yg, Zg, degreeToRadControl);
    Zangle = Z_angle(Xg, Yg, Zg, degreeToRadControl);
  
    // Creating range ratio for color value, angle in radians, max 3radians.
    redIn = (255*(Xangle/3));
    greenIn = (255*(Yangle/3));
    blueIn = (255*(Zangle/3));
  
  
    color(redIn, greenIn, blueIn);
  } else {
    adjustmentAllowed = 0;
  }

  Serial.print("Adjustment Allowed=");
  Serial.print(adjustmentAllowed);
  Serial.print("\tRed=");
  Serial.print(redIn);
  Serial.print("\tGreen=");
  Serial.print(greenIn);
  Serial.print("\tBlue=");
  Serial.println(blueIn);

  delay(delayTime);  // Adjust the value to change the refresh rate. 
} 


