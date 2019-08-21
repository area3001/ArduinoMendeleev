# Mendeleev project

In 2011, the Belgium based University of Leuven (KUL) has made a [full size Table of Mendeleev](https://www.mtm.kuleuven.be/MTM_Tabel_van_Mendeleev) with every element on display (except for the radioactive ones for obvious reasons). Some of the elements are interactive, for example: a small motor driver is added to rotate a crystal while it is being lighted by UV LEDs.

For the second generation of the full size Table, the university wanted to make it even more interactive so that tutors can teach pupils about the Table of Mendeleev using a tablet. Using LED's and motors in every display cabinet, tutors can colorize and animate the cabinets on the full size table to better visualise all the data available in the tablet application.

Every cabinet is equipped with RGBW LED's, spot lighting and a proximity sensor. The sensor triggers an animation when someone waves in front of the cabinet. Some cabinets also include a motor, which starts rotating when the animation is triggered. Other cabinets include UV LED's to light up a UV sensitive material. Every display cabinet is unique in some way, which is why we went for a modular approach.

The different PCB's to drive all this are custom designed by [Wim Van Gool](http://phyx.be/) and the software is programmed by [Bert Outtier](https://github.com/bertouttier). The main board is built around the Arduino compatible [ATSAMD21G18](https://www.microchip.com/wwwproducts/en/ATsamd21g18) microcontroller. Every main board has a 8-bit DIP switch to set a unique address of the board, in line with the atomic number of all the chemical elements found in the Table of Mendeleev. The main boards of all display cabinets are connected on a RS485 bus system and can be reached using this address. Address `0` is the address of the master of the bus, in our case, a Beaglebone Black with another custom cape PCB made by Wim to have a RS485 connection. For the cabinets that include a motor, the main PCB is fitted with a custom made addon board with a motor driver.

The RS485 bus communication is a custom protocol based on Modbus-RTU. The Beaglebone Black is the master node on the bus and acts as a [MQTT to RS485 gateway](https://github.com/area3001/mqtt2mendeleev). The tablet application connects to the MQTT broker running on the Beaglebone Black, which will translate the MQTT messages to RS485 messages addressed to the correct display cabinet. The Beaglebone runs a [custom made Linux system](https://github.com/area3001/mendeleevos) using [Buildroot](https://github.com/buildroot/buildroot) and is connected to the same IP network as the tablet.

links:
 * https://hackaday.io/project/164265-mendeleev
 * https://area3001.com/

## RS485 Protocol
| Preamble | Destination | Source | Sequence number | Command | Data length | Data | Checksum |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| 8 Bytes | 1 Byte | 1 Byte | 2 Bytes | 1 Byte | 2 Bytes | n Bytes | 2 Bytes |

 * Preamble: 8 bytes with a value of ```0xA5``` to indicate the start of a Mendeleev protocol frame
 * Destination: A 1 byte number indicating the address of the destination node of the frame
 * Source: A 1 byte number indicating the address of sender of the frame
 * Sequence number: 2 bytes containing a number to track the request and corresponding response frames
 * Command: 1 byte indicating the command to execute on the node
 * Data length: 2 bytes indicating the length (in bytes) of the payload data that follows
 * Data: variable length payload data
 * Checksum: a 2 bytes CRC16 checksum of the frame, excluding the preamble and the checksum

The response frame has exectly the same structure. If the command field value is kept as it is, this means a ```acknowledge``` response. If the command field value is one's complemented, this means a ```not acknowledge``` response.

Nodes should not respond to broadcast messages (destination field has the value of ```0xFF```).

### Commands

#### SET_COLOR (0x00)
Set the value of the LEDs of the board. The payload is a 3 to 7 byte value:

| Byte 0 | Byte 1 | Byte 2 | Byte 3* | Byte 4* | Byte 5* | Byte 6* |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| Red | Green | Blue | White | Warm white | UV | Text leds |

Bytes with * are optional

So the LED light intensity can be set in 8bit levels. It is required to give a value for all LEDs in the payload.
The response does not have payload data.

#### SET_MODE (0x01)
Set the mode of the node. There are 2 modes: automatic and teaching mode.

#### OTA (0x02)
Send firmware upgrade data to the node. The firmware binary file is split by the mqtt2mendeleev bridge in parts of 236 bytes and sent to the destination node using this command. The first 2 bytes of the payload contain the index of the part. The next 2 bytes containing the remaining amount of bytes to be sent. This way, we know when we completely received the firmware update data and if we receive parts out of order.
The response does not have payload data.

#### GET_VERSION (0x03)
There is no payload to call this command. The response contains the bytes of a string that describes the version number.

#### SET_OUTPUT (0x04)
The payload should be 2 bytes. The first byte indicates a mask of which outputs need to be considered. The second byte contains the value to which the indicated outputs should be toggled.
The response does not have payload data.
