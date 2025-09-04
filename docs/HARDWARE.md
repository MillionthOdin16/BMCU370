# Hardware Configuration Guide

This guide provides detailed information about the hardware setup and configuration for the BMCU370 system.

## Overview

The BMCU370 is built around the CH32V203C8T6 microcontroller, a RISC-V based MCU designed for embedded applications.

## Microcontroller Specifications

- **CPU**: RISC-V 32-bit core @ 144MHz
- **Flash Memory**: 64KB
- **RAM**: 20KB
- **GPIO Pins**: 37 available
- **Timers**: 4x 16-bit, 2x watchdog
- **Communication**: 3x USART, 2x I2C, 2x SPI
- **ADC**: 12-bit, 10 channels with DMA support

## Pin Assignment

### RGB LED Outputs
```
PA11 - Channel 0 RGB LEDs (Neopixel)
PA8  - Channel 1 RGB LEDs (Neopixel)
PB1  - Channel 2 RGB LEDs (Neopixel)
PB0  - Channel 3 RGB LEDs (Neopixel)
PD1  - Main Board Status RGB LED
```

### AS5600 Hall Sensors (I2C)
```
Channel 0: SCL=PB15, SDA=PD0
Channel 1: SCL=PB14, SDA=PC15
Channel 2: SCL=PB13, SDA=PC14
Channel 3: SCL=PB12, SDA=PC13
```

### ADC Inputs
```
ADC Channel 0: Channel 3 Pull Sensor
ADC Channel 1: Channel 3 Online Key Sensor
ADC Channel 2: Channel 2 Pull Sensor
ADC Channel 3: Channel 2 Online Key Sensor
ADC Channel 4: Channel 1 Pull Sensor
ADC Channel 5: Channel 1 Online Key Sensor
ADC Channel 6: Channel 0 Pull Sensor
ADC Channel 7: Channel 0 Online Key Sensor
```

### Motor Control (PWM)
Motor control pins are configured in the Motion_control module. Each channel has dedicated PWM outputs for forward/reverse operation.

### Communication
```
USART1: BambuBus Protocol Communication
USART2: Debug/Programming (115200 baud)
```

## Component Specifications

### AS5600 Hall Sensors

**Purpose**: Rotary position sensing for filament movement tracking

**Specifications**:
- 12-bit resolution (4096 positions per revolution)
- Contactless magnetic sensing
- I2C interface (address 0x36)
- 3.3V or 5V operation
- Response time: <1ms

**Installation**:
- Mount sensor PCB near rotating magnet
- Magnet should be diametrically magnetized
- 0.5-3mm air gap recommended
- Ensure magnet is centered over sensor
- **CRITICAL: Magnet polarity orientation must be consistent across all channels**
  - Install all magnets with the same pole (North or South) facing the sensor
  - Inconsistent magnet polarity will cause motor direction detection errors
  - Mark magnets during assembly to ensure consistent orientation

### Pressure Sensors

**Purpose**: Filament insertion/removal detection

**Type**: Analog pressure sensors or micro-switches
**Interface**: ADC input (0-3.3V)
**Thresholds**:
- High pressure (filament inserted): >1.85V
- Low pressure (filament removed): <1.45V
- Hysteresis prevents false triggering

### RGB LEDs (NeoPixel Compatible)

**Purpose**: Visual status indication

**Specifications**:
- WS2812B or compatible addressable RGB LEDs
- 5V power supply required
- Data transmission: 800kHz
- 24-bit color depth (8 bits per channel)

**LED Configuration**:
- 2 LEDs per filament channel (configurable)
- 1 LED for main board status
- Total: 9 LEDs default configuration

## Power Requirements

### Supply Voltage
- **MCU Core**: 3.3V (internal regulator from 5V)
- **RGB LEDs**: 5V (high current capability required)
- **Sensors**: 3.3V (provided by MCU regulator)
- **Motors**: Variable (typically 12V-24V)

### Current Consumption
- **MCU**: ~50mA @ 144MHz
- **RGB LEDs**: ~20mA per LED per color channel (max 60mA per LED)
- **Sensors**: ~10mA total for all AS5600 sensors
- **Motors**: Variable depending on load

### Power Supply Recommendations
- **5V Rail**: Minimum 2A capacity for RGB LEDs
- **Motor Supply**: Appropriate for stepper motors (typically 1-2A per channel)
- **Clean Power**: Use bypass capacitors near MCU and sensors

## PCB Layout Considerations

### Signal Integrity
- Keep crystal oscillator traces short
- Use ground plane under high-speed digital signals
- Separate analog and digital ground sections

### Power Distribution
- Dedicate wide traces for power rails
- Place bypass capacitors close to IC power pins
- Use appropriate trace width for current capacity

### EMI/EMC
- Keep switching circuits away from analog sensors
- Use ferrite beads on motor power lines if needed
- Shield sensitive analog circuits

## Mechanical Mounting

