/*
    SeeedRFID.cpp
    A library for RFID module. This revised version now decodes cards (or badges) that transmit 14 bytes, whereas the previous version handled only 5 bytes.

    original library by :
     - Copyright (c) 2008-2014 seeed technology inc.
     - Author      : Ye Xiaobo(yexiaobo@seeedstudio.com)
     - Create Time: 2014/2/20
    
    modified library by : 
     - Author      : Adrien FALLOT (dev@adrienfallot.fr)
     - Modified Time: 2026/06/19

    The MIT License (MIT)

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.
*/

/**************************************************************************
    Pins
    ====

    1. VCC support 3.3 ~ 5V
    2. TX, RX connect to Arduino or Seeeduino
    3. T1, T2 is the Signal port for RFID antenna
    4. W0, W1 is for wiegand protocol, but this library not support yet.

    ```
 		+-----------+
 	----|VCC	  T1|----
 	----|GND	  T2|----
 	----|TX		 SER|----
 	----|RX		 LED|----
 	----|W0		BEEP|----
 	----|W1		 GND|----
 		+-----------+
    ```
***************************************************************************/

#include <SoftwareSerial.h>
#include "SeeedRFID.h"
#include "Arduino.h"

SeeedRFID::SeeedRFID(int rxPin, int txPin) {
    _rfidIO = new SoftwareSerial(rxPin, txPin);
    _rfidIO->begin(9600);

    // init RFID data
    _data.dataLen = 0;
    _data.chk = 0;
    _data.valid = false;

    _type = RFID_UART;
}

SeeedRFID::~SeeedRFID() {
}

/* 
    Convert raw value to corresponding HEX value of char in ASCII
	Examples: 
		- raw 48 is char '1' in ASCII which is 0x01 in HEX
		- raw 65 is char 'A' in ASCII which is 0x10 in HEX
*/
int SeeedRFID::rawToHex(char ASCII)
{
	if(ASCII > 47 && ASCII < 58) { // ASCII number 0-9
		return ASCII-48;
	}
	else if(ASCII > 64 && ASCII < 71) { // ASCII Letters A-F
		return ASCII-55;
	}
}

/*
    Check if data is valid using checksum (and right length)
    XOR checksum == XOR data 
*/
boolean SeeedRFID::checkBitValidationUART() {
    if (EXPECTED_DATA_LENGTH != _data.dataLen) {
        return false;
    }

    int dataXOR = 0;
    for (int i = 0 ; i <= 9 ; i ++) { // Ignore checksum bytes
        dataXOR ^= rawToHex(_data.raw[i]);
    }

    // XOR checksum == XOR data
    _data.valid = _data.chk == dataXOR;
    return _data.valid;
}

boolean SeeedRFID::read() {

    _data.chk = 0;

    if (_data.dataLen != 0) {
        _data.dataLen = 0;
    }

    while (_rfidIO->available()) {
        char byteReceived = _rfidIO->read();
        if (byteReceived == 2 || byteReceived == 3) { //Ignore start, stop bytes
            continue;
        }
        else if (_data.dataLen >= 10) { // checksum bytes
            _data.chk ^= rawToHex(byteReceived);
        }
        _data.raw[_data.dataLen++] = byteReceived;

        #ifdef DEBUG
        Serial.println("SeeedRFID:read() function, and the RFID raw data: ");
        for (int i = 0; i < _data.dataLen; ++i) {
            Serial.println();
            Serial.print(_data.raw[i], HEX);
            Serial.print('\t');
            Serial.print(rawToHex(_data.raw[i]));
        }
        Serial.println();
        #endif
        delay(10);
    }

    return SeeedRFID::checkBitValidationUART();
}

boolean SeeedRFID::isAvailable() {
    switch (_type) {
        case RFID_UART:
            return SeeedRFID::read();
            break;
        case RFID_WIEGAND: //no support of the wiegand protocol yet.
            return false;
            break;
        default:
            return false;
            break;
    }
}

RFIDdata SeeedRFID::data() {
    if (_data.valid) {
        return _data;
    } else {
        // empty data
        RFIDdata _tag;
        return _tag;
    }
}

/*
    Retrieve engraved card number by converting the last four hexadecimal bytes of data to base-ten.
*/
unsigned long SeeedRFID::cardNumber() {

    unsigned long sum = 0;
    for (int i = 2; i < _data.dataLen - 2; ++i) { // Ignore constructor identifier and checksum bytes
		sum += rawToHex(_data.raw[i]);
        if (i != _data.dataLen - 3) { // Ignore checksum bytes and last data byte
			sum = sum << 4;
		}
    }

    #ifdef DEBUG
    Serial.print("cardNumber(HEX): ");
    Serial.println(sum, HEX);
    Serial.print("cardNumber: ");
    Serial.println(sum);
    #endif

    return sum;
}
