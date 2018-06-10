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
void color (unsigned char red, unsigned char green, unsigned char blue) // the color generating function 
{ 
analogWrite(redPin, red); 
analogWrite(greenPin, green); 
analogWrite(bluePin, blue); 
}

// Calculating radians and then converting to degrees with atan(param) * (180/PI);
// adding 1.5 radians to end to make range from 0rad to 3rad instead of -1.5rad to 1.5rad
double X_angle(double Xg, double Yg, double Zg, int degreeControl) {
  return (atan(Xg/(sqrt((Yg*Yg) + (Zg*Zg))))*degreeControl) + 1.5;
}

double Y_angle(double Xg, double Yg, double Zg, int degreeControl) {
  return (atan(Yg/(sqrt((Xg*Xg) + (Zg*Zg))))*degreeControl) + 1.5;
}

double Z_angle(double Xg, double Yg, double Zg, int degreeControl) {
  return (atan(Zg/(sqrt((Yg*Yg) + (Xg*Xg))))*degreeControl) + 1.5;
}

/******************************************************/


/**************************************************************************/
void setup()
{ 
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

//  TODO: ADD an input setup that allows me to select which access to adjust to control colors indivudually
  
//  RGB Stuffs...
//  Basic colors: 
//  color(255, 0, 0); // turn the RGB LED red 
//  delay(delayTime); // delay for 1 second 
//  color(0,255, 0); // turn the RGB LED green 
//  delay(delayTime); // delay for 1 second 
//  color(0, 0, 255); // turn the RGB LED blue 
//  delay(delayTime); // delay for 1 second 
//  // Example blended colors: 
//  color(255,0,252); // turn the RGB LED red 
//  delay(delayTime); // delay for 1 second 
//  color(237,109,0); // turn the RGB LED orange 
//  delay(delayTime); // delay for 1 second 
//  color(255,215,0); // turn the RGB LED yellow 
//  delay(delayTime); // delay for 1 second 
//  color(34,139,34); // turn the RGB LED green 
//  delay(delayTime); // delay for 1 second 
//  color(0,112,255); // turn the RGB LED blue 
//  delay(delayTime); // delay for 1 second
//  color(0,46,90); // turn the RGB LED indigo 
//  delay(delayTime); // delay for 1 second
//  color(128,0,128); // turn the RGB LED purple 
//  delay(delayTime); // delay for 1 second

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

  
//  if (Yangle > 0.5 || Yangle < -0.5){
//    color(255, 255, 0); // turn the RGB LED red 
//  } else {
//    color(0, 0, 0); // turn the RGB LED red 
//  }
  color(redIn, greenIn, blueIn);

  Serial.print("Red=");
  Serial.print(redIn);
  Serial.print("\tGreen=");
  Serial.print(greenIn);
  Serial.print("\tBlue=");
  Serial.println(blueIn);
  
//  Serial.print("\tX=");
//  Serial.print(Xangle);
//  Serial.print("\tY=");
//  Serial.print(Yangle);
//  Serial.print("\tZ=");
//  Serial.println(Zangle);

//  Serial.print("X=");
//  Serial.print(Xg);
//  Serial.print("\tY=");
//  Serial.print(Yg);
//  Serial.print("\tZ=");
//  Serial.println(Zg);

// Delay not need here due to RGB delay
  delay(delayTime);  // Adjust the value to change the refresh rate. 
} 


