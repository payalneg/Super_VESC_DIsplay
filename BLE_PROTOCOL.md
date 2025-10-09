# BLE Protocol Documentation

## Overview

Super VESC Display поддерживает BLE коммуникацию с использованием VESC протокола с обрамлением (framing).

## BLE Service Configuration

- **Device Name:** `SuperVESCDisplay`
- **Service UUID:** `6E400001-B5A3-F393-E0A9-E50E24DCCA9E` (Nordic UART Service)
- **RX Characteristic:** `6E400002-B5A3-F393-E0A9-E50E24DCCA9E` (Write)
- **TX Characteristic:** `6E400003-B5A3-F393-E0A9-E50E24DCCA9E` (Notify)

## Packet Framing Protocol

### Short Packets (≤255 bytes payload)

```
┌────────┬────────┬──────────┬─────────┬──────────┐
│ 0x02   │ LENGTH │ PAYLOAD  │ CRC_H   │ 0x03     │
│(start) │(1 byte)│(N bytes) │ CRC_L   │(end)     │
└────────┴────────┴──────────┴─────────┴──────────┘
```

### Long Packets (>255 bytes payload)

```
┌────────┬─────────┬─────────┬──────────┬─────────┬──────────┐
│ 0x04   │ LENGTH_H│ LENGTH_L│ PAYLOAD  │ CRC_H   │ 0x05     │
│(start) │(1 byte) │(1 byte) │(N bytes) │ CRC_L   │(end)     │
└────────┴─────────┴─────────┴──────────┴─────────┴──────────┘
```

### CRC Calculation

- **Algorithm:** CRC16 (same as VESC firmware)
- **Applied to:** PAYLOAD only (not including framing bytes)

## Command Format

### Example 1: Request Firmware Version

**Mobile App → Device:**
```
02 01 00 AB CD 03
│  │  │  └──┴─ CRC16 of [0x00]
│  │  └────── COMM_FW_VERSION (0x00)
│  └───────── Payload length (1 byte)
└──────────── Start byte
```

**Device → Mobile App:**
```
02 3C 00 01 02 53 75 70 65 72 ... 03
│  │  │  │  │  └──────────────────┴─ HW name, MAC, UUID, etc.
│  │  │  │  └─ FW minor version
│  │  │  └──── FW major version
│  │  └─────── COMM_FW_VERSION response
│  └────────── Payload length (60 bytes)
└───────────── Start byte
```

### Example 2: Request with Device Address

Если команда адресована конкретному устройству:

**Mobile App → Device:**
```
02 02 02 00 XX XX 03
│  │  │  │  └──┴─ CRC16
│  │  │  └────── COMM_FW_VERSION (0x00)
│  │  └───────── Device address (0x02 = default CAN ID)
│  └──────────── Payload length (2 bytes)
└─────────────── Start byte
```

Устройство проверяет первый байт payload:
- Если = `0x02` (наш CAN ID) → скипаем и обрабатываем остальное
- Иначе → обрабатываем весь payload как команду

## Supported VESC Commands

### Query Commands
- `0x00` - **COMM_FW_VERSION** - Get firmware version and device info
- `0x04` - **COMM_GET_VALUES** - Get current values (RPM, current, voltage, etc.)
- `0x33` - **COMM_GET_CUSTOM_CONFIG** - Get custom configuration
- `0x34` - **COMM_GET_CUSTOM_CONFIG_DEFAULT** - Get default configuration
- `0x35` - **COMM_GET_CUSTOM_CONFIG_XML** - Get configuration XML

### Control Commands
- `0x05` - **COMM_SET_DUTY** - Set duty cycle
- `0x06` - **COMM_SET_CURRENT** - Set motor current
- `0x07` - **COMM_SET_CURRENT_BRAKE** - Set brake current
- `0x08` - **COMM_SET_RPM** - Set motor RPM
- `0x09` - **COMM_SET_POS** - Set position

### Configuration Commands
- `0x36` - **COMM_SET_CUSTOM_CONFIG** - Update custom configuration

## Architecture

### Receive Path
```
BLE RX Characteristic (Write)
        ↓
Packet Parser (byte-by-byte)
        ↓
CRC Validation
        ↓
Check Device Address (if present)
        ↓
vesc_handler_process_command()
        ↓
Generate Response
        ↓
response_callback (BLE_SendFramedResponse)
        ↓
Build Framed Packet
        ↓
BLE TX Characteristic (Notify)
```

### Send Path (Responses)

1. **Via BLE:** `response_callback` → `BLE_SendFramedResponse()` → Frame packet → Notify
2. **Via CAN:** No callback → `comm_can_send_buffer()` → CAN bus

## Implementation Files

- **`src/packet_parser.h/cpp`** - Framing protocol parser
- **`src/ble_vesc_driver.h/cpp`** - BLE communication layer
- **`src/vesc_handler.h/cpp`** - VESC command handler with callback support
- **`src/main.cpp`** - Initialization and callback registration

## Testing

### Using Nordic nRF Connect App

1. Scan for device: **"SuperVESCDisplay"**
2. Connect to service: `6E400001-B5A3-F393-E0A9-E50E24DCCA9E`
3. Enable notifications on TX: `6E400003...`
4. Write to RX: `6E400002...`

**Test Command (FW_VERSION):**
```
Send: 02 01 00 00 00 03
```

Expected response format:
```
02 <len> 00 <fw_major> <fw_minor> <hw_name...> ... 03
```

### Using VESC Tool Mobile App

The device should be compatible with official VESC Tool mobile apps that support BLE communication.

## Debugging

Enable debug output via Serial Monitor (115200 baud):

```
📥 BLE: received 6 bytes: 02 01 00 00 00 03
✅ Valid packet received: 1 bytes (CRC: 0x0000)
📦 BLE: Parsed complete packet (1 bytes)
📍 BLE: Processing as direct VESC command
[VESC CMD #0001] Len=1, CMD=0x00 (FW_VERSION) - Sending response
✅ FW_VERSION response sent: SuperVESCDisplay v1.00
📤 BLE: Sent framed response (65 bytes total, 60 payload): 02 3C 00 01 ...
```

## Notes

- Default device CAN ID: **2** (`CONF_CONTROLLER_ID`)
- MTU size automatically negotiated with client
- Large responses (>MTU) are automatically fragmented
- Parser maintains state between chunks for multi-write scenarios

