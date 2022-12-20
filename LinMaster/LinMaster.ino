/*
Software für MASTER LIN-Bus Teilnhemer
Pin 4 wird mittels touchpin (touch0) als Taster verwendet, ESP gibt Signal mittels UART weiter an LIN-Bus Breakoutboard

Uart:   https://www.circuitbasics.com/how-to-set-up-uart-communication-for-arduino/
LIN:    https://copperhilltech.com/lin-bus-breakout-board/
Libary: https://mygit.th-deg.de/mh13366/arduino-lin-bibliothek


    WS22/23 Semester 2 Mechatronik Master
    Fabian Marschall  mafa1024
    Christoph Weiß    wech1030

IDE: Arduino IDE 2.0.3
LIN-Master: ESP32 mit UART 2 (Pin 16 & 17): Eingestellt im Header
LIN-Slave:  LOLIN32 mit UART 2 (Pin 16 & 17): Eingestellt im Header

*/


#include "LinMaster.h"
#include "HardwareSerial.h"

LinMaster lin;

unsigned char bytes = 1;
unsigned char corrupted = 0;
unsigned char* response;
unsigned char myData;
const int T_XE = 19; //Fault
const int S0 = 4;
const int CS = 0;
const int LED =  27;



void setup() {
  
  //Pins zum ansteueren des LIN-Breakoutboard
  pinMode(T_XE, OUTPUT_OPEN_DRAIN);
  pinMode(CS, OUTPUT_OPEN_DRAIN);
  pinMode(LED, OUTPUT);


  //Initialize HardwareSerial for UART Communication
  lin.begin(4800);  
  Serial.begin(9600);
  delay (200);
  
}

void loop() {
  //LIN-Breakoutboard aktivieren
  digitalWrite(CS, HIGH);
  digitalWrite(T_XE, HIGH);

  if (touchRead(S0)< 30)  //Touch/Taster einlesen, dass LED am Slave angesteuert wird
  {
      lin.sendHeader(0x10);   //LIN-Header: Slave mit Adresse wird angesprochen 
      corrupted = 0;
      response = lin.receiveResponse(lin.getPID(0x10), &bytes, &corrupted);   //Rückgabewert LIN-Bus Slave
      if (!corrupted)
      {
        Serial.println("0x10");
        Serial.print(response[0]);
      }
       delete response;
  }
  else
  {
      lin.sendHeader(0x00);   //LIN-Teilnehmer ohne Adresse wird angesprochen
  }
  delay(50);
}
