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
 * @file LinSlave.h
 *
 * @author Michael Hollmer
 *
 * @date 10.05.2020
 *
########################################################### */

#include "Arduino.h"
#include <HardwareSerial.h>

#define SLAVE_ID 0x10               //The identifier of this slave node
#define PID_TO_ID_MASK 0x3F         //Use this mask with a logic AND on the protected identifier to get the ID 

/*! ####################################
 * @class LinSlave
 *
 * @author Michael Hollmer
 *
 * @date 10.05.2020
 *
 * @brief Implementation of a LIN Slave node for the Arduino
 *
#################################### */
class LinSlave
{
	public:

        /*!
        * @brief Empty constructor - all the startup code is in the begin() method
        */
		LinSlave();

        /*!
        * @brief Sets up everything for the communication
        * @param[in]    baudRate    The baud rate which should be used for the communication
        */
		void begin(unsigned int baudRate);

        /*!
        * @brief Returns the protected identifier from an incoming header
        * @param[out]   corrupted   Corrupted flag is set to true, if the pid does not match with the id
        */
        unsigned char listen(unsigned char* corrupted);

        /*!
        * @brief Sends the data 
        * @param[in]    data    The data which should be send
        */
        void send(unsigned char data);

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

	protected:

        /*!
        * @brief Calculates the protected identfier
        * @param[in]    idNumber    The identfier for which the PID should be calculated
        */
        unsigned char getPID(unsigned char idNumber);
        
        /*!
        * @brief The baud rate of the communication
        */
		unsigned int _baudRate;
};
