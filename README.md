# Mendeleev project

In 2011, the Belgium based University of Leuven (KUL) has made a [full size Table of Mendeleev](https://www.mtm.kuleuven.be/MTM_Tabel_van_Mendeleev) with every element on display (except for the radioactive ones for obvious reasons). Some of the elements are interactive, for example: a small motor driver is added to rotate a crystal while it is being lighted by UV LEDs.

For the second generation of the full size Table, the university wanted to make it even more interactive so that tutors can teach pupils about the Table of Mendeleev using a tablet. Using LED's and motors in every display cabinet, tutors can colorize and animate the cabinets on the full size table to better visualise all the data available in the tablet application.

Every cabinet is equipped with RGBW LED's, spot lighting and a proximity sensor. The sensor triggers an animation when someone waves in front of the cabinet. Some cabinets also include a motor, which starts rotating when the animation is triggered. Other cabinets include UV LED's to light up a UV sensitive material. Every display cabinet is unique in some way, which is why we went for a modular approach.

The different PCB's to drive all this are custom designed by [Wim Van Gool](http://phyx.be/) and the software is programmed by [Bert Outtier](https://github.com/bertouttier). The main board is built around the Arduino compatible [ATSAMD21G18](https://www.microchip.com/wwwproducts/en/ATsamd21g18) microcontroller. Every main board has a 8-bit DIP switch to set a unique address of the board, in line with the atomic number of all the chemical elements found in the Table of Mendeleev. The main boards of all display cabinets are connected on a RS485 bus system and can be reached using this address. Address `0` is the address of the master of the bus, in our case, a Beaglebone Black with another custom cape PCB made by Wim to have a RS485 connection. For the cabinets that include a motor, the main PCB is fitted with a custom made addon board with a motor driver.

The RS485 bus communication is a custom protocol based on Modbus-RTU. The Beaglebone Black is the master node on the bus and acts as a [MQTT to RS485 gateway](https://github.com/area3001/mqtt2mendeleev). The tablet application connects to the MQTT broker running on the Beaglebone Black, which will translate the MQTT messages to RS485 messages addressed to the correct display cabinet. The Beaglebone runs a [custom made Linux system](https://github.com/area3001/mendeleevos) using [Buildroot](https://github.com/buildroot/buildroot) and is connected to the same IP network as the tablet.

links:
 * https://hackaday.io/project/164265-mendeleev
 * https://area3001.com/

## Pinmux

| Arduino Pin | MCU PIN | Mendeleev | Comments        |
|------------|--------|----------------|-----------------|
| 0          |  PA11  |  M1CTRL0_PIN   | TCC1/WO[1]      |
| 1          |  PA10  |  M2CTRL0_PIN   | TCC1/WO[0]      |
| 2          |  PA14  |  PIN_SPI_CS0   |                 |
| 3          |  PA09  |  M2CTRL1_PIN   |                 |
| 4          |  PA08  |  M2CTRL2_PIN   | TCC0/WO[0]      |
| 5          |  PA15  |  LED_R_PIN     | TCC0/WO[5]      |
| 6          |  PA20  |  LED_UV_PIN    | TCC0/WO[6]      |
| 7          |  PA21  |  PIN_SPI_CS1   |                 |
| 8          |  PA06  |  M1CTRL2_PIN   |                 |
| 9          |  PA07  |  M1CTRL1_PIN   |                 |
| 10         |  PA18  |  LED_A_PIN     | TC3/WO[0]       |
| 11         |  PA16  |  LED_G_PIN     | TCC2/WO[0]      |
| 12         |  PA19  |  LED_W_PIN     | TC3/WO[1]       |
| 13         |  PA17  |  LED_B_PIN     | TCC2/WO[1]      |
| 14         |  PA02  |  INPUT2_PIN    | ADC/AIN[0]      |
| 15         |  PB08  |  INPUT0_PIN    | ADC/AIN[2]      |
| 16         |  PB09  |  INPUT1_PIN    | ADC/AIN[3]      |
| 17         |  PA04  |  INPUT3_PIN    | ADC/AIN[4]      |
| 18         |  PA05  |  PROX_PIN      | ADC/AIN[5]      |
| 19         |  PB02  |  RS485_DIR_PIN | ADC/AIN[10]     |
| 20         |  PA22  |  SDA           | SERCOM3/PAD[0]  |
| 21         |  PA23  |  SCL           | SERCOM3/PAD[1]  |
| 22         |  PA12  |  MISO          | SERCOM4/PAD[0]  |
| 23         |  PB10  |  MOSI          | SERCOM4/PAD[2]  |
| 24         |  PB11  |  SCK           | SERCOM4/PAD[3]  |
| 25         |  PB03  |                | USB RX LED      |
| 26         |  PA27  |                | USB TX LED      |
| 27         |  PA28  |                | USB host enable |
| 28         |  PA24  |                | USB/DM          |
| 29         |  PA25  |                | USB/DP          |
| 30         |  PB22  |  RS485 TX      | SERCOM5/PAD[2]  |
| 31         |  PB23  |  RS485 RX      | SERCOM5/PAD[3]  |
| 32         |  PA22  |                | Pin 20 (SDA)    |
| 33         |  PA23  |                | Pin 21 (SCL)    |
| 34         |  PA19  |                | SERCOM1/PAD[3]  |
| 35         |  PA16  |                | SERCOM1/PAD[0]  |
| 36         |  PA18  |                | SERCOM1/PAD[2]  |
| 37         |  PA17  |                | SERCOM1/PAD[1]  |
| 38         |  PA13  |  LED_TXT_PIN   | TCC2/WO[1]      |
| 39         |  PA21  |                | Pin 7           |
| 40         |  PA06  |                | Pin 8           |
| 41         |  PA07  |                | Pin 9           |
| 42         |  PA03  |                | [ADC|DAC]/VREFA |
| 43         |  PA02  |                | Alternate use of A0 (DAC output) |
| 44         |  PA30  |                | TCC1/WO[0]      |
| 45         |  PA31  |                | TCC1/WO[1]      |


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

The response frame has exectly the same structure. An ```acknowledge``` response is a frame with the same value in the command field as its request frame. If the command field value is one's complemented, this means a ```not acknowledge``` response.

Nodes should not respond to broadcast messages (destination field has the value of ```0xFF```).

### Commands

#### SET_COLOR (0x00)
Set the value of the LEDs of the board.

The payload is a 3 to 7 byte value, so each LED intensity can be set in 8bit levels:

| Byte 0 | Byte 1 | Byte 2 | Byte 3* | Byte 4* | Byte 5* | Byte 6* |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| Red | Green | Blue | White | Warm white | UV | Text leds |

Bytes with * are optional.

The response does not have payload data.

#### SET_MODE (0x01)
Set the mode of the node.

The payload data is 1 byte indicating the mode to switch to. There are 2 modes: guest (```0x01```) and teacher (```0x02```) mode.

#### OTA (0x02)
Send firmware upgrade data to the node.

The firmware binary file is split by the [mqtt2mendeleev bridge](https://github.com/area3001/mqtt2mendeleev) in parts of 236 bytes and sent to the destination node using this command. The first 2 bytes of the payload contain the index of the part. The next 2 bytes containing the remaining amount of bytes to be sent. This way, we know when we completely received the firmware update data and if we receive parts out of order.

The node should acknowledge the commands before rebooting. The responses do not have payload data.

#### GET_VERSION (0x03)
Get the firmware version of the node.

There is no payload to call this command. The response contains the bytes of a string that describes the version number.

#### SET_OUTPUT (0x04)
Control the 4 outputs of the node.

The payload should be 2 bytes. The first byte indicates a mask of which outputs need to be toggled. The second byte contains the state to which the indicated outputs should be toggled.
The response does not have payload data.

#### REBOOT (0x05)
Reboot the node.

There is no payload to call this command. The device should acknowledge the command before rebooting.
