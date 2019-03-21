# Mendeleev project
Arduino based project for the Mendeleev board.

links:
 * https://area3001.com/

## RS485 Protocol
| Preamble | Destination | Source | Sequence number | Command | Data length | Data | Checksum |
|---|---|---|---|---|---|---|---|
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
Set the value of the LEDs of the board. The payload is a 7 byte value:

| Byte 0 | Byte 1 | Byte 2 | Byte 3 | Byte 4 | Byte 5 | Byte 6 |
|---|---|---|---|---|---|---|
| Red | Green | Blue | White | Warm white | UV | Text leds |

So the LED light intensity can be set in 8bit levels. It is required to give a value for all LEDs in the payload.
The response does not have payload data.

#### SET_MODE (0x01)
Set the mode of the node. There are 2 modes: automatic and teaching mode.

#### OTA (0x02)
Send firmware upgrade data to the node. The firmware to upgrade to is split by the mqtt2mendeleev bridge in parts of XXX bytes and sent to the destination node using this command. The first 2 bytes of the payload contain the number of bytes remaining to be sent. This way, we know when we completely received the firmware update data.
The response does not have payload data.

#### GET_VERSION (0x03)
There is no payload to call this command. The response contains the bytes of a string that describes the version number.

#### SET_OUTPUT (0x04)
The payload should be 2 bytes. The first byte indicates a mask of which outputs need to be considered. The second byte contains the value to which the indicated outputs should be toggled.
The response does not have payload data.
