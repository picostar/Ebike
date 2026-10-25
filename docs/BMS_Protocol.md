# BionX BMS CAN Protocol — What Is Known for Waking and Unlocking the 3562

**Recorded:** 2026-10-25  
**Status:** Extracted from published sources. Nothing here has been tried on this pack yet.  
**Primary sources:** BionXtool `PROTOCOL.md` and `registers.h` (buxtronix), BionXtool `bike-on-off.pcap` capture, BionX Minimal Translator `main.c` (stancecoke).

## Bottom line

1. There is **no documented "clear lockout" register**. The BMS wakes from Deep Sleep on **external DC on the charge input**, not on a CAN command. If the charger does not wake it, CAN cannot either, because the BMS CAN node is asleep.
2. Once the BMS is awake, **every diagnostic needed to tell a dead BMS from a dead pack is readable over CAN** with a $30 CAN adapter: 16 alert flags with console error codes, charger-manager state, DC-in voltage, all 13 cell voltages, reset and watchdog counters.
3. Deep Sleep is triggered by **inactivity (60 days), low SoC (7 days under 10%), or average cell voltage under 2.54 V**. The low-voltage path is the deep-discharge lockout people describe. Recovery is raising the cells externally, then presenting the charger. That matches the field reports in [Battery_Revival.md](Battery_Revival.md).
4. **The motor does not need the battery on the bus.** The Minimal Translator runs a BionX CAN motor from a generic pack with the battery node absent. It talks only to node `0x20`. It supplies the motor's 12 V and 5 V rails from step-down converters. This changes the scope of the ESP32 work: no battery emulation is required unless the stock console is kept.

## Bus basics

- CAN 2.0A, 11-bit IDs, **125 kbit/s**.
- Console is bus master. Battery and motor never initiate.
- Node IDs: console `0x08` (slave `0x48`), battery `0x10` (slave `0x50`), motor `0x20` (slave `0x60`), BIB diagnostic box `0x58`.
- Query: ID = target node, DLC 2, data `[0x00, reg]`.
- Reply: ID = requester (`0x08`), DLC 4, data `[0x00, reg, 0x00, value]`.
- Set: ID = target node, DLC 4, data `[0x00, reg, 0x00, value]`.
- Physical tap: battery connector pins **2 = CAN_H, 5 = CAN_L**, plus GND. About 50 V is present on adjacent pins. Or an inline tap at the Hirose console connector (HR30-6J-6P / HR30-6P-6S).

## Stock power-on sequence to the battery (from the pcap)

```
SET  0x10  0x20 = 0      alarm enable off
SET  0x10  0x21 = 1      POWER_VOLTAGE_ENABLE: turn on vPower rail to motor
QRY  0x10  0x3B          hw rev   (capture: 0x63)
QRY  0x10  0x3C          sw rev   (capture: 0x112 = firmware 112)
SET  0x10  0x22 = 0      accessory off
QRY  0x10  0x3D          battery type (capture: 3)
QRY  0x10  0xF9/0xFA     max charge current  (capture: 0x2710 = 10.000 A)
QRY  0x10  0xFB/0xFC     max discharge current (capture: 0xA2C6 = 41.670 A)
```
The console then mirrors those limits into the motor after `SET 0x20 0xA5 = 0xAA` (motor unlock). Steady state polls battery `0x32`, `0xAA`, `0x1E/0x1F`, `0xE1/0xE2`, `0xA1-0xA4`, `0x30`, `0x61` at low rate. Shutdown is `SET 0x10 0x25 = 1`.

The capture shows the battery already answering before the console sends anything that could be called a wake. The BMS is awake because the console button pressed vControl, which the BMS supplies. A BMS in Deep Sleep does not respond to this sequence.

## Battery registers relevant to wake, lockout and diagnosis

All on node `0x10`. Protected writes need `0x71` first.

