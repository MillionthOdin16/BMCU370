# BMCU Serial Bridge

The BMCU Serial Bridge allows a computer to monitor and interact with the BambuBus network through the debug UART (USART3).

## Hardware Connection

- **Port:** USART3 (same as debug port)
- **Pins:** PB10 (TX), PB11 (RX)
- **Settings:** 115,200 baud, 9-bit, even parity, 1 stop bit
- **Connection:** Connect via USB-to-serial adapter to your computer

## Frame Protocol

All data sent over the serial port uses a simple framing protocol to distinguish between debug messages and BambuBus packets:

```
[SYNC] [TYPE] [LEN_H] [LEN_L] [DATA...]

SYNC  = 0xAA (frame start marker)
TYPE  = Frame type:
        0x01 = Debug message (text)
        0x02 = BambuBus packet (binary)
LEN_H = Length high byte (big-endian)
LEN_L = Length low byte (big-endian)
DATA  = Payload (variable length)
```

### Example Frames

**Debug message "Hello":**
```
0xAA 0x01 0x00 0x05 'H' 'e' 'l' 'l' 'o'
```

**BambuBus heartbeat packet:**
```
0xAA 0x02 0x00 0x06 0x3D 0xC5 0x20 0x01 [CRC16_H] [CRC16_L]
```

## Usage

### Monitor Mode (Computer Receives BambuBus Traffic)

All BambuBus packets received by the BMCU are automatically forwarded to the computer as TYPE 0x02 frames. Simply connect to the serial port and parse the frames.

**Python Example:**
```python
import serial

def parse_frame(ser):
    # Wait for sync byte
    while True:
        b = ser.read(1)
        if b == b'\xAA':
            break

    # Read frame type
    frame_type = ser.read(1)[0]

    # Read length (big-endian)
    len_h = ser.read(1)[0]
    len_l = ser.read(1)[0]
    length = (len_h << 8) | len_l

    # Read data
    data = ser.read(length)

    return frame_type, data

# Open serial port
ser = serial.Serial('/dev/ttyUSB0', 115200, parity=serial.PARITY_EVEN,
                    bytesize=serial.NINEBITS)

while True:
    frame_type, data = parse_frame(ser)

    if frame_type == 0x01:
        print(f"Debug: {data.decode('utf-8', errors='ignore')}")
    elif frame_type == 0x02:
        print(f"BambuBus: {data.hex()}")
```

### Bidirectional Mode (Computer Sends BambuBus Commands)

To send BambuBus commands from the computer:

1. Construct a valid BambuBus packet (must start with 0x3D)
2. Wrap it in a TYPE 0x02 frame
3. Send to the serial port

**Python Example:**
```python
def send_bambubus_packet(ser, packet):
    # Construct frame
    frame_type = 0x02
    length = len(packet)

    frame = bytes([
        0xAA,                    # Sync
        frame_type,              # Type
        (length >> 8) & 0xFF,    # Length high
        length & 0xFF,           # Length low
    ]) + packet

    ser.write(frame)

# Example: Send heartbeat to BambuBus
heartbeat_packet = bytes([0x3D, 0xC5, 0x20, 0x01, 0x12, 0x34])
send_bambubus_packet(ser, heartbeat_packet)
```

## BambuBus Packet Format

BambuBus packets forwarded via the serial bridge maintain their original format:

```
[0x3D] [HEADER] [LENGTH] [DATA] [CRC8] [PAYLOAD] [CRC16]
```

For details on BambuBus protocol, see `docs/developer-guide/bambubus-protocol.md`.

## Common Packet Types

The following BambuBus packet types can be observed/sent:

| Packet Type | Byte[1] | Byte[4] | Direction | Description |
|-------------|---------|---------|-----------|-------------|
| heartbeat | 0xC5 | 0x20 | Printer→BMCU | Keep-alive (1 Hz) |
| filament_motion_long | 0x05 | 0x04 | Printer→BMCU | Filament movement command |
| online_detect | 0xC5 | 0x05 | Printer→BMCU | Device detection |
| version | 0x05 | 0x103 | Printer→BMCU | Firmware version query |
| Dxx_res | 0xCx | varies | BMCU→Printer | Response packet |

## Implementation Files

- `src/SerialBridge.h` - Serial bridge interface
- `src/SerialBridge.cpp` - Frame encoding/decoding and forwarding logic
- `src/Debug_log.cpp` - Modified to send framed debug messages
- `src/BambuBus.cpp` - Modified to forward received packets (line 270)
- `src/main.cpp` - Calls `SerialBridge_process()` in main loop (line 238)

## Notes

- The serial bridge operates concurrently with normal BMCU operation
- Debug messages are automatically framed (TYPE 0x01) and remain human-readable after unwrapping
- BambuBus packets are forwarded in real-time with minimal latency
- Invalid frames received from computer are silently discarded
- All BambuBus packets must pass CRC validation (handled by existing BambuBus code)

## Troubleshooting

**No data received:**
- Check baud rate (115,200)
- Verify parity is set to EVEN
- Ensure 9-bit mode is supported by your serial adapter

**Corrupted frames:**
- Check for buffer overruns on computer side
- Verify frame sync (0xAA) is correctly detected
- Ensure length field is read as big-endian

**Commands not working:**
- Verify BambuBus packet starts with 0x3D
- Check CRC16 is correctly calculated
- Ensure packet matches BambuBus protocol specification
