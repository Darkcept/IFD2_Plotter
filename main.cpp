#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <stdio.h>
#include <string.h>
#define topicIn "/plotter/stream"
#define topicOut "/plotter/ack"

#include <Adafruit_PWMServoDriver.h>
#include <math.h>
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define USMIN  600 // This is the rounded 'minimum' microsecond length based on the minimum pulse of 150
#define USMAX  2400 // This is the rounded 'maximum' microsecond length based on the maximum pulse of 600
#define NEUTRE 375
#define NEUTREM 1500
#define VITESSE 1

//on stock la valeur maximale et minimale des angles moteurs

#define SERVO3MIN 200
#define SERVO3MAX 600//x

#define SERVO2MIN 300
#define SERVO2MAX 480//y

#define SERVO1MIN 184
#define SERVO1MAX 232//z

#include <Arduino.h>

String instruct;
char pos[4];
int move;
int speed=10; //vitesse moteur

uint8_t servonum1 = 5;
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

const char* ssid = "Crunch LAB";
const char* password = "90xV@FsT";

const char* mqtt_server = "192.168.1.15";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

const int ledPin = 13;

//connection au wifi puis au serveur mqtt

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

//boucle de dessin du plotter

int draw(String instruct)
{
    int success=1;
    Serial.println(instruct);
    if (instruct=="G00") {
      speed=50; //vitesse rapide
    } else if (instruct=="G01") {
      speed=10; //vitesse ecriture
    } else {
      instruct.toCharArray(pos,5);
      switch (pos[0]) {
          //on traite le GCode pour définir si le bras doit bouger sur le moteur 1 ou le moteur 2
      case 'X':
      move=atoi(&pos[1]); //convertissage de l'instruction en int lisible par l'arduino
      Serial.println(move);
        pwm.setPWM(servonum3,0,map(move,0,240,SERVO3MIN,SERVO3MAX)); //le bras du servo correspondant bouge proportionnelement a son angle maximum
        break;
      case 'Y':
      move=atoi(&pos[1]);
      Serial.println(move);
        pwm.setPWM(servonum2,0,map(move,0,320,SERVO2MIN,SERVO2MAX));
        break;
      case 'Z':
      move=atoi(&pos[1]);
        if(move==0) { //dernier test pour baisser ou lever le stylo
          Serial.println("Stylo baissé");
          pwm.setPWM(servonum1, 0, SERVO1MAX);
        } else if(move!=0) {
          Serial.println("Stylo remonté");
          pwm.setPWM(servonum1, 0, SERVO1MIN);
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
    client.publish(topicOut, "continue");
  }
  else
  {
    client.publish(topicOut, "PROBLEME D'ECRITURE");
  }
}

void setup()
{
  pwm.begin();
  pwm.setOscillatorFrequency(27000000);  // The int.osc. is closer to 27MHz  
  pwm.setPWMFreq(50);  // Analog servos run at ~50 Hz updates
  delay(10);

  Serial.begin(9600);  

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  pinMode(ledPin, OUTPUT);
}

void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("Plotter"))
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

  }
}
