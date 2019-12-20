#include <Arduino.h>

String instruct;
char pos[4];
int move;
int speed=10; //vitesse moteur

void setup() {
  Serial.begin(19200);
}

void loop() {
  if(Serial.available()){
    instruct = Serial.readString();
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
  }
}