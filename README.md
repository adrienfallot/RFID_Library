RFID Library (14 bytes)
============

This revised version now decodes cards (or badges) that transmit 14 bytes, whereas the previous version handled only 5 bytes.

This library only support TTL RS232 serial port.

![125Khz RFID Reader](https://statics3.seeedstudio.com/images/product/gr125k.jpg)


[Grove - 125KHz RFID Reader
](https://www.seeedstudio.com/s/Grove-125KHz-RFID-Reader-p-1008.html)

Example:
- When reading the data from some RFID card, you will get data like `2  30  32  30  30  36  41  44  42  34  46  46  43  3` (2 and 3 indicate the start and stop of a transmission; we are going to ignore them).

- Each numbers are is a ASCII character in HEX: `30  32  30  30  36  41  44  42  34  46  46  43` ==> `0 2 0 0 6 A D B 4 F F C`

- And need to be reunited in pairs: `0 2 0 0 6 A D B 4 F F C` ==> `02 00 6A DB 4F FC`

- `FC` is the checksum using XOR: `02^00^6A^DB^4F == FC`

- `02 00 6A DB 4F` are your card number: last four bytes (`00 6A DB 4F`) are the engraved number on the card

- `00 6A DB 4F`: engraved number can be retrieved by converting hexadecimal to base-ten (`00 6A DB 4F`(hex) = `00000000 01101010 11011011 01001111`(bin) = `0007002959`(dec))

#### Pins 

1. VCC support 3.3 ~ 5V
2. TX, RX connect to Arduino or Seeeduino
3. T1, T2 is the Signal port for RFID antenna
4. W0, W1 is for wiegand protocol, but this library not support yet.

```
     		+-----------+
++++++++----|VCC	  T1|----
|  +++++----|GND	  T2|----
|  | |++----|TX		 SER|----
|  | |	----|RX		 LED|----	
|  | |	----|W0		BEEP|----
|  | |	----|W1		 GND|----
|  | |		+-----------+
|  | \___________________________________
|  |_____________________________        |
|                                |       |
|                              + + + + + + + +  + + + + + + + +
|                              | | | | | | | |  | | | | | | | |
|                              | | | | | | | |  | | | | | | | |
|            +-----------------|-|-|-|-|-|-|-|--|-|-|-|-|-|-|-|-+
|            |                 x-x-x-x-x-x-x-x  x-x-x-x-x-x-x-x |
|            |              xxx                                 |
|       +--------+          xxx    --- ---    .............     |
|       |        |                | - x + |  ' |  ||\ |.-. '    +
|       |        |          ^      --- ---   | |__|| \|._. |     x
|       |        |        <+++>    ARDUINO   '.............'      +
|       +--------+          V                                     |
|            |            _____                       ++-++   xx  |
|            |           <_____>                      +-O-+   xx  |
|            |                                        ++-++   xx  |
|            |+--++--                                             |
|            ||||++--                  +---------------------+    |
|            |+--++--                  |                     |    |
|            |                         |                     |    |
|         ++------+                    +---------------------+    |
|         ++      |                                               +
|         ++      |          +-+ +-+                             x
|         ++------+          +-+ +-+   x-x-x-x-x-x x-x-x-x-x-x  +
|            +-------------------------|-|-|-|-|-|-|-|-|-|-|-|--+
|                                      | | | | | | | | | | | |
|                                      | | | | | | | | | | | |
|                                      + + + + + + + + + + + +
|____________________________________________|
	
```

### Usage
Connecting RFID Reader RX, TX to Arduino pin headers.

`RX <--> 10`
`TX <--> 11`

```c
// RFID_UART.ino

#include <SoftwareSerial.h>
#include <SeeedRFID.h>

#define RFID_RX_PIN 10
#define RFID_TX_PIN 11

#define TEST

SeeedRFID RFID(RFID_RX_PIN, RFID_TX_PIN);
RFIDdata tag;

void setup() {
	Serial.begin(57600);
	Serial.println("Hello, double bk!");
}

void loop() { 
	if(RFID.isAvailable()){
		tag = RFID.data();
		Serial.print("RFID card number: ");
		Serial.println(RFID.cardNumber());
#ifdef TEST
	Serial.print("RFID raw data: ");
	for(int i=0; i<tag.dataLen; i++){
	    Serial.print(tag.raw[i], HEX);
	    Serial.print('\t');
		}
#endif
	}
}

```

### Using multiple readers
When using multiple readers, you'll need to call the 'listen' function in order to receive any future readings.
```c
// RFID_UART.ino

#include <SoftwareSerial.h>
#include <SeeedRFID.h>

#define RFID_RX_PIN 10
#define RFID_TX_PIN 11

#define RFID2_RX_PIN 12
#define RFID2_TX_PIN 13


SeeedRFID RFID(RFID_RX_PIN, RFID_TX_PIN);
RFIDdata tag;

SeeedRFID RFID2(RFID2_RX_PIN, RFID2_TX_PIN);
RFIDdata tag2;

void setup() {
  Serial.begin(9600);
  RFID.listen(); //first, we listen for data on reader #1
}

void loop() { 

  if(RFID.isAvailable()){
    tag = RFID.data();
    Serial.print("RFID card number: ");
    Serial.println(RFID.cardNumber());
    RFID2.listen(); //now start listening for data on reader #2
  }

  if(RFID2.isAvailable()){
    tag2 = RFID2.data();
    Serial.print("RFID2 card number: ");
    Serial.println(RFID2.cardNumber());
    RFID.listen(); //then we listen to reader #1 again...
  }
}

```

For more information please visit [wiki]([wiki](http://wiki.seeedstudio.com/Grove-125KHz_RFID_Reader/)).

----

The original library was written by [Ye Xiaobo][Github Homepage] for seeed studio<br>
and is licensed under [The MIT License](https://github.com/yexiaobo-seeedstudio/RFID_Library/blob/master/LICENSE). <br>
This modified library was written by [Adrien FALLOT][Github Homepage].

Contributing to this software is warmly welcomed. You can do this basically by<br>
[forking](https://help.github.com/articles/fork-a-repo), committing modifications and then [pulling requests](https://help.github.com/articles/using-pull-requests) (follow the links above<br>
for operating guide). Adding change log and your contact into file header is encouraged.<br>
Thanks for your contribution.

Seeed Studio is an open hardware facilitation company based in Shenzhen, China. <br>
Benefiting from local manufacture power and convenient global logistic system, <br>
we integrate resources to serve new era of innovation. Seeed also works with <br>
global distributors and partners to push open hardware movement.<br>


[RFID Image]: http://www.seeedstudio.com/wiki/images/6/6a/RFID.jpg
[Github Homepage]: https://github.com/yexiaobo-seeedstudio
