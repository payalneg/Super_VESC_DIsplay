# Smart Battery Calculation Feature

## Overview

The Super VESC Display now includes an intelligent battery calculation system that provides more accurate battery percentage readings by tracking actual energy consumption instead of relying solely on the voltage-based percentage from the VESC controller.

## How It Works

### Direct Mode (Default)
- Uses battery percentage directly from the VESC controller
- Based on battery voltage mapping
- Simple and straightforward
- May be less accurate during discharge due to voltage sag

### Smart Calculation Mode
The smart calculation mode works with persistent state storage and automatic charging detection:

1. **Persistent State Storage**
   - Remaining battery capacity (Ah) is saved to flash memory every 30 seconds
   - Last battery percentage is saved for charging detection
   - State survives device restarts and continues seamlessly

2. **Automatic Charging Detection (on startup)**
   - Loads saved battery percentage (e.g., 70%)
   - Reads current percentage from controller (e.g., 90%)
   - If difference > 10% → Battery was charged! Reset to controller reading
   - If difference < 10% → Continue from saved state
   - Example: Saved 70%, Current 72% → Continue tracking
   - Example: Saved 70%, Current 90% → Battery charged, reset to 90%

3. **Continuous Tracking**
   - Monitors amp-hours consumed from the controller
   - Updates remaining capacity in real-time
   - Example: If 0.5 Ah used in 30 sec → Remaining decreases by 0.5 Ah
   - Accounts for regenerative braking (negative consumption adds energy back)

4. **Percentage Calculation**
   - Calculates current percentage: `(Remaining Ah / Capacity Ah) × 100`
   - More accurate during discharge than voltage-based methods
   - Saves state periodically to survive restarts

5. **Automatic Reset Conditions**
   - Battery capacity changed in settings → Reset immediately
   - Large charge detected (>10% increase) → Reset to controller reading
   - Manual mode switch → State preserved for next Smart mode enable

## Configuration

### Settings Menu

Navigate to Settings screen to configure:

1. **Battery Capacity (Ah)**
   - Set your actual battery capacity in amp-hours
   - Default: `15.0 Ah`
   - Range: `1.0` to `200.0 Ah` (0.1 Ah increments)
   - Use +/- buttons to adjust
   - Example capacities:
     - Small pack: 10-15 Ah
     - Medium pack: 15-20 Ah
     - Large pack: 20-30 Ah
     - E-bike: 10-20 Ah
     - E-scooter: 7-15 Ah

2. **Battery Calculation Mode**
   - **Direct from Controller**: Traditional voltage-based percentage
   - **Smart Calculation**: Energy-based calculation using Ah consumption
   - Settings are saved automatically

### When to Use Each Mode

**Use Direct Mode when:**
- You want simple, traditional battery indication
- Your battery has very consistent voltage curve
- You prefer voltage-based readings
- Testing or troubleshooting

**Use Smart Calculation when:**
- You want more accurate remaining capacity
- Your rides vary in power consumption
- You need better range estimation
- You have regenerative braking
- Battery voltage sags under load

## Important Notes

### Calibration & State Persistence
- **Automatic Calibration**: System detects charging automatically (>10% increase)
- **Persistent State**: Battery state is saved every 30 seconds to flash memory
- **Seamless Restart**: Device continues from saved state after restart
- To force recalibration:
  - Charge battery >10% and restart device
  - Change battery capacity setting
  - System will automatically reset to controller reading

### Accuracy Factors
The smart calculation accuracy depends on:
- Correct battery capacity setting
- Controller's amp-hour accuracy
- Initial battery level reading from controller
- Battery actual capacity (may degrade over time)

### Regenerative Braking
- Smart calculation accounts for regen braking
- If amp-hours decrease, battery percentage increases
- More accurate than voltage-based methods during regen

### Display Update
- Battery percentage updates in real-time
- Calculation logged every 5 seconds in debug mode
- Shows: Initial Ah, Used Ah, Remaining Ah, Percentage

## Example Scenarios

### Scenario 1: Fresh Battery (First Use)
```
Battery Capacity: 15.0 Ah
No saved state found
Current Reading: 100% → 15.0 Ah
After 30 min riding: -3.5 Ah consumed
Remaining: 15.0 - 3.5 = 11.5 Ah
Display: 76.7%
State saved: 76.7%, 11.5 Ah
```

### Scenario 2: Restart After Riding (No Charging)
```
Battery Capacity: 15.0 Ah
Saved state: 76.7%, 11.5 Ah
Current reading: 78% (voltage recovered at rest)
Difference: 78 - 76.7 = 1.3% (< 10% threshold)
Result: Continue from saved 11.5 Ah
After 20 min riding: -2.0 Ah consumed
Remaining: 11.5 - 2.0 = 9.5 Ah
Display: 63.3%
```

### Scenario 3: Restart After Charging
```
Battery Capacity: 15.0 Ah
Saved state: 63.3%, 9.5 Ah
Current reading: 95% (battery was charged!)
Difference: 95 - 63.3 = 31.7% (> 10% threshold)
Result: ✓ Charging detected! Reset to controller reading
New remaining: 95% × 15.0 = 14.25 Ah
Display: 95%
```

### Scenario 4: With Regeneration
```
Battery Capacity: 15.0 Ah
Current remaining: 7.5 Ah (50%)
Downhill with regen: -0.8 Ah consumed (negative = energy added)
Remaining: 7.5 - (-0.8) = 8.3 Ah
Display: 55.3%
State saved automatically
```

### Scenario 5: Capacity Changed
```
Battery Capacity: 15.0 Ah → Changed to 20.0 Ah
Saved state: 9.0 Ah remaining
Current reading: 60%
Result: ✓ Capacity change detected! Reset immediately
New remaining: 60% × 20.0 = 12.0 Ah
Display: 60%
State saved with new capacity
```

## Troubleshooting

### Battery percentage seems wrong
1. Verify battery capacity setting matches your actual pack
2. Try recalibrating by restarting the device
3. Check if controller amp-hour reading is accurate
4. Compare with voltage-based reading (Direct mode)

### Percentage increases during use
- This is normal during regenerative braking
- Check controller amp-hours value (may be negative during regen)

### Percentage doesn't update
1. Check if ESC is connected (should not show "Not Connected")
2. Verify CAN communication is working
3. Switch to Direct mode to test if controller data is received

### Reset doesn't work
- Settings reset button resets to defaults (15.0 Ah, Direct mode)
- For recalibration, restart the device or toggle modes

## Technical Details

### Data Sources
- **Controller Battery Level**: From VESC RT data (`battery_level`)
- **Amp-Hours**: From VESC RT data (`amp_hours`)
- **Battery Capacity**: User-configured in settings

### Storage
- Settings saved to NVS (non-volatile storage)
- Persists across power cycles
- No cloud or external storage required

### Performance
- Updates every 50ms with UI refresh
- Minimal CPU overhead
- Logs calculation every 5 seconds for debugging

## Summary

Smart Battery Calculation provides more accurate remaining capacity estimation by:
- ✅ Tracking actual energy consumption (Ah)
- ✅ Accounting for regenerative braking
- ✅ Avoiding voltage sag inaccuracies
- ✅ Providing better range estimates
- ✅ Customizable for any battery size

Configure your battery capacity and enable Smart Calculation for the best battery monitoring experience!

