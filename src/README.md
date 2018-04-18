# Installation

## Preparations

Edit the ```/boot/config.txt``` file. Add following at the end of the file:
```
enable_uart=1
dtoverlay=pi3-miniuart-bt
```

Edit the ```/boot/cmdline.txt``` file. Remove any mentions of serial port ```ttyAMA0```. It looks someting like this:
```
xxxxxxx xxxxxxxx console=ttyAMA0,115200 yyyyyyy yyyyyyy
```

## Security

A good practice is to replace default user pi with a new user. Make sure that the new user has administration rights before removing the new one.

## Network

The system does not require a static ip address. It may be a usefull for remote access and administration.

## Installation

Use regular ```make``` commands to build and install the utility.

Before installing you must edit the ```mbustoudp.service``` file. Replace ```ip_address``` and ```port``` with address and port of your Node Red server.

```make install``` will copy files to their locations and starts andd enables the service. Must be run as root.

Service will be stopped, disabled and all the files will be removed with ```make clean```

## What is working and what's not

The program is sending consumed power and voltage and current for all three phases.

It does not send reactive power nor the data that are coming every hour. It has not been tested with one phase meter. This will be changed in later version.

The choice of sending separate JSON for every value comes from early testing which did not catch all the data. I will test sending all the data in one JSON object.


