# Trip and Amp-Hours Persistence

## Описание

Модуль `vesc_trip_persist` обеспечивает сохранение расстояния поездки (trip) и использованных ампер-часов (amp-hours) в энергонезависимую память (NVS). Данные автоматически сохраняются каждые 10 секунд и восстанавливаются при включении устройства.

## Основные возможности

- ✅ Автоматическое сохранение trip и amp-hours каждые 10 секунд
- ✅ Восстановление значений при включении (не теряются при выключении контроллера)
- ✅ Автоматическое определение сброса VESC (когда контроллер был выключен)
- ✅ Функция ручного сброса trip и amp-hours
- ✅ Интеграция с существующими функциями отображения

## Как это работает

### 1. Автоматическое сохранение

Модуль автоматически сохраняет текущие значения trip и amp-hours в NVS каждые 10 секунд:

```cpp
// Вызывается автоматически из vesc_rt_data_process_response()
trip_persist_update(rt_data.tachometer_abs, rt_data.amp_hours);
```

### 2. Определение сброса VESC

Когда VESC выключается и включается снова, его внутренние счетчики сбрасываются в ноль. Модуль автоматически определяет это и добавляет offset для сохранения непрерывности:

```
При старте:
  VESC trip = 0 м
  Saved trip = 5000 м
  → Offset = 5000 м
  → Displayed trip = 0 + 5000 = 5000 м

После поездки 1 км:
  VESC trip = 1000 м
  Offset = 5000 м
  → Displayed trip = 1000 + 5000 = 6000 м
```

### 3. Получение значений

Все существующие функции уже используют persistent значения:

```cpp
// В вашем коде используйте эти функции:
float trip_km = vesc_rt_data_get_trip_km();        // Возвращает trip с учетом сохраненных данных
float amp_hours = vesc_rt_data_get_amp_hours();    // Возвращает amp-hours с учетом сохраненных данных
```

### 4. Сброс trip и amp-hours

Для сброса trip и amp-hours (например, при замене аккумулятора) используйте:

```cpp
#include "vesc_battery_calc.h"

// Сбросить trip и amp-hours в ноль
battery_calc_reset_trip_and_ah();
```

Эта функция:
- Сбрасывает счетчики trip и amp-hours в ноль
- Очищает сохраненные значения в NVS
- Записывает информацию в лог

## Интеграция

Модуль уже полностью интегрирован в существующий код:

### Файлы модуля
- `src/vesc_trip_persist.h` - заголовочный файл
- `src/vesc_trip_persist.cpp` - реализация

### Изменения в существующих файлах

#### `src/vesc_rt_data.cpp`
- Инициализация модуля в `vesc_rt_data_init()`
- Обновление значений в `vesc_rt_data_process_response()`
- Функции `vesc_rt_data_get_trip_km()` и `vesc_rt_data_get_amp_hours()` используют persistent значения
- Расчет range и efficiency используют persistent trip

#### `src/ui_updater.cpp`
- Отображение amp-hours использует `vesc_rt_data_get_amp_hours()`

#### `src/vesc_battery_calc.h` и `src/vesc_battery_calc.cpp`
- Добавлена функция `battery_calc_reset_trip_and_ah()` для сброса

## Пример использования

### Сценарий 1: Обычная работа

```
1. Устройство включается
   → Загружаются сохраненные значения: Trip = 10.5 км, Ah = 2.3
   
2. Едем 5 км, потребляем 1.2 Ah
   → Каждые 10 секунд значения автоматически сохраняются
   → Trip = 15.5 км, Ah = 3.5
   
3. Выключаем контроллер
   → Последние значения сохранены: Trip = 15.5 км, Ah = 3.5
   
4. Включаем контроллер снова
   → Загружаются сохраненные значения
   → Продолжаем с Trip = 15.5 км, Ah = 3.5 (не потеряли данные!)
```

### Сценарий 2: Замена аккумулятора

```cpp
// В вашем коде (например, в обработчике кнопки "Сброс"):
void on_battery_swap_button_pressed() {
    LOG_INFO(UI, "User requested battery swap reset");
    
    // Сбросить trip и amp-hours
    battery_calc_reset_trip_and_ah();
    
    // Также можно сбросить battery calculation, если нужно
    // battery_calc_reset(100.0f, battery_capacity);
}
```

## API функций

### vesc_trip_persist.h

```cpp
// Инициализация (вызывается автоматически)
void trip_persist_init(void);

// Обновление значений (вызывается автоматически каждые ~100мс)
void trip_persist_update(float vesc_trip_meters, float vesc_amp_hours);

// Получение persistent trip в км
float trip_persist_get_trip_km(void);

// Получение persistent amp-hours
float trip_persist_get_amp_hours(void);

// Сброс trip и amp-hours в ноль
void trip_persist_reset(void);

// Проверка инициализации
bool trip_persist_is_initialized(void);
```

### vesc_battery_calc.h (новая функция)

```cpp
// Сброс trip и amp-hours (для пользовательского интерфейса)
void battery_calc_reset_trip_and_ah(void);
```

## Логирование

Модуль выводит информацию в лог:

```
[SYSTEM] Trip persistence module initialized with saved state
[SYSTEM] Trip offsets calculated: Trip offset=5000.00 m, Ah offset=2.30
[SYSTEM] Trip persist: Trip=15.50 km (VESC=10500.00 m + offset=5000.00 m), Ah=3.50 (VESC=1.20 + offset=2.30)
```

При сбросе:
```
[SYSTEM] Resetting trip and amp-hours due to battery swap
[SYSTEM] Resetting trip and amp-hours to zero
[SYSTEM] Trip and amp-hours reset complete
```

## Примечания

1. **Износ NVS**: Сохранение происходит каждые 10 секунд, что является компромиссом между частотой обновления и износом flash-памяти.

2. **Точность**: Точность зависит от частоты обновления данных от VESC (обычно ~100мс).

3. **Обратная совместимость**: Если в NVS нет сохраненных данных (первый запуск), модуль начинает с нуля.

4. **Автоматическое восстановление**: При обнаружении уменьшения значений VESC (power cycle) модуль автоматически корректирует offset.

## Настройка

Интервал сохранения можно изменить в `src/vesc_trip_persist.cpp`:

```cpp
// Текущее значение: 10 секунд
if (now - last_save_time >= 10000) {
    trip_persist_save_state();
    last_save_time = now;
}
```

Для более частого сохранения измените `10000` на меньшее значение (в миллисекундах).

