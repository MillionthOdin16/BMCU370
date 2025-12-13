# Bill of Materials (BOM) - BMCU-C 370 Hall Version

## Overview

This document lists all components required to build a complete BMCU-C 370 Hall version (4-channel unit). Quantities are for a full 4-channel assembly.

**Important Notes:**
- Links provided are primarily Chinese vendors (Taobao/Tmall)
- International builders should search for equivalent parts locally
- Part specifications are more important than specific vendors
- Some 3D printed parts required (see separate STL files)

---

## PCB Boards

### Mainboard

**Specification:** BMCU-C universal mainboard
- **Quantity:** 1
- **PCB Type:** 4-layer recommended
- **Source:** [OSHWHub - BMCU](https://oshwhub.com/bamboo-shoot-xmcu-pcb-team/bmcu)
- **Notes:**
  - Multiple variants available (standard, Type-C, Xing-C modified)
  - Hall version compatible with all mainboard variants
  - Ensure proper component population (see assembly guide)

### Sub-boards (Channel Boards)

**Specification:** BMCU-C Hall sensor sub-board
- **Quantity:** 4 (one per channel)
- **PCB Type:** 2-layer acceptable
- **Critical:** Must be Hall sensor version, NOT photoelectric version
- **Components per board:**
  - 1× AS5600 Hall sensor (SOIC-8 or SSOP-8)
  - 2× WS2812B RGB LEDs (4020 side-mount package)
  - Resistors and capacitors (see schematic)
  - Connectors for motor and sensors

**Total PCB Cost:** ~80-120 RMB (depends on fabrication service and shipping)

---

## Electronics Components

### Mainboard Components

**CH32V203C8T6 Microcontroller**
- **Quantity:** 1
- **Package:** LQFP-48
- **Notes:** Pre-soldered if ordering assembled PCB

**TP75176E-SR RS-485 Transceiver**
- **Quantity:** 1
- **Package:** SOP-8
- **Notes:** Critical for BambuBus communication

**CH340C USB-to-Serial** (Type-C variants only)
- **Quantity:** 1 (optional, depends on PCB variant)
- **Package:** SOP-16

**Passive Components**
- Resistors, capacitors, LEDs (see schematic)
- Most come pre-populated with assembled PCB orders

**Connectors**
- PH2.0 connectors for sub-board connections
- 6-pin connector for printer cable
- USB Type-C connector (if applicable)

### Sub-board Components (per board × 4)

**AS5600 Hall Sensor**
- **Quantity:** 4 total (1 per channel)
- **Package:** SOIC-8 or SSOP-8
- **I2C Address:** 0x36 (fixed)
- **Notes:** Ensure genuine parts, counterfeits may not work reliably

**WS2812B RGB LED (Side-mount)**
- **Quantity:** 8 total (2 per channel)
- **Package:** 4020 (SMD, side-emitting)
- **Notes:** Orientation critical (check polarity marking)

**Diodes, Resistors, Capacitors**
- As per schematic
- Typically pre-populated on assembled PCBs

---

## Motors and Mechanical

### 370 DC Motors

**Specification:** 24V 6000RPM DC motor
- **Quantity:** 4
- **Shaft Diameter:** 2mm
- **Voltage:** 24V DC
- **No-load Speed:** ~6000 RPM
- **Notes:**
  - Much more powerful than 130/180 motors
  - More noise but better performance
  - Ensure shaft compatible with gears

### BMG Dual-Drive Gears

**Plastic Gears (Recommended)**
- **Quantity:** 4 sets (8 gears total)
- **Type:** BMG-style dual-drive
- **Material:** POM or nylon
- **Notes:**
  - Quieter than metal
  - Less filament chewing
  - Adequate for most filaments

**Optional: Metal Worm Gears**
- **Quantity:** 4 (if desired)
- **Material:** Hardened steel
- **Bore:** 1.95-2.0mm (interference fit)
- **Notes:**
  - Difficult to install (may need heating)
  - Reduces long-term wear
  - Increases metal shavings initially
  - Use plastic spur gears with metal worm

### Radial Magnets

**For AS5600 Sensors (Motor Shaft)**
- **Quantity:** 4
- **Size:** 6mm diameter × 2mm thickness
- **Magnetization:** Radial (NOT axial)
- **Mounting:** On motor shaft, ~1-2mm from AS5600
- **Notes:** Critical for Hall sensor operation

**For Buffer Sliders**
- **Quantity:** 4
- **Size:** 6mm diameter × 2-3mm thickness
- **Magnetization:** Radial
- **Mounting:** Embedded in slider piston
- **Notes:** Polarity must be correct for buffer detection

---

## Pneumatic Components

### PC4 Pneumatic Fittings

**Specification:** PC4-M5 or PC4-M6
- **Quantity:** 4-5 (recommend 1 extra)
- **Thread:** M5 or M6 (depending on printed parts)
- **Tube Size:** 4mm OD
- **Notes:**
  - For connecting PTFE tube to buffer
  - Consider through-bore type for dry box use

### PTFE Tubes

**Specification:** 4mm OD × 2mm ID
- **Quantity:** Variable (depends on printer distance)
- **Recommended:** 1-2 meters per channel
- **Notes:**
  - Standard Bowden tube
  - Low friction for smooth feeding

---

## Springs

### Buffer Springs (Main)

**Specification:** 0.5mm wire × 6mm OD × 10mm length
- **Quantity:** 4
- **Wire Diameter:** 0.5mm
- **Outer Diameter:** 6mm
- **Free Length:** 10mm
- **Notes:**
  - Primary buffer spring
  - A-series and P-series internal five-way version

**Alternative: Heavier Duty**
- **Specification:** 0.6mm wire × 4mm OD × 10mm length
- **Quantity:** 4
- **Use Case:** P-series or sealed dry box setups
- **Notes:** Provides more resistance for longer tube runs

**Alternative: Heavy Duty**
- **Specification:** 0.7mm or 0.8mm wire diameter
- **Use Case:** Long tube runs or high resistance scenarios
- **Notes:** May require testing to find optimal stiffness

### Auxiliary Springs (if needed)

- Small compression springs for various mechanisms
- Quantity and spec depend on specific build variant

---

## Fasteners and Hardware

### M2 Countersunk Self-Tapping Screws

**Size:** M2 × 8mm
- **Quantity:** 48-100 (recommend 100-pack)
- **Type:** Countersunk (flat head)
- **Material:** Steel, black oxide finish typical
- **Use:** Securing front covers, sub-boards

**Additional M2 × 8mm**
- **Quantity:** +3 for base assembly
- **Total recommended:** 100-pack covers everything

### M3 Countersunk Self-Tapping Screws

**Size:** M3 × 14mm
- **Quantity:** 9
- **Type:** Countersunk
- **Use:** Base assembly

### M3 Standard Screws and Nuts

**M3 × 10mm Screws**
- **Quantity:** 2
- **Type:** Standard (not self-tapping)
- **Use:** Bracket/support assembly

**M3 Hex Nuts**
- **Quantity:** 2
- **Material:** Steel
- **Use:** With M3 × 10mm screws

---

## Cables and Connectors

### MX3.0-4P Connector Cable

**Specification:** MX3.0 4-pin, mirror/reversed
- **Quantity:** 1
- **Type:** Male-to-male, pin order reversed
- **Length:** Varies (typically 10-15cm)
- **Use:** Mainboard to base connection
- **Notes:** "Mirror" means pin 1 connects to pin 4, etc.

### Sub-board Connection Cables

**Specification:** PH2.0 connectors, various pin counts
- **Quantity:** 4 sets
- **Lengths:**
  - Channels 3-4: 8cm recommended
  - Channels 1-2: 5cm recommended
  - Alternative: All 8cm (less neat but functional)
- **Notes:**
  - Pre-made cables or crimp your own
  - Longer cables easier for assembly but less tidy

### 6-Pin Printer Cable

**Specification:** For BambuBus connection
- **Quantity:** 1
- **Length:** Depends on printer distance (typically 1-2m)
- **Wires:** 6-conductor (A, B, GND, 24V, +2 spare)
- **Connector:** 6-pin JST or compatible
- **Notes:**
  - May come with printer or need to source separately
  - Ensure proper pin mapping

---

## Optical Components

### Fiber Optic Light Guide

**Specification:** 1.5mm diameter optical fiber
- **Quantity:** 1 meter (can cut to length)
- **Type:** PMMA or glass fiber
- **Use:** Top LED light guides for each channel
- **Alternative:** Transparent filament (works but less effective)
- **Notes:** Transmits light from WS2812B to visible location

---

## 3D Printed Parts

**Note:** STL files available in community repositories

### Per Channel (× 4 for full unit)

- Front cover
- Back housing
- Motor mount
- Buffer chamber
- Slider/piston
- Gear housing
- Cable management parts

### Base and Support

- Base plate
- Support brackets
- Cable guides
- Optional: Mounting brackets for printer

### Recommended Print Settings

- **Material:** PLA, PETG, or ABS
- **Layer Height:** 0.2mm
- **Infill:** 20-30%
- **Walls:** 3-4 perimeters
- **Support:** Required for some parts
- **Notes:**
  - PETG recommended for heat resistance
  - ABS for higher strength
  - PLA adequate for testing

---

## Estimated Costs

**Approximate costs in RMB (2025):**

| Category | Cost (RMB) |
|----------|-----------|
| PCBs (mainboard + 4 sub-boards) | 80-120 |
| Electronic components | 100-150 |
| Motors (4× 370) | 80-120 |
| Gears and mechanical | 40-60 |
| Fasteners and hardware | 20-30 |
| Cables and connectors | 30-50 |
| 3D printing filament | 20-50 |
| **Total** | **~370-580 RMB** |

**USD Equivalent:** ~$52-$82 (approximate)

**Notes:**
- Costs vary significantly by region and vendor
- Buying in bulk or with friends reduces cost
- PCB assembly services add cost but save time
- International shipping may add significant cost

---

## Sourcing Notes

### For Chinese Builders

Most parts available from:
- **Taobao/Tmall:** General parts, cables, fasteners
- **LCSC/JLC:** Electronic components, PCBs
- **Local electronics markets:** Often cheaper for bulk

### For International Builders

**Equivalent Parts:**
- **Motors:** Search "24V DC motor 6000RPM 2mm shaft"
- **AS5600:** Available from Digi-Key, Mouser, AliExpress
- **WS2812B:** NeoPixel compatible, Adafruit/clones
- **BMG Gears:** 3D printer extruder gear sets
- **Magnets:** "6mm radial magnet 2mm" on eBay/Amazon

**PCB Fabrication:**
- **JLCPCB:** International service
- **PCBWay:** Alternative service
- **OSH Park:** USA-based (more expensive)

---

## Assembly Order Recommendations

1. **Order PCBs** first (longest lead time, 1-2 weeks)
2. **Source electronics** while waiting for PCBs
3. **3D print parts** (can do in parallel)
4. **Gather hardware** (fasteners, cables)
5. **Assemble and test** one channel first
6. **Scale to four channels** after validation

---

## Optional Upgrades

### Enhanced Components

- **Better motors:** Higher quality 370 motors for less noise
- **Premium gears:** CNC-machined metal gears
- **Shielded cables:** Reduce electrical interference
- **Ferrite beads:** On power and signal lines

### Accessories

- **Dry box:** For humidity-sensitive filaments
- **Better PTFE tubes:** Low-friction Capricorn-style
- **Quick-disconnect fittings:** Easier filament changes
- **LED diffusers:** Smoother light appearance

---

## Where to Get STL Files

**3D Printable Parts:**
- Community wiki: https://bmcu.wanzii.cn/
- OSHWHub: https://oshwhub.com/bamboo-shoot-xmcu-pcb-team/bmcu
- Community file shares (see wiki for links)

---

## Schematic and PCB Files

**Electronic Design:**
- **Hardware Design:** https://oshwhub.com/bamboo-shoot-xmcu-pcb-team/bmcu
- **Gerber Files:** Available for PCB fabrication
- **BOM Export:** Can be generated from EDA files

---

## Important Warnings

⚠️ **Quality Matters:**
- Counterfeit AS5600 sensors may not work
- Poor quality motors may have high noise or fail
- Cheap magnets may have weak fields

⚠️ **Check Compatibility:**
- Ensure Hall sensor sub-board, not photoelectric
- Verify PCB variant matches your needs
- Confirm connector types match

⚠️ **Order Extras:**
- LEDs (easy to damage during soldering)
- Screws (easy to lose)
- PCBs (in case of damage)

---

## Resources for Purchasing

**Component Search:**
- **LCSC:** https://www.lcsc.com/
- **Digi-Key:** https://www.digikey.com/
- **Mouser:** https://www.mouser.com/
- **AliExpress:** https://www.aliexpress.com/

**3D Printing Services:**
- Local makerspaces
- Online services (Shapeways, Craftcloud)
- Community members may offer prints

---

**Last Updated:** 2025-12-12
**Version:** BMCU-C 370 Hall V0.1-0020

**Note:** This BOM is community-maintained. Please verify specifications against current hardware designs and report any discrepancies.
