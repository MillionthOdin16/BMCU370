# BambuBus Protocol Documentation

## Overview

BambuBus is a proprietary communication protocol used by Bambu Lab 3D printers to communicate with AMS (Automatic Material System) and AMS Lite devices. BMCU-C implements this protocol to emulate an AMS Lite device.

## Physical Layer

### Hardware Interface

**Type:** RS-485 differential signaling
- **Differential Pair:** A and B lines
- **Common:** GND (ground reference)
- **Power:** 24V DC
- **Connector:** 6-pin (A, B, GND, 24V, potentially extras)

**Electrical Characteristics:**
- **Voltage Levels:** ±200mV to ±6V differential
- **Bus Termination:** Built into printer/AMS
- **Direction Control:** PA12 (DE pin) controls transmit/receive
  - HIGH = Transmit mode
  - LOW = Receive mode (default)

### UART Configuration

**USART1 Settings:**
```cpp
Baud Rate: 1,250,000 bps (1.25 Mbaud)
Word Length: 9 bits
Parity: Even
Stop Bits: 1
Hardware Flow Control: None
```

**Pins (CH32V203C8T6):**
- TX: PA9
- RX: PA10
- DE (Direction Enable): PA12

**DMA Configuration:**
- TX uses DMA1 Channel 4 for efficient transmission
- Priority: Very High
- Mode: Normal (not circular)
- Interrupts: RXNE (receive), TC (transmit complete)

## Protocol Structure

### Packet Format

All BambuBus packets follow this structure:

```
┌──────┬───────────┬────────┬────────┬──────┬─────────┬────────┐
│ START│  HEADER   │ LENGTH │  DATA  │ CRC8 │  DATA   │ CRC16  │
│ 0x3D │  1 byte   │ 1 byte │ varies │1 byte│ payload │2 bytes │
└──────┴───────────┴────────┴────────┴──────┴─────────┴────────┘
```

### Start Byte

**Value:** `0x3D`
- Indicates beginning of packet
- All packets must start with this byte
- Parser waits for 0x3D before processing

### Header Byte

**Bit 7:** Packet type indicator
- `1` (0x80-0xFF): Short header packet
- `0` (0x00-0x7F): Long header packet

**Bits 6-0:** Packet type/command identifier

**Short vs Long Header:**
```
Short Header (bit 7 = 1):
  - Length byte at index 2
  - CRC8 at index 3
  - More compact for simple commands

Long Header (bit 7 = 0):
  - Length byte at index 4
  - CRC8 at index 6
  - Allows for extended addressing/options
```

### Length Byte

**Value:** Total packet length in bytes
- Includes all bytes from START to end of CRC16
- Maximum practical length: ~255 bytes
- Buffer size: 1000 bytes (safety margin)

### CRC8 Checksum

**Purpose:** Header validation

**Parameters:**
```cpp
Polynomial: 0x39
Initial Value: 0x66
Final XOR: 0x00
Reflect Input: false
Reflect Output: false
```

**Calculation:**
- Includes: START byte (0x3D) through byte before CRC8
- Excludes: CRC8 itself and everything after
- Implementation: `robtillaart/CRC` library

**Validation:**
```cpp
CRC8 crc(0x39, 0x66, 0x00, false, false);
crc.restart();
crc.add(0x3D);
for (int i = 1; i < crc8_index; i++) {
    crc.add(packet[i]);
}
if (packet[crc8_index] != crc.calc()) {
    // CRC error - discard packet
}
```

### Data Payload

**Variable Length:** Depends on packet type
- Filament information
- Motion commands
- Status queries
- Responses

**Encoding:** Binary (not text)
- Multi-byte values typically little-endian
- Strings null-terminated or length-prefixed

### CRC16 Checksum

**Purpose:** Full packet validation

**Parameters:**
```cpp
Polynomial: 0x1021 (CCITT)
Initial Value: 0x913D
Final XOR: 0x00
Reflect Input: false
Reflect Output: false
```

**Calculation:**
- Includes: Entire packet except CRC16 itself
- Placed at end of packet (2 bytes)
- Transmitted low byte first (little-endian)

## Packet Types

### Enumeration

