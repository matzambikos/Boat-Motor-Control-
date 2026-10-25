/*  motor boat controller
 *  this program uses an ESP32 to control
 *  a dc propeller motor (forward and reverse)
 *  and a rudder servo motor.
 *  The ESP32 connects to a discreet mosfet transistor
 *  H bridge for the dc propeller motor and can control
 *  it with pwm. The rudder servo motor is controlled
 *  via the ESP32Servo library.
 *  The ESP32 exchanges information with an android mobile
 *  which uses the BLUETOOTH SERIAL TERMINAL application
 *  program. The ESP32 has a builtin bluetooth device 
 *  Available commands are : 
 *  1. "F"  dc motor forward
 *  2. "B"  dc motor reverse
 *  3. "N"  dc motor stops
 *  4. "L"  servo motor small angle to the left
 *  5. "R"  servo motor small angle to the right
 *  6. "A"  servo motor to the straight position
 *  7. "S"  changes the speed of the dc propeller motor
 *     Each sending of the "S" command cycles between  
 *     3 speeds :  a. low   b. medium  and c. high.
 *     Specific values of speed depend on the motor used.
 *  The mobile application has corresponding "buttons" 
 *  for the above commands.
 */



#include <ESP32Servo.h>            // library to control the servo motor
#include <BluetoothSerial.h>       // library for the bluetooth communication

#define Cpoint 85             // define the three points of the servo motor (center, left and right)
#define left 120
#define right 50


const int mIN1=33;             // define arduino pins for :  mINx  x=1,2,3,4  for the control inputs         
const int mIN2=25;             // of the propeller motor H bridge control
const int mIN3=26;
const int mIN4=27;
const int mEN=14;              // this is the arduino PWM output to control the speed of the propeller motor 
const int mPWM=36;            // this is the dummy arduino input to "read" the pwm signal
const int ServoPin=23;
int mSpeed1 = 50;             // define the three values for the propeller motor speed
int mSpeed2 = 110;
int mSpeed3 = 250;
int FlSpeed = 1;
String Response1 = "STOP ";   // these are all the possible mobile phone blue tooth commands 
String Response2 = "FORWARD ";
String Response3 = "REVERSE ";
String Response4 = "STRAIGHT ";
String Response5 = "LEFT ";
String Response6 = "RIGHT ";
String Response7 = "SPEED LOW ";
String Response8 = "SPEED MEDIUM ";
String Response9 = "SPEED HIGH ";   
String Resp1 = "";                 // various other variables
String Resp2 = "";
String Resp3 = ""; 
String Response  = "";
int    Flfw =0;                    // if one then motor moves forward 
int    Flbw =0;                    // if one then motor moves in reverse

BluetoothSerial SerialBT;
Servo servo;                      // initialise the servo motor 


void setup() 
{
  pinMode(mIN1, OUTPUT);         // set up arduino pins as outputs or inputs
  pinMode(mIN2, OUTPUT);
  pinMode(mIN3, OUTPUT);
  pinMode(mIN4, OUTPUT);
  pinMode(mEN, OUTPUT);
  pinMode(mPWM, INPUT);
  digitalWrite(mIN1,LOW);        // set initial values for arduino output pins 
  digitalWrite(mIN2,LOW);
  digitalWrite(mIN3, LOW);
  digitalWrite(mIN4, LOW);
  digitalWrite(mEN,LOW);
  Serial.begin(115200); 
  SerialBT.begin("ESP32_Test");    //Bluetooth device name          
  //mySerial.begin(9600);          // set baud rate for the software serial port (communication with HC-05)
  //ESP32PWM::allocateTimer(0);
 // ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  servo.setPeriodHertz(50);
  servo.attach(ServoPin, 700, 2300);               // set arduino pin 23 for the servo motor control 
  servo.write(Cpoint);           // set the initial position of the servo motor and the speed of the propeller motor
  analogWrite(mEN,mSpeed1);
  Resp1 = Response1;
  Resp2 = Response4;
  Resp3 = Response7;
  Response = Resp1 + Resp2 + Resp3;
  //mySerial.print(Response);      // send to the mobole phne the initial values of propeller motor direction and speed and the servo motor initial position
  SerialBT.print(Response);   
}

