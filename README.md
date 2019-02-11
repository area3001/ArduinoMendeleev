# Mendeleev project
Arduino based project for the Mendeleev board.

## RS485 Protocol
| Preamble | Destination | Source | Packet number | Command | Data length | Data | Checksum |
|---|---|---|---|---|---|---|---|
| 8 Bytes | 1 Byte | 1 Byte | 2 Bytes | 1 Byte | 2 Bytes | n Bytes | 2 Byte |
