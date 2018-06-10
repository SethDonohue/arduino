//ADXL335
/********************************
ADXL335
note:vcc-->5v ,but ADXL335 Vs is 3.3V
The circuit:
      5V: VCC
analog 0: x-axis
analog 1: y-axis
analog 2: z-axis
After burning the program, open the serial monitor debugging window, where you can see the data detected being displayed. When the acceleration varies, the figure will vary accordingly.
*********************************/
//Email:support@sunfounder.com
//Website:www.sunfounder.com
//2015.5.7
const int xpin = A0;                  // x-axis of the accelerometer
const int ypin = A1;                  // y-axis
const int zpin = A2;                  // z-axis (only on 3-axis models)
void setup()
{
 // initialize the serial communications:
 Serial.begin(9600);
}
void loop()
{
 int x = analogRead(xpin);  //read from xpin
 delay(1); //
 int y = analogRead(ypin);  //read from ypin
 delay(1);  
 int z = analogRead(zpin);  //read from zpin
//float zero_G = 512.0; //ADC is 0~1023  the zero g output equal to Vs/2
float zero_G = 338.0; //ADXL335 power supply by Vs 3.3V:3.3V/5V*1024=676/2=338,by default is 338
float zero_Gx=331.5;//the zero g output of x axis
float zero_Gy=329.5;//the zero g output of y axis
float zero_Gz=340.0;//the zero g output of z axis
float scale = 67.6;  //supply valtage 5v ADXL335 Sensitivity is 330mv/g
                       //330 * 676/3.3/1000  
/*Serial.print(x); 
Serial.print("\t");
Serial.print(y);
Serial.print("\t");
Serial.print(z);  
Serial.print("\n");*/
Serial.print(((float)x - zero_Gx)/scale*9.8);  //print x value on serial monitor
Serial.print("\t");
Serial.print(((float)y - zero_Gy)/scale*9.8);  //print y value on serial monitor
Serial.print("\t");
Serial.print(((float)z - zero_Gz)/scale*9.8);  //print z value on serial monitor
Serial.print("\n");
delay(1000);  //wait for 1 second 
}