void resst()                    // reset all H bridge control signals (stop motor)
{
  digitalWrite(mIN1, LOW);
  digitalWrite(mIN2, LOW);
  digitalWrite(mIN3, LOW);
  digitalWrite(mIN4, LOW);
}


void loop()
{
if(Flfw == 1 || Flbw == 1)
  {
    if(digitalRead(mPWM) == LOW)
    {
      resst();
    }  
    else if(digitalRead(mPWM) == HIGH)
    {
      if(Flfw == 1)
      {
        digitalWrite(mIN2, LOW);
        digitalWrite(mIN4, LOW);
        digitalWrite(mIN1, HIGH);
        digitalWrite(mIN3, HIGH);
      }  
      else if(Flbw == 1)
      {
        digitalWrite(mIN1, LOW);
        digitalWrite(mIN3, LOW);
        digitalWrite(mIN2, HIGH);
        digitalWrite(mIN4, HIGH);     
      }
      
    }
  }
  //if(mySerial.available())  // Send data only when you receive data:
  if(SerialBT.available())
  {
    //char value = mySerial.read();
    char value = SerialBT.read();
    //Serial.print(value);
    if(value=='F')                 // command "forward"
    {
      resst();
      //digitalWrite(mIN1,HIGH);
      //digitalWrite(mIN3, HIGH);
      Flbw =0;
      Flfw =1;
      Resp1 = Response2;
      Response = Resp1 + Resp2 + Resp3;
      //mySerial.print(Response);
      SerialBT.print(Response);   
      delay(10);
    }
    else if(value=='B')            // command "reverse"
    {
      resst();
      //digitalWrite(mIN2, HIGH);
      //digitalWrite(mIN4, HIGH);
      Flfw =0;
      Flbw =1;
      Resp1 = Response3;
      Response = Resp1 + Resp2 + Resp3;
      //mySerial.print(Response);
      SerialBT.print(Response);   
      delay(10);
    }
    else if(value=='R')            // command "right"
    {
      servo.write(Cpoint);
      delay(100);
      servo.write(right);
      Resp2 = Response6;
      Response = Resp1 + Resp2 + Resp3;
      //mySerial.print(Response);
      SerialBT.print(Response);   
      delay(100);
    }
    else if(value=='L')            // command "left"
    {
      servo.write(Cpoint);
      delay(100);
      servo.write(left);
      Resp2 = Response5;
      Response = Resp1 + Resp2 + Resp3;
      //mySerial.print(Response);
      SerialBT.print(Response);   
      delay(100);
    }
    else if(value=='N')            // command "stop"
    {
      resst();
      Flfw = 0;
      Flbw = 0;
      servo.write(Cpoint);
      Resp1 = Response1;
      Response = Resp1 + Resp2 + Resp3;
      //mySerial.print(Response);
      SerialBT.print(Response);   
      delay(10);
    }
    else if(value=='S')            // command "align"
    {
      servo.write(Cpoint);
      Resp2 = Response4;
      Response = Resp1 + Resp2 + Resp3;
      //mySerial.print(Response);
      SerialBT.print(Response);   
      delay(10);
    }
    else if(value=='V')            // command "set speed"
    {
      if(FlSpeed==1)
      {
        analogWrite(mEN,mSpeed2);  // speed2 = medium speed
        Resp3 = Response8;
        Response = Resp1 + Resp2 + Resp3;
        // mySerial.print(Response);
        SerialBT.print(Response);   
        FlSpeed = 2;
      }
      else if(FlSpeed==2)
      {
        analogWrite(mEN,mSpeed3);   // speed 3 = high speed
        Resp3 = Response9;
        Response = Resp1 + Resp2 + Resp3;
        //mySerial.print(Response);
        SerialBT.print(Response);   
        FlSpeed = 3;
      }
      else if(FlSpeed==3)
      {
        analogWrite(mEN,mSpeed1);    // speed 1 = low speed (default)
        Resp3 = Response7;
        Response = Resp1 + Resp2 + Resp3;
        //mySerial.print(Response);
        SerialBT.print(Response);   
        FlSpeed = 1;
      }
    }
  }
}
