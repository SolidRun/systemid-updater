# systemid-updater
read and write SystemID eeprom via kernel sysfs for the LX2160a-CEX7 based boards

## Requirements
This utility can be used standalone to create a firmware file or directly on the LX2160A based system. To use this utility directly on the device in Linux you will need to be running firmware at `git hash #47f178541` or later.

## Building
To build the software it is quite simple

* clone this repository
* `mkdir build`
* `cd build`
* `cmake ..`
* `make`

## Usage

First you need to add the system eeprom into Linux's sysfs.  This is done with the following command as root

`echo 24c02 0x57 > /sys/bus/i2c/devices/i2c-0/new_device`

You can then initialize a new NXID eeprom structure on the device using the `-i`

`./systemID_updater -iw`

You can then use the check command to see the entire and verbose to actually see the structure on the eeprom

```
./systemID_updater -cv
00: 4E 58 49 44 00 00 00 00 00 00 00 00 00 00 00 00 | NXID
10: 00 00 00 00 00 21 04 14 11 51 48 00 00 00 00 01 | 
20: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | 
30: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | 
40: 11 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | 
50: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | 
60: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | 
70: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | 
80: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | 
90: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | 
A0: 00 00 D0 63 B4 02 54 5A 00 00 00 00 00 00 00 00 | 
B0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | 
C0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | 
D0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | 
E0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | 
F0: 00 00 00 00 00 00 00 00 00 00 00 00 FB 8F 32 29 | 
100:                                                 | 
check EEPROM:
TagID: NXID
serialnumber: 
build date: 14.04.2021 11:51:48
version: 1
mac flags: 00
mac17: D0:63:B4:02:54:5A
CRC: FB8F3229
```

Adding or removing MAC addresses can be done by updating the device with `-m` and `-a`, where `-m` specifies the **MAC interface number** and `-a` is the **MAC address to be assigned** to that interface.

On all devices the 1000-BaseT RJ45 interface is MAC `17` and you can see that is the only MAC set in the firmware above.  That can be added or changed with the following command.

`./systemID_updater -u -m 17 -a D2:63:B4:02:54:5A`

an address can be removed by setting the index address to all `00`'s

## Reference

The sys eeprom driver in edk2 does not need all **18 possible MACs** filled allocated if you know they are not going to be used.  I have therefore modified this project to allow only certain mac's to have addresses assigned.  If you are changing your serdes networking configuration to be different than the default on then you should reference **section 26.1.4 SerDes** options in the **Qoriq LX2160a Reference Manual** to determine the **MAC id** that will need an address assigned to it in your configuration.

The default MACs for a HoneyComb system are

17 - RJ45

7,8,9,10 - SFP+ modules