### AS5600 Sensor Mounting
- Sensors must be precisely aligned with rotating magnets
- Use mounting brackets to maintain consistent air gap
- Protect sensors from contamination and physical damage
- **Ensure consistent magnet polarity orientation across all channels**

### Magnet Assembly Guidelines
**Critical for Motor Direction Detection:**

1. **Polarity Marking**: Mark all magnets during manufacturing to indicate pole orientation
2. **Consistent Installation**: Install all magnets with the same pole (North or South) facing the AS5600 sensor
3. **Verification Procedure**: Use a compass or magnetic field detector to verify polarity before final assembly
4. **Quality Control**: Include magnet polarity check in assembly verification steps

**Magnet Specifications:**
- Diametrically magnetized (not axially magnetized)
- Neodymium N35 or stronger recommended  
- Diameter appropriate for gear assembly
- Thickness 2-4mm for optimal field strength

**Assembly Notes:**
- Random magnet orientation will cause motor direction detection errors
- Inconsistent polarity is a primary cause of channels 1, 2, and 3 direction reversal
- Proper magnet installation reduces need for software direction corrections

### LED Placement
- Channel LEDs should be visible from front panel
- Main board LED indicates overall system status
- Consider light diffusion for even illumination

### Heat Management
- Ensure adequate ventilation around motor drivers
- Consider heat sinks for high-power components
- Monitor component temperatures during operation

## Configuration Options

### Hardware Variants

The firmware supports multiple hardware configurations through `config.h`:

#### LED Count Configuration
```c
#define LED_PA11_NUM    2    // Channel 0 LED count
#define LED_PA8_NUM     2    // Channel 1 LED count  
#define LED_PB1_NUM     2    // Channel 2 LED count
#define LED_PB0_NUM     2    // Channel 3 LED count
#define LED_PD1_NUM     1    // Main board LED count
```

#### Sensor Thresholds
```c
#define PULL_VOLTAGE_HIGH    1.85f   // High pressure threshold
#define PULL_VOLTAGE_LOW     1.45f   // Low pressure threshold
```

#### Motion Control Parameters
```c
#define ASSIST_SEND_TIME_MS     1200    // Feed assist duration
#define P1X_OUT_FILAMENT_MM     200.0f  // Retraction distance
```

## Testing and Validation

### Initial Hardware Test
1. **Power-on Test**: Verify all voltage rails
2. **LED Test**: Check each RGB LED individually
3. **Sensor Test**: Verify AS5600 sensor communication
4. **ADC Test**: Check pressure sensor readings
5. **Communication Test**: Verify UART communication

### Calibration Procedures

#### Pressure Sensor Calibration
1. With no filament inserted, record "low" voltage
2. With filament fully inserted, record "high" voltage
3. Set thresholds with appropriate hysteresis
4. Test insertion/removal detection

#### Hall Sensor Calibration
1. Verify sensor communication on each I2C channel
2. Check magnet alignment and air gap
3. **Verify magnet polarity consistency across all channels**
4. Validate position readings through full rotation
5. Test movement tracking accuracy
6. **Confirm direction detection consistency between channels**

### Troubleshooting Common Issues

#### LEDs Not Working
- Check 5V power supply voltage and current capacity
- Verify data line connections and signal integrity
- Test individual LED segments
- Check for correct LED type (WS2812B compatible)

#### Sensor Communication Errors
- Verify I2C pullup resistors (4.7kΩ typical)
- Check for address conflicts
- Test with oscilloscope or logic analyzer
- Ensure proper power supply to sensors

#### Motor Control Issues
- Check PWM signal generation
- Verify motor driver power supply
- Test motor connections and phases
- Check for overheating or overcurrent

### Performance Optimization

#### Real-time Performance
- Monitor main loop execution time
- Optimize sensor reading frequency
- Use DMA for ADC to reduce CPU load
- Consider interrupt-driven communication

#### Power Efficiency
- Reduce RGB LED brightness if thermal issues occur
- Implement sleep modes when idle (future enhancement)
- Optimize PWM frequencies for motor efficiency

## Safety Considerations

### Electrical Safety
- Use appropriate fuses for motor power supplies
- Implement overcurrent protection
- Ensure proper grounding of all components
- Use isolated power supplies where required

### Thermal Protection
- Monitor component temperatures
- Implement thermal shutdown for motor drivers
- Provide adequate cooling for high-power components

### Mechanical Safety
- Ensure proper guarding of moving parts
- Use appropriate torque limits for motors
- Implement position limits to prevent damage

## Future Hardware Enhancements

### Possible Improvements
- Add temperature sensors for thermal monitoring
- Implement current sensing for motor load monitoring
- Add Ethernet connectivity for remote monitoring
- Include buzzer for audio feedback
- Add SD card slot for configuration storage

### Expansion Capabilities
- Additional ADC channels available for more sensors
- Unused GPIO pins available for future features
- SPI interface available for high-speed peripherals
- Additional UART available for secondary communication