```cpp
enum class BambuBus_package_type {
    ERROR = -1,                // Communication error
    NONE = 0,                  // No packet / waiting
    filament_motion_short,     // Short motion command
    filament_motion_long,      // Long motion command
    online_detect,             // Device presence detection
    REQx6,                     // Request type 6
    NFC_detect,                // NFC tag read request
    set_filament_info,         // Set filament properties
    MC_online,                 // Channel online status
    read_filament_info,        // Request filament data
    set_filament_info_type2,   // Alternative filament set
    version,                   // Firmware version query
    serial_number,             // Device serial number
    heartbeat,                 // Keepalive packet
    ETC                        // Miscellaneous packets
};
```cpp

### Device Types

```cpp
enum BambuBus_device_type {
    BambuBus_none = 0x0000,        // No device
    BambuBus_AMS = 0x0700,         // Full AMS (4 channels)
    BambuBus_AMS_lite = 0x1200,    // AMS Lite (4 channels)
};
```

**BMCU-C Identification:**
- Reports as `BambuBus_AMS_lite` (0x1200)
- Compatible with A-series printers
- May work as BambuBus_AMS on some printers

## Packet Processing

### Reception Flow

```cpp
void USART1_IRQHandler() {
    if (USART_GetITStatus(USART1, USART_IT_RXNE)) {
        uint8_t byte = USART_ReceiveData(USART1);
        RX_IRQ(byte);  // Process byte
    }
}
```

**RX_IRQ State Machine:**

```
State: WAITING_START
  ↓ (receive 0x3D)
State: RECEIVING_HEADER
  ↓ (determine packet type from bit 7)
State: RECEIVING_DATA
  ↓ (collect bytes until length reached)
State: VALIDATING_CRC8
  ↓ (verify header checksum)
  ↓ (if valid, continue; if invalid, reset)
State: COLLECTING_PAYLOAD
  ↓ (receive remaining data)
State: VALIDATING_CRC16
  ↓ (verify full packet - not shown in code excerpt)
State: PACKET_COMPLETE
  ↓ (copy to buffer, set have_data flag)
State: WAITING_START (loop)
```cpp

### Transmission

```cpp
void send_uart(const unsigned char *data, uint16_t length) {
    // 1. Configure DMA with packet data
    DMA_DeInit(DMA1_Channel4);
    Bambubus_DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)data;
    Bambubus_DMA_InitStructure.DMA_BufferSize = length;
    DMA_Init(DMA1_Channel4, &Bambubus_DMA_InitStructure);

    // 2. Enable transmit mode (set DE pin high)
    GPIOA->BSHR = GPIO_Pin_12;

    // 3. Start DMA transfer
    DMA_Cmd(DMA1_Channel4, ENABLE);
    USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
}

// Transmit complete interrupt
void USART1_IRQHandler() {
    if (USART_GetITStatus(USART1, USART_IT_TC)) {
        USART_ClearITPendingBit(USART1, USART_IT_TC);
        // Return to receive mode (set DE pin low)
        GPIOA->BCR = GPIO_Pin_12;
    }
}
```

**DMA Benefits:**
- Non-blocking transmission
- CPU free during transfer
- Precise timing
- Reduced interrupt overhead

## Filament Data Structure

### In-Memory Representation

```cpp
struct _filament {
    char ID[8];              // Material identifier (e.g., "GFG00")
    uint8_t color_R;         // Red component (0-255)
    uint8_t color_G;         // Green component (0-255)
    uint8_t color_B;         // Blue component (0-255)
    uint8_t color_A;         // Alpha/transparency (0-255)
    int16_t temperature_min; // Minimum temp (°C)
    int16_t temperature_max; // Maximum temp (°C)
    char name[20];           // Material name (e.g., "PETG")
    float meters;            // Meters extruded
    uint64_t meters_virtual_count; // Virtual usage counter
    AMS_filament_stu statu;  // Status enum
    AMS_filament_motion motion_set; // Motion state
    uint16_t pressure;       // Pressure value (0xFFFF default)
};
```

### Flash Storage Format

**Flash Address:** `0x0800F000`

**Storage Structure:**
```cpp
struct flash_save_struct {
    _filament filament[4];           // All 4 channels
    int BambuBus_now_filament_num;   // Active channel (0xFF=none)
    uint8_t filament_use_flag;       // Usage flags (0x00-0x04)
    uint32_t version;                // Data version (5 currently)
    uint32_t check;                  // Validation (0x40614061)
} __attribute__((aligned(4)));
```

**Persistence:**
- Survives power cycles
- Loaded on boot via `Bambubus_read()`
- Saved when needed via `Bambubus_save()`
- Validated by version and checksum

