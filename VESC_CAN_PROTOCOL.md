# VESC CAN Protocol Implementation

Эта документация основана на официальной спецификации VESC CAN протокола от [vesc-project.com](https://vesc-project.com/sites/default/files/imce/u15301/VESC6_CAN_CommandsTelemetry.pdf).

## Обзор протокола

VESC использует расширенные CAN фреймы (29-bit ID) для команд и телеметрии:
- **Биты 0-7**: VESC ID (0-255) - идентификатор конкретного VESC
- **Биты 8-15**: Номер команды
- **Биты 16-28**: Зарезервированы (обычно 0)

## Команды управления

### Основные команды (обновлено в VESC_CAN_Driver)

| Команда | ID | Описание | Формат данных |
|---------|----|-----------|--------------| 
| `CAN_PACKET_SET_DUTY` | 0 | Установить duty cycle | int32 * 100000 (-1 to 1) |
| `CAN_PACKET_SET_CURRENT` | 1 | Установить ток | int32 в миллиамперах |
| `CAN_PACKET_SET_CURRENT_BRAKE` | 2 | Установить тормозной ток | int32 в миллиамперах |
| `CAN_PACKET_SET_RPM` | 3 | Установить RPM | int32 RPM |
| `CAN_PACKET_PING` | 17 | Ping запрос | Нет данных |
| `CAN_PACKET_PONG` | 18 | Pong ответ | Нет данных |

### Пример использования команд
```cpp
// Установить ток 10A
vesc_can.set_current(10.0f);  // Отправляет CAN_PACKET_SET_CURRENT с данными 10000

// Установить duty cycle 50%
vesc_can.set_duty_cycle(0.5f);  // Отправляет CAN_PACKET_SET_DUTY с данными 50000

// Установить RPM 1000
vesc_can.set_rpm(1000);  // Отправляет CAN_PACKET_SET_RPM с данными 1000
```

## Телеметрия (Статусные сообщения)

VESC автоматически отправляет до 5 различных статусных сообщений:

### Status Message 1 (`CAN_PACKET_STATUS` = 9)
**Содержимое (8 байт):**
- RPM (32-bit int)
- Total Current × 10 (16-bit int) 
- Duty Cycle × 1000 (16-bit int)

```cpp
void parse_status_1(uint8_t* payload, int length) {
    values.rpm = (float)buffer_get_int32(payload, &index);
    values.current_motor = (float)buffer_get_int16(payload, &index) / 10.0f;
    values.duty_now = (float)buffer_get_int16(payload, &index) / 1000.0f;
}
```

### Status Message 2 (`CAN_PACKET_STATUS_2` = 14)
**Содержимое (8 байт):**
- Amp Hours Charged × 10000 (32-bit int)
- Amp Hours Consumed × 10000 (32-bit int)

### Status Message 3 (`CAN_PACKET_STATUS_3` = 15)  
**Содержимое (8 байт):**
- Watt Hours Charged × 10000 (32-bit int)
- Watt Hours Consumed × 10000 (32-bit int)

### Status Message 4 (`CAN_PACKET_STATUS_4` = 16)
**Содержимое (8 байт):**
- MOSFET Temperature × 10 (16-bit int)
- Motor Temperature × 10 (16-bit int) 
- Input Current × 10 (16-bit int)
- PID Position × 50 (16-bit int)

### Status Message 5 (`CAN_PACKET_STATUS_5` = 27)
**Содержимое (8 байт):**
- Reserved (16-bit int)
- Input Voltage × 10 (16-bit int)
- Tachometer Value (32-bit int)

## CAN ID Format

Для VESC ID = 0 и команды SET_CURRENT:
```
Extended CAN ID (29-bit): 0x00000100
- Bits 0-7:   0x00 (VESC ID = 0)
- Bits 8-15:  0x01 (CAN_PACKET_SET_CURRENT)
- Bits 16-28: 0x000 (Reserved)
```

## Настройка VESC для CAN

### В VESC Tool:
1. **App Settings → CAN**
2. **CAN ID**: 0-255 (уникальный)
3. **CAN Baud Rate**: 500k
4. **Enable CAN status messages**: ✓
5. **Status send rate**: 50-100 Hz
6. **Enable status messages 1-5**: ✓

### Настройка статусных сообщений:
- **Status 1**: Всегда включен (RPM, ток, duty)
- **Status 2**: Включить для Ah данных  
- **Status 3**: Включить для Wh данных
- **Status 4**: Включить для температур
- **Status 5**: Включить для напряжения и тахометра

## Реализация в VESC_CAN_Driver

### Структура данных
```cpp
typedef struct {
    // Status Message 1
    float rpm;
    float current_motor; 
    float duty_now;
    
    // Status Message 2
    float amp_hours;
    float amp_hours_charged;
    
    // Status Message 3
    float watt_hours;
    float watt_hours_charged;
    
    // Status Message 4  
    float temp_fet;
    float temp_motor;
    float current_in;
    float pid_pos_now;
    
    // Status Message 5
    int32_t tachometer;
    float v_in;
    
    // Дополнительные поля
    int32_t tachometer_abs;
    uint8_t fault_code;
    uint8_t vesc_id;
    unsigned long last_update;
    bool connected;
} vesc_can_values_t;
```

### Парсинг сообщений
```cpp
bool process_packet(uint8_t* payload, int length) {
    uint8_t command = payload[0];
    
    switch (command) {
        case CAN_PACKET_STATUS:     parse_status_1(payload + 1, length - 1); break;
        case CAN_PACKET_STATUS_2:   parse_status_2(payload + 1, length - 1); break;
        case CAN_PACKET_STATUS_3:   parse_status_3(payload + 1, length - 1); break;
        case CAN_PACKET_STATUS_4:   parse_status_4(payload + 1, length - 1); break;
        case CAN_PACKET_STATUS_5:   parse_status_5(payload + 1, length - 1); break;
        case CAN_PACKET_PONG:       return true; // Ping response
    }
}
```

## Преимущества CAN протокола

1. **Эффективность**: Компактные 8-байт сообщения
2. **Масштабируемость**: До 255 VESC на одной шине
3. **Надежность**: Встроенная проверка ошибок CAN
4. **Реальное время**: Автоматические статусные сообщения
5. **Стандартизация**: Официальный протокол VESC

## Отличия от UART

| Параметр | UART | CAN |
|----------|------|-----|
| Протокол | Пакетный с CRC | CAN фреймы |
| Размер данных | ~60 байт | 8 байт |
| Частота обновления | По запросу | Автоматически |
| Количество VESC | 1 | До 255 |
| Надежность | CRC16 | CAN встроенная |
| Сложность | Низкая | Средняя |

## Совместимость

VESC_CAN_Driver полностью совместим с:
- VESC Tool CAN настройками
- Официальной спецификацией VESC CAN
- Множественными VESC на одной шине
- Стандартными CAN конверторами

## Использование

```cpp
#include "VESC_CAN_Driver.h"

void setup() {
    VESC_CAN_Init();  // Инициализация CAN драйвера
}

void loop() {
    VESC_CAN_Loop();  // Обработка CAN сообщений
    
    if (vesc_can.is_connected()) {
        float speed = vesc_can.get_speed_kmh();
        float battery = vesc_can.get_battery_percentage();
        float power = vesc_can.get_power_watts();
        
        // Управление
        vesc_can.set_current(5.0f);  // 5A
    }
}
```

Этот драйвер теперь полностью соответствует официальной спецификации VESC CAN протокола и должен корректно работать с любыми VESC контроллерами, настроенными для CAN связи.
