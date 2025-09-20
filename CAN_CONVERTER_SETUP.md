# VESC CAN Converter Setup

Эта конфигурация использует CAN конвертер, подключенный к Serial0 ESP32-S3 для связи с VESC по CAN шине.

## Новая архитектура драйверов

Теперь у вас есть два драйвера:

1. **VESC_Driver** - Оригинальный UART драйвер (Serial1)
2. **VESC_CAN_Driver** - Новый CAN конвертер драйвер (Serial0)
3. **VESC_Interface** - Унифицированный интерфейс для переключения между драйверами

## Аппаратная конфигурация

```
ESP32-S3 ←→ CAN Converter ←→ CAN Bus ←→ VESC
Serial0  ←→ UART/Serial   ←→ CAN_H/L  ←→ CAN_H/L
TX: 43   ←→ RX            
RX: 44   ←→ TX            
GND      ←→ GND           ←→ GND      ←→ GND
```

## CAN Converter Types

### Популярные CAN конверters:
1. **CANable/CANtact** - USB CAN адаптер с UART интерфейсом
2. **MCP2515 + UART** - Модуль с микроконтроллером
3. **SN65HVD230** - CAN трансивер с UART
4. **Custom CAN-UART bridge** - Кастомное решение

## Настройки

### ESP32-S3 Configuration
- **Serial Port**: Serial0 (Hardware UART0)
- **TX Pin**: 43
- **RX Pin**: 44  
- **Baud Rate**: 115200 (проверьте спецификацию вашего конвертера)
- **Format**: 8N1 (8 data bits, no parity, 1 stop bit)

### CAN Converter Settings
Убедитесь, что ваш CAN конвертер настроен на:
- **CAN Speed**: 500 kbps (стандарт для VESC)
- **UART Speed**: 115200 bps
- **Protocol**: Transparent mode или VESC-compatible

### VESC Configuration
1. Подключите VESC к компьютеру через USB
2. Откройте VESC Tool
3. Перейдите в **App Settings → CAN**
4. Установите:
   - **CAN ID**: 0-255 (уникальный для каждого VESC)
   - **CAN Baud Rate**: 500k
   - **Enable CAN status messages**: ✓
   - **Status send rate**: 50-100 Hz

## Протокол связи

### Отправка команд (ESP32 → VESC)
```cpp
// Команды отправляются через CAN конвертер
vesc.set_current(10.0f);     // Установить ток 10A
vesc.set_rpm(1000);          // Установить RPM 1000
vesc.set_brake_current(5.0f); // Тормозной ток 5A
```

### Получение данных (VESC → ESP32)
```cpp
// Данные приходят автоматически через CAN статус сообщения
vesc_values_t values = vesc.get_values();
float speed = vesc.get_speed_kmh();
float battery = vesc.get_battery_percentage();
float power = vesc.get_power_watts();
```

## Диагностика

### Проверка подключения
```cpp
if (vesc.is_connected()) {
    Serial.println("VESC connected via CAN converter");
} else {
    Serial.println("VESC not connected - check CAN converter");
}
```

### Serial Monitor Output
При правильной настройке вы увидите:
```
VESC Display Starting...
Initializing VESC via CAN converter on Serial0...
VESC initialized with CAN converter configuration:
  CAN Converter: Serial0 (RX:44, TX:43)
  Baud Rate: 115200
  Wheel diameter: 0.097 m
  Motor poles: 14
  Gear ratio: 1.0
  Battery: 10S 5.0Ah
VESC connection established
```

## Устранение неисправностей

### Нет связи с VESC
1. **Проверьте подключение проводов**:
   - TX ESP32 → RX Converter
   - RX ESP32 → TX Converter
   - GND общий

2. **Проверьте настройки CAN конвертера**:
   - Правильная скорость UART (115200)
   - Правильная скорость CAN (500k)
   - Режим transparent/bridge

3. **Проверьте настройки VESC**:
   - CAN включен в VESC Tool
   - Правильный CAN ID
   - Status messages включены

4. **Проверьте CAN шину**:
   - CAN_H и CAN_L правильно подключены
   - Терминальные резисторы 120Ω на концах шины
   - Длина проводов не более 1м для 500kbps

### Периодические разрывы связи
- Увеличьте `CONNECTION_TIMEOUT_MS` в конфигурации
- Проверьте качество CAN проводов
- Убедитесь в стабильном питании конвертера

### Неправильные данные
- Проверьте конфигурацию мотора и колес
- Убедитесь, что VESC ID соответствует настройкам
- Проверьте масштабирование данных в конвертере

## Преимущества CAN Converter

1. **Простота интеграции**: Использует существующий UART код
2. **Изоляция**: CAN конвертер изолирует ESP32 от CAN шины
3. **Надежность**: CAN протокол более устойчив к помехам
4. **Расширяемость**: Можно подключить несколько VESC
5. **Совместимость**: Работает с любым CAN конвертером

## Как переключиться на CAN конвертер

### Шаг 1: Изменить конфигурацию
В файле `VESC_Config.h` измените:
```cpp
#define USE_CAN_CONVERTER 1  // Включить CAN конвертер
```

### Шаг 2: Обновить main код
В файле `LVGL_Arduino.ino` замените:
```cpp
// Старый код
#include "VESC_Driver.h"
void setup() {
    VESC_Init();
}
void DriverTask(void *parameter) {
    VESC_Loop();
}

// Новый код с унифицированным интерфейсом
#include "VESC_Interface.h"
void setup() {
    VESC_Interface_Init();  // Автоматически выберет драйвер
}
void DriverTask(void *parameter) {
    VESC_Interface_Loop();  // Работает с любым драйвером
}
```

### Шаг 3: Обновить использование данных
```cpp
// Вместо прямого обращения к vesc
float speed = vesc.get_speed_kmh();
bool connected = vesc.is_connected();

// Используйте унифицированный интерфейс
float speed = VESC_Interface_GetSpeedKmh();
bool connected = VESC_Interface_IsConnected();
```

## Преимущества новой архитектуры

1. **Гибкость**: Легкое переключение между UART и CAN
2. **Совместимость**: Существующий код не нуждается в больших изменениях
3. **Тестирование**: Можно тестировать оба интерфейса
4. **Масштабируемость**: Легко добавить новые интерфейсы

## Рекомендации

- Используйте экранированные провода для CAN шины
- Располагайте CAN конвертер близко к ESP32
- Проверьте спецификацию конвертера для оптимальных настроек
- Тестируйте на низких скоростях перед полным использованием
- Используйте `VESC_Interface` для будущей совместимости