### Filament States

```cpp
enum class AMS_filament_stu {
    offline,      // No filament detected
    online,       // Filament present and ready
    NFC_waiting   // Waiting for NFC read (not used in BMCU)
};
```cpp

### Motion States

```cpp
enum class AMS_filament_motion {
    before_pull_back,  // Preparing to retract
    need_pull_back,    // Actively retracting
    need_send_out,     // Sending filament to printer
    on_use,            // Currently printing
    idle               // No action required
};
```

**State Transitions:**
```
idle → need_send_out → on_use → before_pull_back → need_pull_back → idle
  ↑                                                                     ↓
  └─────────────────────────────────────────────────────────────────────┘
```

## Protocol Implementation Details

### Initialization

```cpp
void BambuBus_init() {
    // 1. Load saved filament data from flash
    bool init_ready = Bambubus_read();

    // 2. Initialize CRC calculators
    crc_8.reset(0x39, 0x66, 0, false, false);
    crc_16.reset(0x1021, 0x913D, 0, false, false);

    // 3. If flash data valid, load colors
    if (init_ready) {
        for (int i = 0; i < 4; i++) {
            channel_colors[i][0] = data_save.filament[i].color_R;
            channel_colors[i][1] = data_save.filament[i].color_G;
            channel_colors[i][2] = data_save.filament[i].color_B;
            channel_colors[i][3] = data_save.filament[i].color_A;
        }
    } else {
        // Initialize with default colors (Red, Green, Blue, Yellow)
        // ... (default initialization code)
    }

    // 4. Initialize UART hardware
    BambuBUS_UART_Init();
}
```

### Main Loop Processing

```cpp
BambuBus_package_type BambuBus_run() {
    // 1. Check if packet received
    if (BambuBus_have_data > 0) {
        // 2. Parse packet type
        BambuBus_package_type type = parse_packet(buf_X);

        // 3. Handle packet based on type
        switch (type) {
            case online_detect:
                // Respond with device type
                break;
            case set_filament_info:
                // Update filament properties
                // Save to flash
                break;
            case read_filament_info:
                // Send filament data to printer
                break;
            case filament_motion_short:
            case filament_motion_long:
                // Update motion state
                break;
            case heartbeat:
                // Respond to keepalive
                break;
            // ... other cases
        }

        // 4. Clear received flag
        BambuBus_have_data = 0;
        return type;
    }

    // 5. Check for timeout (offline detection)
    if (timeout_exceeded()) {
        return BambuBus_package_type::ERROR;
    }

    return BambuBus_package_type::NONE;
}
```

## Timing and Performance

### Baud Rate Calculation

```
Baud Rate: 1,250,000 bps
Bit Time: 1 / 1,250,000 = 0.8 microseconds
9-bit character: 9 × 0.8 = 7.2 microseconds
Maximum throughput: ~138,888 characters/second
```

### Typical Packet Sizes

- **Heartbeat:** ~10-20 bytes
- **Motion Command:** ~15-30 bytes
- **Filament Info:** ~50-100 bytes
- **NFC Data:** ~100-200 bytes (not used in BMCU)

### Latency

- **RX Interrupt:** < 10 microseconds
- **Packet Processing:** ~1-5 milliseconds
- **Response Time:** ~5-10 milliseconds
- **Flash Save:** ~50-100 milliseconds (blocks interrupts)

## Error Handling

### CRC Validation Failure

**CRC8 Failure:**
```cpp
if (data != _RX_IRQ_crcx.calc()) {
    _index = 0;  // Reset parser, wait for next 0x3D
    return;
}
```

**Result:** Packet discarded, no response sent

### Timeout Detection

**Implementation:**
```cpp
static unsigned long last_packet_time = 0;
unsigned long now = get_time64();

if (now - last_packet_time > TIMEOUT_MS) {
    return BambuBus_package_type::ERROR;
}
```

**Timeout Value:** Typically 3-10 seconds
**Result:** System enters offline state, red LED

### Buffer Overflow Protection

**RX Buffer:**
```cpp
if (_index >= 999) {  // Buffer size limit
    _index = 0;       // Reset to prevent overflow
}
```

**Maximum Packet Size:** 999 bytes (buffer size - 1)

## Security Considerations

### CRC Strength

- **CRC8:** Detects single-bit errors, most multi-bit errors
- **CRC16:** Very reliable for packet sizes used
- **Limitation:** Not cryptographic, doesn't prevent intentional tampering

