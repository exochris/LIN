#include "HardwareSerial.h"
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
 * @file LinMaster.cpp
 *
 * @author Michael Hollmer
 *
 * @date 10.05.2020
 *
########################################################### */



#include "LinMaster.h"

HardwareSerial SerialPort(2); // use UART2 // Noch als Privat Attribut anlegen geht iwi nicht

LinMaster::LinMaster()
{

}

void LinMaster::begin(unsigned int baudRate)
{
    //To pull up the LIN bus set D7 to HIGH
    //pinMode(7, OUTPUT);
    //digitalWrite(7, HIGH);
  
	//Safe the baud rate, because we need it later to calculate the bitTime for the break block
    _baudRate = baudRate;

    SerialPort.begin(baudRate, SERIAL_8N1, 16, 17);
    
  //Serial.begin(baudRate);
	//SerialPort.write("Master: Started Lin communication\n");
}

void LinMaster::sendHeader(unsigned char idNumber)
{
	//The header consists of the break signal, the sync field and the PID

	//First we send the break signal
	this->sendBreak();

	//Now we send the sync field
	SerialPort.write(0x55);
	
	//Send the PID
    unsigned char pid = this->getPID(idNumber);
    SerialPort.write(pid);

    //Because the RX Pin of the master is also connected to the LIN bus we will receive the message we just send
    //Here we want to remove it from the Serial input buffer
    unsigned char recBuffer = 0;
    unsigned int timeout = 0;
    while (recBuffer != pid)
    {
        timeout = 0;
        while (!SerialPort.available())
        {
            delay(1);
            timeout++;
            if (timeout > TIMEOUT_MILLISEC)
            {
                return;
            }
        }

        recBuffer = SerialPort.read();
    }
}

unsigned char* LinMaster::receiveResponse(unsigned char pid, unsigned char* receivedBytes, unsigned char* corrupted)
{
    //Now we can try to receive the data from the slave
    unsigned int timeout = 0;
    unsigned char* receiveBuffer = new unsigned char[9]; //8 Databytes + 1 Checksum byte
    unsigned char bytes = 0;
    //Try to read 9 Bytes from the bus
    for (bytes = 0; bytes < 9; bytes++)
    {
        timeout = 0;
        while (!SerialPort.available())
        {
            delay(1);
            timeout++;
            if (timeout > TIMEOUT_MILLISEC)
            {
                goto done;
            }
        }

        receiveBuffer[bytes] = SerialPort.read();
    }
    done:

    //Store the received bytes
    *receivedBytes = bytes;

    //If we were unable to read from the LIN bus we exit here
    if (*receivedBytes == 0)
    {
        return NULL;
    }

    //Do the checksum calculation here
    #ifdef USE_ENHANCED_CHECKSUM
        //Check here if the enhanced checksum is correct
        if (this->getEnhancedChecksum(receiveBuffer, *receivedBytes - 1, pid) == receiveBuffer[*receivedBytes - 1])
        {
            return receiveBuffer;
        }
    #elif
        //Check here if the classic checksum is correct
        if (this->getClassicChecksum(receiveBuffer, *receivedBytes - 1) == receiveBuffer[*receivedBytes - 1])
        {
            return receiveBuffer;
        }
    #endif



    //If a checksum error occured we return the data anyways but we set the corrupted var to true
    *corrupted = 1;
    return receiveBuffer;
}

void LinMaster::send(unsigned char data)
{
    SerialPort.write(data);
}

unsigned char LinMaster::getPID(unsigned char idNumber)
{
    //The PID field consists of 6 data bits and 2 parity bits
    //X  X  D5 D4 D3 D2 D1 D0

    unsigned char p0 = ((idNumber) & 1) ^ ((idNumber >> 1) & 1) ^ ((idNumber >> 2) & 1) ^ ((idNumber >> 4) & 1);
    unsigned char p1 = (~(((idNumber >> 1) & 0x1) ^ ((idNumber >> 3) & 0x1) ^ ((idNumber >> 4) & 0x1) ^ ((idNumber >> 5) & 0x1))) & 0x1;

    //P1 P0 D5 D4 D3 D2 D1 D0
    idNumber = ((idNumber | (p0 << 6)) | (p1 << 7));

    return idNumber;
}

/* ####################### Private ############################# */

void LinMaster::sendBreak()
{
	unsigned int bitTime = (1000000 / _baudRate); //The time it takes to send one bit in microseconds
	unsigned int breakDuration = bitTime * HEADER_BREAK_BITS; //The total time of our break signal

    //In order to set the TX_PIN to HIGH or LOW we need to end the SerialPort connection first
    SerialPort.end();

	//Now set the pin to low for the duration of the break
	pinMode(TX_PIN, OUTPUT);
	digitalWrite(TX_PIN, LOW);
	delayMicroseconds(breakDuration);

	//Send the delimiter
	digitalWrite(TX_PIN, HIGH);
	delayMicroseconds(bitTime); //The Delimter is only one bit long

	//Begin the Serial communication
	SerialPort.begin(_baudRate);
}

unsigned char LinMaster::getClassicChecksum(unsigned char* data, unsigned char dataBytes)
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

unsigned char LinMaster::getEnhancedChecksum(unsigned char* data, unsigned char dataBytes, unsigned char pid)
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
