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

//proto
/*
void setup_wifi();
void callback(char* topic, byte* message, unsigned int length);
int draw(string message);
void setup();
void reconnect();
void loop();
*/

#include <Arduino.h>

String instruct;
char pos[4];
int move;
int speed=10; //vitesse moteur


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
        //bouger les moteur sur X
        break;
      case 'Y':
      move=atoi(&pos[1]);
      Serial.println("Move Y");
      Serial.println(move);
        //bouger les moteur sur Y
        break;
      case 'Z':
      move=atoi(&pos[1]);
        if(move==0) {
          Serial.println("Stylo baissé");
          //baisser le stylo
        } else if(move!=0) {
          Serial.println("Stylo remonté");
          //monter le stylo
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