### No Encryption

- **BambuBus uses plaintext communication**
- Assumes trusted local connection
- Physical security of RS-485 bus relied upon

### Authentication

**Device Type:** Simple identifier (0x1200 for AMS Lite)
- No challenge-response
- No certificates or keys
- Printer may implement basic filtering (future risk)

**NFC/RFID:** BMCU does not support
- Cannot read Bambu Lab filament tags
- No material verification

## Future Compatibility Risks

### Potential Changes by Bambu Lab

⚠️ **Firmware-based blocking:** Printer may reject non-official devices
⚠️ **Protocol encryption:** Could add encryption to prevent DIY devices
⚠️ **Authentication:** May require signing or certificates
⚠️ **Different protocol version:** Major changes could break compatibility

**Mitigation:**
- Keep BMCU firmware updated
- Monitor community for printer firmware updates
- Consider avoiding printer firmware updates if BMCU breaks
- Have backup plan (manual filament changes)

### Known Compatibility

**Working Printers:**
- ✅ Bambu Lab A1 (tested)
- ✅ Bambu Lab A1 mini (tested)
- 🔶 Bambu Lab P-series (requires modification)
- ❓ Newer models (unknown)

## Debugging the Protocol

### Enable Debug Output

```cpp
#define Debug_log_on
```

**Useful Debug Points:**
```cpp
// Log received packet type
DEBUG_MY("Packet type: ");
DEBUG_float((int)packet_type, 0);
DEBUG_MY("\n");

// Log packet length
DEBUG_MY("Length: ");
DEBUG_float(BambuBus_have_data, 0);
DEBUG_MY("\n");
```

### Logic Analyzer Capture

**Recommended Settings:**
- Sample Rate: 10 MHz minimum
- Channels: A, B, DE (optional)
- Trigger: Start byte (0x3D) on differential signal
- Decoder: Generic UART (1.25 Mbaud, 9N1 even parity)

### Oscilloscope Measurements

**What to Check:**
- Differential voltage (A - B)
- Signal integrity (clean edges)
- Timing accuracy
- Noise and reflections

**Expected Waveform:**
- Differential: ±2V to ±5V
- Rise/Fall Time: < 100 ns
- No ringing or overshoot

## Implementation Notes

### DMA Usage

**Benefits:**
- Efficient data transfer
- Minimal CPU overhead
- Precise timing
- Allows CPU to handle other tasks

**Configuration:**
```cpp
DMA1 Channel 4 (USART1 TX):
  - Priority: Very High
  - Direction: Memory to Peripheral
  - Mode: Normal (one-shot)
  - Memory Increment: Yes
  - Peripheral Increment: No
```

### Interrupt Priority

**USART1:** Priority 0 (highest)
- Ensures timely packet reception
- Prevents data loss
- Critical for protocol reliability

### Thread Safety

**Current Implementation:**
- Single-threaded (no RTOS)
- Interrupts modify shared state
- No mutex/semaphore protection

**Potential Issues:**
- Race conditions if main loop reads while ISR writes
- Mitigated by atomic operations and careful design

**Best Practice:**
- Disable interrupts during critical sections
- Use volatile for shared variables
- Consider adding proper synchronization if adding RTOS

## Testing and Validation

### Protocol Compliance

**Verify:**
- ✓ Correct CRC8/CRC16 calculation
- ✓ Proper packet framing (0x3D start)
- ✓ Baud rate accuracy (±2%)
- ✓ Timing requirements met
- ✓ No data corruption

**Test Tools:**
- Logic analyzer with protocol decoder
- Oscilloscope for signal quality
- Custom test jig for packet injection

### Integration Testing

**Test Scenarios:**
1. Power on, establish connection
2. Load filament, verify detection
3. Start print, monitor commands
4. Change filament mid-print
5. Complete print, retract filament
6. Power cycle, verify persistence

## References

### External Resources

- **Bambu Lab Community:** User reports on protocol
- **BMCU Wiki:** https://wiki.yuekai.fr/en/BMCU
- **Gitee Repository:** https://gitee.com/at_4061N/BMCU
- **RS-485 Standard:** TIA/EIA-485

### Related Documentation

- [Architecture Overview](architecture.md)
- [Development Setup](development-setup.md)

---

**Note:** This documentation is based on reverse engineering and community analysis. The BambuBus protocol is proprietary to Bambu Lab and subject to change without notice.
