# VESC Communication: CAN Bus vs UART

This document explains the differences between UART and CAN bus communication with VESC controllers and provides implementation guidance.

## Current Implementation (UART)

Your project currently uses **UART** communication:
- **Interface**: Serial communication over TX/RX pins
- **Speed**: 115200 baud
- **Protocol**: Custom VESC packet protocol with CRC
- **Pins**: TX=43, RX=44 (ESP32-S3)
- **Advantages**: 
  - Simple point-to-point connection
  - Well-documented protocol
  - Direct connection to VESC UART port
- **Disadvantages**:
  - Only one VESC per UART port
  - No network capabilities
  - Manual packet construction required

## CAN Bus Implementation

The provided CAN implementation offers:
- **Interface**: CAN bus network
- **Speed**: 500 kbps (standard for VESC)
- **Protocol**: VESC CAN message format
- **Pins**: TX=21, RX=22 (configurable)
- **Advantages**:
  - Multiple VESCs on same bus
  - Network topology support
  - Automatic status broadcasting
  - Built-in error detection
  - Real-time communication
- **Disadvantages**:
  - Requires CAN transceiver hardware
  - More complex setup
  - Network configuration needed

## Hardware Requirements

### UART (Current)
```
ESP32-S3 ←→ VESC
    TX43 ←→ RX
    RX44 ←→ TX
     GND ←→ GND
```

### CAN Bus (New)
```
ESP32-S3 ←→ CAN Transceiver ←→ CAN Bus ←→ VESC(s)
    TX21 ←→ CTX             ←→ CAN_H   ←→ CAN_H
    RX22 ←→ CRX             ←→ CAN_L   ←→ CAN_L
                            ←→ GND     ←→ GND
```

**Required CAN Hardware:**
- MCP2515 CAN controller + MCP2551 transceiver, OR
- ESP32 built-in CAN controller + external transceiver (TJA1050, etc.)

## Protocol Differences

### UART Messages
```cpp
// Request values
uint8_t command = COMM_GET_VALUES;
send_packet(&command, 1);

// Receive complete telemetry packet (60+ bytes)
parse_values(payload, length);
```

### CAN Messages
```cpp
// Automatic status broadcasts from VESC
// Status 1: RPM + Current (8 bytes)
// Status 2: Voltage + Temperature (8 bytes)  
// Status 3: Amp Hours (8 bytes)
// Status 4: Tachometer (8 bytes)

// Commands sent as CAN frames
set_current(10.0f); // Sends 4-byte CAN message
```

## Configuration Changes Needed

### 1. Add CAN Library
Add to `platformio.ini`:
```ini
lib_deps = 
    sandeepmistry/CAN@^0.3.1
```

### 2. Update VESC_Config.h
```cpp
// Add CAN configuration
#define USE_CAN_COMMUNICATION 1  // 0 for UART, 1 for CAN
#define VESC_CAN_ID 0           // VESC ID on CAN bus (0-255)
```

### 3. Modify Main Code
Replace UART initialization:
```cpp
// Old UART version
#include "VESC_Driver.h"
void setup() {
    VESC_Init();
}
void loop() {
    VESC_Loop();
}

// New CAN version  
#include "VESC_CAN_Driver.h"
void setup() {
    VESC_CAN_Init(0); // VESC ID = 0
}
void loop() {
    VESC_CAN_Loop();
}
```

## VESC Configuration

### UART Setup
1. Connect VESC via USB to PC
2. Open VESC Tool
3. Go to App Settings → UART
4. Set baud rate to 115200
5. Enable UART communication

### CAN Setup  
1. Connect VESC via USB to PC
2. Open VESC Tool
3. Go to App Settings → CAN
4. Set CAN ID (0-255, unique per VESC)
5. Set CAN baud rate to 500k
6. Enable CAN status messages
7. Configure broadcast intervals

## Performance Comparison

| Feature | UART | CAN Bus |
|---------|------|---------|
| Latency | ~10ms | ~1-5ms |
| Bandwidth | 115kbps | 500kbps |
| Multiple VESCs | No | Yes |
| Network topology | Point-to-point | Multi-drop |
| Error detection | CRC | Built-in |
| Real-time | Limited | Excellent |
| Complexity | Low | Medium |

## Migration Steps

1. **Test Current UART**: Ensure UART version works perfectly
2. **Add CAN Hardware**: Connect CAN transceiver to ESP32
3. **Install CAN Library**: Add to platformio.ini
4. **Configure VESC**: Set up CAN in VESC Tool
5. **Switch Code**: Replace UART driver with CAN driver
6. **Test & Debug**: Verify CAN communication
7. **Optimize**: Fine-tune message rates and filters

## Recommendation

- **Keep UART** if you have single VESC and current setup works well
- **Switch to CAN** if you need:
  - Multiple VESCs
  - Lower latency
  - Network capabilities
  - Real-time control

The CAN implementation provides better performance and scalability, but UART is simpler and sufficient for single-VESC applications.
