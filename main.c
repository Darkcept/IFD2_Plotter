#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// called this way, it uses the default address 0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
// you can also call it with a different address you want
//Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x41);
// you can also call it with a different address and I2C interface
//Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40, Wire);

// Depending on your servo make, the pulse width min and max may vary, you 
// want these to be as small/large as possible without hitting the hard stop
// for max range. You'll have to tweak them as necessary to match the servos you
// have!
#define SERVOMIN  150 // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  600 // This is the 'maximum' pulse length count (out of 4096)
#define USMIN  600 // This is the rounded 'minimum' microsecond length based on the minimum pulse of 150
#define USMAX  2400 // This is the rounded 'maximum' microsecond length based on the maximum pulse of 600
#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates
#define NEUTRE 375
#define NEUTREM 1500
#define VITESSE 1
#define SERVO3MIN 200
#define SERVO3MAX 380
#define SERVO2MIN 322
#define SERVO2MAX 480
#define SERVO1MIN
#define SERVO1MAX
// our servo # counter
uint8_t servonum = 0;
uint8_t servonum2 = 4;
uint8_t servonum3 = 3;
int joysticky1 = 34;
int joystickx1 = 39;
int joysticky2 = 36;
int joystickx2 = 4;
int clic1 = 23;
int clic2 = 21;
int x_pos1;
int x_pos2;
int y_pos1;
int y_pos2;
int pos1=NEUTRE,pos2=NEUTRE,pos3=NEUTRE;
int clic=1;
int pos33;

void setup() 
{
    pinMode (joystickx1, INPUT) ;                     
    pinMode (joysticky1, INPUT) ;
    pinMode (joystickx2, INPUT) ;                     
    pinMode (joysticky2, INPUT) ;
    pinMode (clic1, INPUT_PULLUP) ;                     
    pinMode (clic2, INPUT_PULLUP) ;
    Serial.begin(9600);
    Serial.println("3 channel Servo test!");
    pwm.begin();
    // In theory the internal oscillator is 25MHz but it really isn't
    // that precise. You can 'calibrate' by tweaking this number till
    // you get the frequency you're expecting!
    pwm.setOscillatorFrequency(27000000);  // The int.osc. is closer to 27MHz  
    pwm.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates
    delay(10);
}

// You can use this function if you'd like to set the pulse length in seconds
// e.g. setServoPulse(0, 0.001) is a ~1 millisecond pulse width. It's not precise!
void setServoPulse(uint8_t n,double pulse) {
  double pulselength;
  pulselength = 1000000;   // 1,000,000 us per second
  pulselength /= SERVO_FREQ;   // Analog servos run at ~60 Hz updates
  Serial.print(pulselength); Serial.println(" us per period"); 
  pulselength /= 4096;  // 12 bits of resolution
  Serial.print(pulselength); Serial.println(" us per bit"); 
  pulse *= 1000000;  // convert input seconds to us
  pulse /= pulselength;
  Serial.println(pulse);
  pwm.setPWM(n, 0, pulse);
}

void loop() {
    Serial.println(pos1);
    //Serial.println(pos2);
    if(x_pos2!=analogRead(joystickx2))
    {
    if(pos3>600)
    {
        pos3 = 600;
        
    }  
    else if(pos3<80)
    {
        pos3=80;
    }
    else
    {
      switch(x_pos2)
    {
        case 0 ... 1800 : 
            pos3=pos3+VITESSE;
            pwm.setPWM(servonum, 0, pos3);
        break;
        case 1801 ... 1949 : ;
        break;
        case 1950 ... 4095 : 
            pos3=pos3-VITESSE;
            pwm.setPWM(servonum, 0, pos3);
        break;
        default:;
    }
    }
    }
    if(pos2>SERVO2MAX)
    {
        pos2 = SERVO2MAX;
        
    }  
    else if(pos2<SERVO2MIN)
    {
        pos2=SERVO2MIN;
    }
    else
    {
      switch(y_pos1)
    {
        case 0 ... 1800 : 
            pos2=pos2+VITESSE;
            pwm.setPWM(servonum2, 0, pos2);
        break;
        case 1801 ... 1949 : ;
        break;
        case 1950 ... 4095 : 
            pos2=pos2-VITESSE;
            pwm.setPWM(servonum2, 0, pos2);
        break;
        default:;
    }
    }
    if(pos1>SERVO3MAX)
    {
        pos1=SERVO3MAX;
    }  
    else if(pos1<SERVO3MIN)
    {
        pos1=SERVO3MIN;
    }
    else
    {
      switch(x_pos1)
    {
        case 0 ... 1800: 
            pos1=pos1+VITESSE;
            pwm.setPWM(servonum3, 0, pos1);
        
        break;
        case 1801 ... 1949 : ;
        break;
        case 1950 ... 4095 : 
            pos1=pos1-VITESSE;
            pwm.setPWM(servonum3, 0, pos1);
        
        break;
        default:;
    }
    }
    delay(10);
    x_pos1 = analogRead (joystickx1) ;  
    y_pos1 = analogRead (joysticky1) ;
    x_pos2 = analogRead (joystickx2) ;  
    y_pos2 = analogRead (joysticky2) ;
    /*
    for (uint16_t pulselen = SERVOMIN; pulselen < SERVOMAX; pulselen=pulselen+10) 
    {
        pwm.setPWM(servonum, 0, pulselen);
        pwm.setPWM(servonum2, 0, pulselen);
        pwm.setPWM(servonum3, 0, pulselen);
    }
    delay(500);
    for (uint16_t pulselen = SERVOMAX; pulselen > SERVOMIN; pulselen--) 
    {
        pwm.setPWM(servonum, 0, pulselen);
        pwm.setPWM(servonum2, 0, pulselen);
        pwm.setPWM(servonum3, 0, pulselen);
    }
    delay(500);
    // Drive each servo one at a time using writeMicroseconds(), it's not precise due to calculation rounding!
    // The writeMicroseconds() function is used to mimic the Arduino Servo library writeMicroseconds() behavior. 
    for (uint16_t microsec = USMIN; microsec < USMAX; microsec++) 
    {
        pwm.writeMicroseconds(servonum, microsec);
        pwm.writeMicroseconds(servonum2, microsec);
        pwm.writeMicroseconds(servonum3, microsec);
    }
    delay(500);
    for (uint16_t microsec = USMAX; microsec > USMIN; microsec--) 
    {
        pwm.writeMicroseconds(servonum, microsec);
        pwm.writeMicroseconds(servonum2, microsec);
        pwm.writeMicroseconds(servonum3, microsec);
    }
    delay(500);
    */
}
