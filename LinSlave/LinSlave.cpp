/* #######################################################################

    Copyright (C) 2020  Michael Hollmer

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

   ######################################################################## */

/*! ###########################################################
 *
 * @file LinSlave.cpp
 *
 * @author Michael Hollmer
 *
 * @date 10.05.2020
 *
########################################################### */


#include "LinSlave.h"

HardwareSerial SerialPort(2); // use UART2 // Noch als Privat Attribut anlegen geht iwi nicht

LinSlave::LinSlave()
{

}

void LinSlave::begin(unsigned int baudRate)
{
    //To pull up the LIN bus set D7 to HIGH
    //pinMode(7, OUTPUT);
    //digitalWrite(7, HIGH);
    
	//Start serialPort
  //Serial.begin(baudRate);
  SerialPort.begin(baudRate, SERIAL_8N1, 16, 17);
	//Save the baud rate
	_baudRate = baudRate;
}

unsigned char LinSlave::listen(unsigned char* corrupted)
{
    //Get the header from the master here
    unsigned char syncByte = 0;
    unsigned char pid = 0; 

    //Wait for the Sync Byte
    while(syncByte != 0x55)
    {
        //As long as their is no data on the bus we do not read
        while(!SerialPort.available()) { }

        //Get the byte from the SerialPort read buffer
        syncByte = SerialPort.read();
    }

    //If we received the Sync-Byte we wait here for the PID
    while(!SerialPort.available()) { }

    //Get the pid
    pid = SerialPort.read();

    //Check if the pid matches with the id
    if (this->getPID((pid & PID_TO_ID_MASK) != pid))
    {
        *corrupted = 1;
    }

    return pid;
}

void LinSlave::send(unsigned char data)
{
	SerialPort.write(data);
}

unsigned char LinSlave::getClassicChecksum(unsigned char* data, unsigned char dataBytes)
{
    //Calculate the classic checksum

    unsigned int checksum = 0;
    
    //Add every byte to the checksum. If the checksum exceeds 255, we substract 255 from it
    for (unsigned char i = 0; i < dataBytes; i++)
    {
        checksum = checksum + data[i];
        if (checksum > 255)
        {
            checksum = checksum - 255;
        }
    }

    //Return the inverted bits of the checksum
    return ~(unsigned char)(checksum);
}

unsigned char LinSlave::getEnhancedChecksum(unsigned char* data, unsigned char dataBytes, unsigned char pid)
{
    //Calculate the enhanced checksum
    unsigned int checksum = pid;

    //Add every byte to the checksum. If the checksum exceeds 255, we substract 255 from it
    for (unsigned char i = 0; i < dataBytes; i++)
    {
        checksum = checksum + data[i];
        if (checksum > 255)
        {
            checksum = checksum - 255;
        }
    }

    //Return the inverted bits of the checksum
    return ~(unsigned char)(checksum);
}

/* ################# Private ################## */

unsigned char LinSlave::getPID(unsigned char idNumber)
{
    //The PID field consists of 6 data bits and 2 parity bits
    //X  X  D5 D4 D3 D2 D1 D0

    unsigned char p0 = ((idNumber) & 1) ^ ((idNumber >> 1) & 1) ^ ((idNumber >> 2) & 1) ^ ((idNumber >> 4) & 1);
    unsigned char p1 = (~(((idNumber >> 1) & 0x1) ^ ((idNumber >> 3) & 0x1) ^ ((idNumber >> 4) & 0x1) ^ ((idNumber >> 5) & 0x1))) & 0x1;

    //P1 P0 D5 D4 D3 D2 D1 D0
    idNumber = ((idNumber | (p0 << 6)) | (p1 << 7));

    return idNumber;
}
