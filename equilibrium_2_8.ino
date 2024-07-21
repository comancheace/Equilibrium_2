#include <SoftwareSerial.h> // TX RX software library for bluetooth
#include <Servo.h>
#include <Wire.h> // This library allows you to communicate with I2C devices.
#include <MPU6050.h>

#define SERVO1_PIN 6
int pos;
Servo myservo;

MPU6050 sensor ;

int16_t ax, ay, az ;
int16_t gx, gy, gz ;

const int MPU_ADDR = 0x68; // I2C address of the MPU-6050. If AD0 pin is set to HIGH, the I2C address will be 0x69.

int16_t accelerometer_x, accelerometer_y, accelerometer_z; // variables for accelerometer raw data
int16_t gyro_x, gyro_y, gyro_z; // variables for gyro raw data
int16_t temperature; // variables for temperature data

char tmp_str[7]; // temporary variable used in convert function

char* convert_int16_to_str (int16_t i) { // converts int16 to string. Moreover, resulting strings will have the same length in the debug monitor.
  sprintf(tmp_str, "%6d", i);
  return tmp_str;
}

#define Lpwm_pin  11     //pin of controlling speed---- ENA of motor driver board
#define Rpwm_pin  10    //pin of controlling speed---- ENB of motor driver board
int pinLB=5;             //pin of controlling turning---- IN1 of motor driver board
int pinLF=4;             //pin of controlling turning---- IN2 of motor driver board
int pinRB=3;            //pin of controlling turning---- IN3 of motor driver board
int pinRF=2;            //pin of controlling turning---- IN4 of motor driver board

boolean goesForward = false;

int bluetoothTx = 8; // bluetooth tx to 10 pin
int bluetoothRx = 9; // bluetooth rx to 11 pin

char caracter;
String palavra;
String accelX;
String accelY;

int k1;

SoftwareSerial bluetooth(bluetoothTx, bluetoothRx);

void setup(){

  myservo.attach ( SERVO1_PIN );

  Wire.begin();
  Serial.begin(9600); 
  Serial.println("Initializing the sensor"); 
  sensor.initialize(); 
  Serial.println(sensor.testConnection()?"Successfully Connected" : "Connection failed"); 
  delay(1000); 
  Serial.println("Taking Values from the sensor");
  delay(1000);
  
  bluetooth.begin(9600);//Setup Bluetooth serial connection to android
  
  pinMode(pinLB,OUTPUT); // /pin 5
  pinMode(pinLF,OUTPUT); // pin 4
  pinMode(pinRB,OUTPUT); // pin 3
  pinMode(pinRF,OUTPUT);  // pin 2
  pinMode(Lpwm_pin,OUTPUT);  // pin 11 (PWM) 
  pinMode(Rpwm_pin,OUTPUT);  // pin 10 (PWM) 
}

void loop(){

   //Read from bluetooth and write to usb serial
  if (bluetooth.available()) // receive number from bluetooth
  {
    caracter = bluetooth.read();
    palavra = palavra + caracter;

    if (caracter == '*') {
      Serial.println(palavra);
      palavra = palavra.substring(0, palavra.length() - 1); // Delete last char *
      k1 = palavra.indexOf(',');
      accelX = palavra.substring(0, k1);
      accelY = palavra.substring(k1 + 1, -1);
      //Serial.println(accelX);
      //Serial.println(accelY);
      int x = accelX.toInt();
      int y = accelY.toInt();   
      if (x >=0) {
      advance();
      Set_Speed(80);
      }
      if (x <0) {
       back();
        Set_Speed(80);
      }
      palavra = "";
  }
sensor.getMotion6 (&ax, &ay, &az, &gx, &gy, &gz);
az = map (az, -17000, 17000, 0, 180) ;
Serial.println (az);
myservo.write (az); 
delay (0);
}
}

void Set_Speed(unsigned char pwm) //function of setting speed
{
  analogWrite(Lpwm_pin,pwm);
  analogWrite(Rpwm_pin,pwm);
}

void stop()//stop
    {
     digitalWrite(pinRB,HIGH);
     digitalWrite(pinRF,HIGH);
     digitalWrite(pinLB,HIGH);
     digitalWrite(pinLF,HIGH);  
    }

void advance()//going forward
    {
    if(!goesForward){
    Serial.println("==moveForward==");
    goesForward=true;
     digitalWrite(pinRB,HIGH);
     digitalWrite(pinRF,LOW);
     digitalWrite(pinLB,LOW);
     digitalWrite(pinLF,HIGH);
     }
    } 

void back()//back
    {
     goesForward=false;
     Serial.println("==moveBackward==");
     digitalWrite(pinRB,LOW);
     digitalWrite(pinRF,HIGH);
     digitalWrite(pinLB,HIGH);
     digitalWrite(pinLF,LOW);
    }