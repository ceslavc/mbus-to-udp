# MBUS to UDP converter

## System decription
The converter consists of a interface card mounted on a Raspberry Pi Zero. Input is a stream of meter data on a MBUS interface. Output is a stream of JSON objects sent to a UDP port on a extern server running Node Red. Network connection is via RPi's internal wifi card. 
The converter card is supplied from the MBUS line, Raspbery Pi is sypplied from a separate 5V power supply.
RPi and converter card is connected via three pins in the GPIO header - 3,3V, UART RX and GND.

## The converter card
A MBUS interface IC NCN5150 needs a small number of external components to function. Connection to the smart meter is via standard ethernet cable (MBUS uses pins 1 and 2) through a RJ45 socket. The converter card has the same header pins as the Raspberry Pi. It is possible to connect all the 40 pins, but it is not required. Only three pins must be connected.

## Data stream from smart meter
This description is for Kamstrup smart meter, all the other meters should follow the same standard (see the NVE note).

There are two data blocks. One is sent every 10 seconds, one every hour.

Following data are pushed from the meter:

Nr|OBIS code|Object|Descrtiption|Unit|Scaler
--|---------|------|------------|----|------
1|1.1.0.2.129.255|OBIS List version identifier
2|1.1.0.0.5.255|GS1 number|Meter -ID (GIAI GS1 -16 digit )
3|1.1.96.1.1.255|Meter type|Visible string
4|1.1.1.7.0.255|P14|Active power from the grid|W|0
5|1.1.2.7.0.255|P23|Active power to the grid|W|0
6|1.1.3.7.0.255|Q12|Reactive power from the grid|var|0
7|1.1.4.7.0.255|Q34|Reactive power to the grid|var|0
8|1.1.31.7.0.255|IL1|Current phase 1|A|-2
9|1.1.51.7.0.255|IL2|Current phase 2|A|-2
10|1.1.71.7.0.255|IL3|Current phase 3|A|-2
11|1.1.32.7.0.255|UL1|Phase or line voltage 1|V|0
12|1.1.52.7.0.255|UL2|Phase or line voltage 2|V|0
13|1.1.72.7.0.255|UL3|Phase or line voltage 3|V|0
14|0.1.1.0.0.255|RTC|Clock and date
15|1.1.1.8.0.255|A14|Cumulative hourly active import energy|Wh|1
16|1.1.2.8.0.255|A23|Cumulative hourly active export energy|Wh|1
17|1.1.3.8.0.255|R12|Cumulative hourly reactive import energy|varh|1
18|1.1.4.8.0.255|R34|Cumulative hourly reactive export energy|varh|1

Data 1-13 are pushed every 10 seconds, 1-18 is pushed every hour

Data that are not present (if the meter is only one phase) are omitted. Voltage depends on type of grid connection (3 or 4 wire).

Example (from Kamstrup documentation)
```
7E A0E2 2B 21 13 239A E6E700
0F 00000000 0C07D0010106162100FF800001
0219
0A0E 4B616D73747275705F5630303031
0906 0101000005FF 0A10 35373036353637303030303030303030
0906 0101600101FF 0A12 303030303030303030303030303030303030
0906 0101010700FF 0600000000   
0906 0101020700FF 0600000000
0906 0101030700FF 0600000000
0906 0101040700FF 0600000000
0906 01011F0700FF 0600000000
0906 0101330700FF 0600000000
0906 0101470700FF 0600000000
0906 0101200700FF 120000
0906 0101340700FF 120000
0906 0101480700FF 120000
0906 0001010000FF 090C 07E1081003100005FF800000
0906 0101010800FF 0600000000
0906 0101020800FF 0600000000
0906 0101030800FF 0600000000
0906 0101040800FF 0600000000
C8867E
```
The interesting data start on line 7. Every data line starts with 0906, then the OBIS code and a data block. Different blocks for different data. 