| Reg | Name | Notes |
|---|---|---|
| `0x13` | CHARGER_MANAGER_STATUS | 0 Off, 1 Standby, 2 Charger, 3 Accessory, 4 Vdcin sense, 5 Overtemp, 6 Charge done, 7 Buck failed. **First thing to read with the charger plugged in.** |
| `0x14` | CONFIG_WAKE_ON_POWERVOLTAGE | Wake automatically when voltage appears on vPower. 0 disables. Protected. Setting this to 1 would make the pack wake when the motor side is energized, worth considering once awake. |
| `0x1D` / `0xF0` | STATUS_FLAGS_HI / LO | 16 alert bits with console codes: 0 Vctrl (20), 1 Precharge (21/67), 2 Relay (22), 3 BMS (23), 4 DCDC (28), 6 gas-gauge temp, 7 pack temp, 8 balancer overvolt (62), 9 balancer undervolt (61), 10 pack problem (63), 11 accessory overcurrent (60), 12 electronic fuse (66), 13 balancer plug not connected, 14 +5 V short (latched). **Second thing to read.** |
| `0x21` | CONFIG_POWER_VOLTAGE_ENABLE | 1 turns on the high-voltage rail to the motor. |
| `0x25` | CONFIG_SHUTDOWN | 1 shuts the system down, BMS enters low-power sleep. |
| `0x26` | CONFIG_CONTROL_VOLTAGE_ENABLE | vControl rail (the ~12 V the motor's logic runs on). |
| `0x31` | STATUS_CHARGE_PLUG | Charger plugged in. |
| `0x37` | CONFIG_SHIPMODE | Write `0xAA` then value. 1 = ship mode: only external power can wake it on next shutdown. **Check this is 0.** A pack left in ship mode behaves exactly like a locked BMS until the charger is connected. |
| `0x3B` / `0x3C` | REV_HW / REV_SW | Firmware 112 and earlier work only with the G2 console; 115 is hybrid (BX Legacy). |
| `0x46` | CONFIG_DIAG | Diagnostic mode. Function undocumented. |
| `0x47` | CONFIG_FORCE_DONE | Write `0x45` to force SoC to 100%. Useful after a recell so the gauge does not report empty. Protected. |
| `0x48` / `0x49` | STATISTIC_RESETS | Reset counter. |
| `0x4A` / `0x4B` | INTERNAL_BATTERY_VOLTAGE | vBattInternal, 0.001 V. |
| `0x4C` / `0x4D` | CONSOLE_VOLTAGE | Voltage applied to console, 0.001 V. |
| `0x4E` / `0x4F` | 12V_VOLTAGE | Internal 12 V rail, 0.001 V. |
| `0x50` | CONFIG_NOMINAL_BATTERY_VOLTAGE | Pack nominal. Expect 48. |
| `0x51`-`0x57` | TIMER_* | Power-off, accessory-off, precharge, inactivity shutdown timers in seconds. The inactivity timer is the "5 minute shutdown" bypassers saw. |
| `0x6C` / `0x6D` / `0x6E` | CELLMON_CHANNEL_ADDRESS / DATA | Write cell index to `0x6C`, read 16-bit voltage from `0x6D`/`0x6E`. **All 13 cell voltages without opening the case.** |
| `0x71` | PROTECT_UNLOCK | Value = seconds to stay unlocked. **Do not send above 0x54**; higher values are firmware magic. |
| `0x7D` | POWERON_RESET_COUNT | Hard resets of the main MCU. |
| `0x85` / `0x86` | BRIDGE_CHARGER_ADDR / DATA | Gateway to the on-board charger's own registers (mode `0x10`: 0 low power, 1 idle, 2 charging, 3 accessory, 5 calibration, 99 fault; final voltage `0x12/0x13`; current `0x14/0x15`). Protected. |
| `0x9A` / `0x9B` | DCIN_VOLTAGE | External supply voltage seen by the pack, 0.001 V. Confirms the 26 V charger is reaching the BMS. |
| `0x9F` | WATCHDOG_RESET_COUNT | |
| `0xA6` / `0xA7` | BATTERY_VOLTAGE | vBatt, 0.001 V. |
| `0xAA` / `0xAB` | POWER_VOLTAGE | vPower actually applied to the motor. |
| `0xAC` / `0xAD` | CONTROL_VOLTAGE | vControl applied to the motor. |
| `0xAE` / `0xAF` | PACK_SERIAL / PACK_PARALLEL | Cell count. Expect 13 / 4. |
| `0xF9`-`0xFC` | MAX_CHARGE / MAX_DISCHARGE | Current limits the console mirrors into the motor. |

Deep Sleep configuration and counters live in the battery's I2C/EEPROM space, reached through a bridge, not as plain CAN registers: `0xA05B` inactivity delay (default 60 days), `0xA05C` low-SoC delay (default 7 days), `0xA05D`-`0xA060` and `0xA08F`-`0xA090` entry counters, the last one being "entered deep sleep due to average voltage under 2.54 V/cell". How BionXtool or BBI reaches that space is not documented in the public repo.

## Diagnostic procedure once the BMS answers

Requires: USB CAN adapter with Candlelight firmware (CANable 2.0 or similar), Linux with SocketCAN, BionXtool built from source, tap on pins 2/5/GND.

```
ip link set can0 up type can bitrate 125000
./BionXtool -R battery 0x13     # charger manager status
./BionXtool -R battery 0x1D     # flags hi
./BionXtool -R battery 0xF0     # flags lo
./BionXtool -R battery 0x9A     # dc-in voltage hi
./BionXtool -R battery 0x37     # ship mode, want 0
./BionXtool -s                  # full status dump incl. 13 cell voltages
```

Interpretation:
- Flags clean, cells all above 3.0 V, charger status 2 or 6: BMS is fine, pack was only asleep.
- Flags clean, one or more cells low, charger status 1 or 4: deep-discharge path. Raise cells externally per Battery_Revival step 3, then retry.
- Bit 9 (balancer undervolt, code 61) or bit 10 (pack problem, code 63) set: cell group fault. Recell that group.
- Bit 3 (BMS, code 23), bit 2 (relay, code 22) or bit 12 (electronic fuse, code 66) set: board fault. BBI or a replacement BMS.
- Bit 14 (+5 V short, latched): a latched fault that a plain power cycle may not clear. Candidate for BBI.

## What this means for the ESP32-C3 controller

The Minimal Translator proves the motor runs with only these on the bus: unlock `0xA5 = 0xAA`, gauge gain write `0x6D`-equivalent, then a loop of `QRY 0x21` torque, `SET 0x09` assist level, and status reads for speed `0x11`, power `0x14`, temperature `0x16`, power voltage `0xAA`-equivalent on the motor node. It reads battery voltage **from the motor**, not from a battery node. It feeds the motor 12 V and 5 V from DC-DC converters on the generic pack.

So the ESP32 project has two tiers:
- **Tier 1 (no stock console):** port the Minimal Translator loop to ESP32-C3 TWAI. No battery emulation. The original pack, revived or not, becomes optional.
- **Tier 2 (keep stock console, generic pack):** emulate node `0x10` well enough to answer the console's power-on queries (`0x3B`, `0x3C`, `0x3D`, `0xF9`-`0xFC`) and steady-state polls (`0x32`, `0xAA`, `0x1E/1F`, `0x30`, `0x61`), and keep the inactivity timer from firing. The pcap in BionXtool is the spec.

Tier 1 is the smaller project and is what the repo is scoped for.

## Sources

1. BionXtool PROTOCOL.md, registers.h, bike-on-off.pcap — https://github.com/buxtronix/BionXtool
2. BionX Minimal Translator main.c — https://github.com/stancecoke/BionX_Minimal_Translator
3. BX Legacy technical support (Deep Sleep behavior, firmware 112/115) — https://www.bx-legacy.com/pages/technical-support-information
4. Endless Sphere thread 75347 (field bypass reports) — https://endless-sphere.com/sphere/threads/bionx-48v-battery-not-responding-but-cells-are-ok.75347/
