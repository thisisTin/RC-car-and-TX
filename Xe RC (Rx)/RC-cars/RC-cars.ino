#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>
//L298N pin definitions
#define IN1	2
#define IN2	4
#define IN3	6
#define IN4	9
#define ENA 3
#define ENB 10
#define MAX_SPEED 255 //từ 0-255
#define MIN_SPEED 0

int pwm1,pwm2;
//create an RF24 object
RF24 radio(7, 8);  // CE, CSN

//address through which two modules communicate.
const byte address[6] = "00001";

//Signal Receive
struct Signal {
byte throttle;
byte steer;
byte servoPos;
};

Signal data;

//Servo definitions
Servo myservo;  
int servoPin = 5;

void ResetData()
{
// Define the initial value of each data input.
// The middle position for Potentiometers. (254/2=127) 
data.throttle = 125; // Motor Stop | 
data.steer = 129;  // Center | 
data.servoPos = 89;
}

void setup()
{
  while (!Serial);
  Serial.begin(9600);
  radio.begin();
  
  //set the address
  radio.openReadingPipe(0, address);
  
  //Set module as receiver
  radio.startListening();
  
}

void loop()
{
  //Read the data if available in buffer
  if (radio.available())
  {
    radio.read(&data, sizeof(Signal));
    int ythrottle=data.throttle;
    int xsteer=data.steer;
    int servoPos=data.servoPos;

    if(ythrottle > 140)
      {
        pwm1 = ythrottle;
        pwm2 = ythrottle;

        digitalWrite(IN1,HIGH);
        digitalWrite(IN2,LOW);
        digitalWrite(IN3,HIGH);
        digitalWrite(IN4,LOW);
        analogWrite(ENA, pwm1);
        analogWrite(ENB, pwm2);
        // Serial.println("motor FW");
      }
    else if(ythrottle < 120)
      {
        pwm1 = ythrottle;
        pwm2 = ythrottle;
        digitalWrite(IN1,LOW);  
        digitalWrite(IN2,HIGH);
        digitalWrite(IN3,LOW);
        digitalWrite(IN4,HIGH);
        analogWrite(ENA, pwm1);
        analogWrite(ENB, pwm2);
        // Serial.println("motor RV");
      }
    else
      {
        pwm1 = 0;
        pwm2 = 0;
        digitalWrite(IN1,LOW);
        digitalWrite(IN2,LOW);
        digitalWrite(IN3,LOW);
        digitalWrite(IN4,LOW);
        analogWrite(ENA, pwm1);
        analogWrite(ENB, pwm2);
        // Serial.println("motor stop");
      }
    
    if (xsteer > 140) 
    {
    pwm1= xsteer;
    pwm2= xsteer;
    digitalWrite(IN1, HIGH);        // Động cơ trái tiến
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);         // Động cơ phải lùi
    digitalWrite(IN4, HIGH);
    analogWrite(ENA, pwm1);  // Động cơ trái giữ tốc độ
    analogWrite(ENB, pwm2 / 2);  // Giảm tốc độ động cơ phải
    } 
    else if (xsteer < 122) 
    {
    pwm1= xsteer;
    pwm2= xsteer;
    digitalWrite(IN1, LOW);         // Động cơ trái lùi
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH);        // Động cơ phải tiến
    digitalWrite(IN4, LOW);
    analogWrite(ENA, pwm1 / 2);  // Giảm tốc độ động cơ trái
    analogWrite(ENB, pwm2);  // Động cơ phải giữ tốc độ 
    }     
    Serial.println(pwm1);
    Serial.println(pwm2);
  }
}
