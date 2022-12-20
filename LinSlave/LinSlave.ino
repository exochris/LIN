/*
Software für SLAVE LIN-Bus Teilnhemer
Pin 15 wird als Output mit LED verwende, ESP empfängt Signal mittels UART von LIN-Bus Breakoutboard und steuert LED an

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

#include "LinSlave.h"


LinSlave lin;

const int LED = 15;
const int CS=0;
const int T_XE = 19; //Fault

void setup() {
  // put your setup code here, to run once:
  lin.begin(4800);
  pinMode(LED, OUTPUT);

  //Pins zum ansteueren des LIN-Breakoutboard
  pinMode(T_XE, OUTPUT_OPEN_DRAIN);
  pinMode(CS, OUTPUT_OPEN_DRAIN);
}

void loop() {
  //LIN-Breakoutboard aktivieren
  digitalWrite(CS, HIGH);
  digitalWrite(T_XE, HIGH);


  //Slave liest LIN Bus aus und liest ID ein (ID wurde in LinSlave.h gesetzt)
  unsigned char corrupted = 0;
  unsigned char pid = lin.listen(&corrupted);
  unsigned char id =  pid & PID_TO_ID_MASK;
  unsigned char MSG = 0x18;

  //Wenn vom MAster gesendete ID mit Slave ID übereinstimmt
  if (id == SLAVE_ID)
  {
      unsigned char myData = (unsigned char)(MSG);  //Nachricht (kann auch Sensor wert sein, dient hier nur als Rückgabewert)
      unsigned char checksum = lin.getEnhancedChecksum(&myData, 1, pid); //Checksum erstellen
    
      //Send the data
      lin.send(myData);   //Nachricht zurück senden
      lin.send(checksum); //Checksum zurücksenden
      
      digitalWrite(LED, HIGH);    //LED einschalten
      //delay(500);
      //digitalWrite(LED, LOW);
      //delay(500);
      
  }
  else    //LED ausschalten
  {
    digitalWrite(LED, LOW);  
  }    
}
