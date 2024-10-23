//  4 Channel Receiver 
//  PWM output on L298N
//  UI A4 A5 LCD display
//
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
LiquidCrystal_I2C lcd(0x27,  16, 2);
//L298N pin definitions
#define IN1	5
#define IN2	4
#define IN3	3
#define IN4	2
#define MAX_SPEED 255 //từ 0-255
#define MIN_SPEED 0

int progress,i;
const char text[15];
struct Signal {
byte throttle;      
byte steer;
};

Signal data;

const uint64_t pipeIn = 0xE9E8F0F0E1LL;
RF24 radio(7, 8); 

void ResetData()
{
// Define the initial value of each data input.
// The middle position for Potentiometers. (254/2=127) 
data.throttle = 127; // Motor Stop | 
data.steer = 127;  // Center | 
}

byte zero[] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};
byte one[] = {
  B10000,
  B10000,
  B10000,
  B10000,
  B10000,
  B10000,
  B10000,
  B10000
};

byte two[] = {
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000
};

byte three[] = {
  B11100,
  B11100,
  B11100,
  B11100,
  B11100,
  B11100,
  B11100,
  B11100
};

byte four[] = {
  B11110,
  B11110,
  B11110,
  B11110,
  B11110,
  B11110,
  B11110,
  B11110
};

byte five[] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};


 void updateProgressBar(unsigned long count, unsigned long totalCount, int lineToPrintOn)
 {
    double factor = totalCount/80.0;          //See note above!
    int percent = (count+1)/factor;
    int number = percent/5;
    int remainder = percent%5;
    if(number > 0)
    {
       lcd.setCursor(number-1,lineToPrintOn);
       lcd.write(5);
    }
   
       lcd.setCursor(number,lineToPrintOn);
       lcd.write(remainder);   
 }



void setup()
{
  Serial.begin(9600);
  //LCD initiate
  lcd.init();
  lcd.backlight();
  
  lcd.createChar(0, zero);
  lcd.createChar(1, one);
  lcd.createChar(2, two);
  lcd.createChar(3, three);
  lcd.createChar(4, four);
  lcd.createChar(5, five);
 
  delay(1000);
  lcd.clear();
  lcd.setCursor(3,0);
  lcd.print("Loading RX");
  for(int i=0; i <= 100; i=i+4)
  {
    updateProgressBar(i, 100, 1);   //This line calls the subroutine that displays the progress bar.  The 3 arguments are the current count, the total count and the line you want to print on.
    delay(200);
  }
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Waiting 4 NRF24");
  //Set the pins for each PWM signal to L298N driver
	pinMode(IN1, OUTPUT);
	pinMode(IN2, OUTPUT);
	pinMode(IN3, OUTPUT);
	pinMode(IN4, OUTPUT);

  //Configure the NRF24 module
  // ResetData();
  radio.begin();
  radio.setAutoAck(1); //Hong biet :)))
  radio.setDataRate(RF24_1MBPS); //Max transmitt speed
  radio.openReadingPipe(1,pipeIn); //Open port
  radio.setChannel(108);  // Set the transmitting channel to CH108
  radio.startListening(); //start the radio comunication for receiver 
}

unsigned long lastRecvTime = 0;

void recvData()
{
if (radio.available()) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("NRF24 connected!");
    radio.read(&data, sizeof(Signal));
    // char text[32] = {0};
    // radio.read(&text, sizeof(text));
    // lcd.print(text);
    // lcd.setCursor(0,0);
    radio.read(&text, sizeof(text));
    Serial.println(text);
    // lastRecvTime = millis();   
}
else
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Waiting 4 NRF24");
}
void motor_1_Dung() {
	digitalWrite(IN1, LOW);
	digitalWrite(IN2, LOW);
}
 
void motor_2_Dung() {
	digitalWrite(IN3, LOW);
	digitalWrite(IN4, LOW);
}
 
void motor_1_Tien(int speed) { //speed: từ 0 - MAX_SPEED
	speed = constrain(speed, MIN_SPEED, MAX_SPEED);//đảm báo giá trị nằm trong một khoảng từ 0 - MAX_SPEED - http://arduino.vn/reference/constrain
	digitalWrite(IN1, HIGH);// chân này không có PWM
	analogWrite(IN2, 255 - speed);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("FW1");
}
 
void motor_1_Lui(int speed) {
	speed = constrain(speed, MIN_SPEED, MAX_SPEED);//đảm báo giá trị nằm trong một khoảng từ 0 - MAX_SPEED - http://arduino.vn/reference/constrain
	digitalWrite(IN1, LOW);// chân này không có PWM
	analogWrite(IN2, speed);
  lcd.setCursor(0,0);
  lcd.print("RV1");
}
 
void motor_2_Tien(int speed) { //speed: từ 0 - MAX_SPEED
	speed = constrain(speed, MIN_SPEED, MAX_SPEED);//đảm báo giá trị nằm trong một khoảng từ 0 - MAX_SPEED - http://arduino.vn/reference/constrain
	analogWrite(IN3, speed);
	digitalWrite(IN4, LOW);// chân này không có PWM
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("FW2");
}
 
void motor_2_Lui(int speed) {
	speed = constrain(speed, MIN_SPEED, MAX_SPEED);//đảm báo giá trị nằm trong một khoảng từ 0 - MAX_SPEED - http://arduino.vn/reference/constrain
	analogWrite(IN4, 255 - speed);
	digitalWrite(IN3, HIGH);// chân này không có PWM
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("RV2");
}
void loop()
{
recvData();
unsigned long now = millis();
if ( now - lastRecvTime > 1000 ) {
ResetData(); // Signal lost.. Reset data |
}
}
int ch_width_1 = map(data.throttle, 0, 255, 1000, 2000);     // pin D2 (PWM signal)
int ch_width_2 = map(data.steer,    0, 255, 1000, 2000);     // pin D3 (PWM signal)

// Write the PWM signal 
// ch1.writeMicroseconds(ch_width_1);
// ch2.writeMicroseconds(ch_width_2);
// ch3.writeMicroseconds(ch_width_3);
// ch4.writeMicroseconds(ch_width_4);

// if(ch_width_1 > 130)
//   {
//     motor_1_tien(ch_width_1);
//     motor_2_tien(ch_width_2);
//     delay(0);
//   }
// if(ch_width_1 < 115)
//   {
//     motor_1_lui(ch_width_1);
//     motor_2_lui(ch_width_2);
//     delay(0);
//   }  
