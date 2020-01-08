#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <stdio.h>
#include <string.h>
//#include <Adafruit_BME280.h>
//#include <Adafruit_Sensor.h>
//#include "ArduinoJson-v6.13.0.h"
#define topicIn "/plotter/stream"
#define topicOut "/plotter/ack"
//#define topicX "/plotter/posX"
//#define topicY "/plotter/posY"

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

//proto
/*
void setup_wifi();
void callback(char* topic, byte* message, unsigned int length);
int draw(string message);
void setup();
void reconnect();
void loop();
*/

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

#include <Arduino.h>

String instruct;
char pos[4];
int move;
int speed=10; //vitesse moteur

uint8_t servonum1 = 0;
uint8_t servonum2 = 4;
uint8_t servonum3 = 3;
int joysticky1 = 34;
int joystickx1 = 39;
int x_pos1;
int x_pos2;
int y_pos1;
int y_pos2;
int pos1=NEUTRE,pos2=NEUTRE,pos3=NEUTRE;
int clic=1;
int pos33;

const char* ssid = "AndroidM";
const char* password = "fpjj5481";

const char* mqtt_server = "192.168.43.137";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

//float posX=-1;
//float posY=-1;

// LED Pin
const int ledPin = 13;

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
/*X-0.5 Y0.
Z0.1
G01 Z0. F5.
G02 X0. Y0.5 I0.5 J0. F2.5*/
int mode=-1;


int draw(String instruct)
{
    int success=1;

    Serial.println(instruct);
    if (instruct=="G00") {
      speed=50; //vitesse rapide
      Serial.println("Gotta go fast");
    } else if (instruct=="G01") {
      speed=10; //vitesse ecriture
      Serial.println("Gotta go slow");
    } else {
      instruct.toCharArray(pos,5);
      switch (pos[0]) {
      case 'X':
      move=atoi(&pos[1]);
      Serial.println("Move X");
      Serial.println(move);
        pwm.setPWM(servonum3, 0, (move*380)/24-200);
        break;
      case 'Y':
      move=atoi(&pos[1]);
      Serial.println("Move Y");
      Serial.println(move);
        pwm.setPWM(servonum2, 0, (move*480)/32-322);
        break;
      case 'Z':
      move=atoi(&pos[1]);
        if(move==0) {
          Serial.println("Stylo baissé");
          //pwm.setPWM(servonum1, 0, );
        } else if(move!=0) {
          Serial.println("Stylo remonté");
          //pwm.setPWM(servonum1, 0, );
        }
        break;
      default:
        break;
      }
    }
  return success;
}


void callback(char* topic, byte* message, unsigned int length)
{
  Serial.print(topic);
  Serial.print(" | ");
  String messageTemp;

  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();
  char msg[40];
  strcpy(msg,messageTemp.c_str());

  int temp=draw(messageTemp);

  if(temp==1) 
  {
    Serial.print("drew | ");
    Serial.print(messageTemp);
    Serial.println();
    client.publish(topicOut, "continue");
  }
  else
  {
    client.publish(topicOut, "PROBLEME D'ECRITURE");
  }
}

void setup()
{
  
  pinMode (joystickx1, INPUT) ;                     
  pinMode (joysticky1, INPUT) ;
  pwm.begin();
  pwm.setOscillatorFrequency(27000000);  // The int.osc. is closer to 27MHz  
  pwm.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates
  delay(10);

  Serial.begin(9600);  

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  pinMode(ledPin, OUTPUT);
}

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

void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client"))
    {
      Serial.println("connected");
      // Subscribe
      client.subscribe(topicIn);
      client.publish(topicOut, "présent");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop()
{
  if (!client.connected()) reconnect();
  client.loop();

  long now = millis();
  if (now - lastMsg > 5000)
  {
    lastMsg = now;

    //client.publish(topicX,gcvt(posX));
    //client.publish(topicY,gcvt(posY));

  }
}