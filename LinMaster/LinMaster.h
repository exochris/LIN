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
 * @file LinMaster.h
 *
 * @author Michael Hollmer
 *
 * @date 10.05.2020
 *
########################################################### */

#include <HardwareSerial.h>
#include "Arduino.h"

//Settings for the Lin communication
#define TX_PIN 17				//For the Arduino nano it's the first pin
#define HEADER_BREAK_BITS 13	//How many break bits we want to send
#define TIMEOUT_MILLISEC 100    //The maximum time a slave has to answer a header

#define USE_ENHANCED_CHECKSUM   //Define this if the master should use the enhanced checksum

/*! ####################################
 * @class LinMaster
 *
 * @author Michael Hollmer
 *
 * @date 10.05.2020
 *
 * @brief Implementation of a LIN Master node for the Arduino 
 *
#################################### */

class LinMaster
{
	public:

        /*!
        * @brief Empty constructor - all the startup code is in the begin() method
        */
		LinMaster();

        /*!
        * @brief Sets up everything we need to send frames and communicate with the slave nodes
        * @param[in]    baudRate    The baud rate which should be used for the communication
        */
		void begin(unsigned int baudRate);

        /*!
        * @brief Sends a standard LIN frame header
        * @param[in]    idNumber    The identifier of the slave we want to talk to
        */
		void sendHeader(unsigned char idNumber);

        /*!
        * @brief Receives the answer of a slave
        * @param[out]   pid             The protected identifier of the slave, who should send the response. If the classic checksum is used the pid is ignored
        * @param[out]   receivedBytes   The amount of databytes the slave send   
        * @param[out]   corrupted       If the checksum does not match with the received data, the corrupted flag is set to 1
        */
		unsigned char* receiveResponse(unsigned char pid, unsigned char* receivedBytes, unsigned char *corrupted);

        /*!
        * @brief Sends the data
        * @param[in]    data    The data which should be send
        */
        void send(unsigned char data);

        /*!
        * @brief Calculates the protected identfier
        * @param[in]    idNumber    The identfier for which the PID should be calculated
        */
        unsigned char getPID(unsigned char idNumber);

  

	protected:

    

        /*!
        * @brief Sends a break signal 
        */
		void sendBreak();

           /*!
        * @brief Calculates the classic checksum from the given data
        * @param[in]    data        The data for which the checksum should be calculated
        * @param[in]    dataBytes   The amount of bytes stored in the data pointer
        */
        unsigned char getClassicChecksum(unsigned char* data, unsigned char dataBytes);

        /*!
        * @brief Calculates the enhanced checksum from the given data
        * @param[in]    data        The data for which the checksum should be calculated
        * @param[in]    dataBytes   The amount of bytes stored in the data pointer
        * @param[in]    pid         The protected identfier 
        */
        unsigned char getEnhancedChecksum(unsigned char* data, unsigned char dataBytes, unsigned char pid);

        /*!
        * @brief The baud rate of the communication
        */
		unsigned int _baudRate;

    

};
