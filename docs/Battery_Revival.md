# BionX 3562 Battery Revival — Research Notes

**Recorded:** 2026-10-25  
**Status:** Research only. None of these steps has been performed on this pack yet.  
**Applies to:** BionX part `3562`, serial `C11090387`, 48 V nominal, 13S4P Li-ion, rear-rack pack with internal BMS and charge electronics (see [Confirmed_Discoveries.md](Confirmed_Discoveries.md)).

> **Safety gate:** The dock contacts carry pack voltage when the BMS is awake. Docking a live pack has been reported to spark hard. Meter A1 to A2 before touching anything. Never attempt to recover a cell group below 2.0 V.

## Why this matters to the project

A live original BMS is the fastest route to a working baseline bike and a stock CAN bus to sniff. That capture is the spec for the ESP32-C3 battery emulation. Reviving the pack is therefore step zero for path 3 (generic 13S pack plus translator), not an alternative to it.

## Facts established

| Fact | Source |
|---|---|
| 48 V BionX packs enter **Deep Sleep** after 2 months idle, or after 1 week below 10% state of charge. | BX Legacy technical support |
| Wake from Deep Sleep: connect the pack to the 26 V BionX power supply (`01-3635` or `01-4576`). | BX Legacy technical support |
| There is no reset button and no console button combination that clears a battery fault. | Endless Sphere thread 75347 |
| Charging LED: red → orange → green indicates BMS loading and charging. Flashing red is associated with deep discharge. | ES 75347, ES 120633 |
| BMS refuses to charge below a pack-level floor, reported between ~24 V and ~35 V. No BionX document for the exact figure. | ES 75347, ES 120633 |
| Without battery CAN frames the motor system shuts off roughly every 5 minutes. Bypassing the BMS loses regen, fuel gauge and auto shutdown. | ES 75347 |
| Console diagnostic error **26** = battery communication fault. | ES 75347 |
| BionX Bike Interface (BBI) dealer software can reprogram and clear faults. It is dealer-licensed. BX Legacy references it. | BX Legacy, ES 75347 |
| BionXtool reads battery registers over CAN once the pack is awake. It does not unlock a dead BMS. | BionXtool README |

## Revival ladder, cheapest first

### 1. Deep Sleep wake
- Plug the `01-3635` charger into the rack pack. Leave it several hours.
- Watch the LED. Red → orange → green means the BMS is alive and charging. Go to step 2 to confirm.
- Flashing red means deep discharge lockout. Go to step 3.
- No LED at all after several hours: go to step 3, but suspect a dead BMS board.

### 2. Paperclip wake test
Source: German BionX workshop manual, as quoted on ES 75347. Only valid on a pack with charge in it.
- Short dock pin **A1** to pin **3** with a paperclip.
- A CAN pack should beep **3** times (an I2C pack beeps 4).
- The pack switches off when the clip is removed, or after about 15 minutes.
- Three beeps means the BMS is alive. The pack is recellable with the stock electronics if cells are tired.

### 3. Deep discharge lockout recovery
Worked for one ES user: raise pack voltage with an external CV/CC supply at low current, then finish on the BionX charger.
- Open the case (teardown posts exist on ES).
- Measure each of the 13 groups at the balance leads.
- Any group at **0 V** is a dead cell group (Samsung cell internal safety disconnect), not a BMS fault. That group gets four new cells. Do not attempt to reset a tripped cell.
- Any group below **2.0 V**: do not recover. Replace.
- For groups between 2.0 V and 3.0 V: charge through the cell terminals, bypassing the BMS, with a bench supply at ~0.5 A per group until every group reads above 3.0 V.
- Close up and return to step 1.

### 4. Dealer software
- BBI can clear a locked BMS. Dealer-licensed. Ask BX Legacy whether they will run it on a shipped pack and at what price.
- Only worth it if steps 1 to 3 show a healthy pack that still refuses to wake.

### Between steps 1 and 3: read the BMS over CAN
If step 1 or 2 shows any sign of life, tap CAN (battery pins 2/5/GND) and read the alert flags, charger status, DC-in voltage, ship-mode flag and all 13 cell voltages before opening the case. Register list and procedure in [BMS_Protocol.md](BMS_Protocol.md). This separates a sleeping pack, a deep-discharged pack and a failed board without a screwdriver.

### If all four fail
The BMS board has failed. Proceed to path 3: generic 13S pack with the ESP32-C3. Correction to the earlier working assumption: **the motor does not require a battery node on the bus.** The Minimal Translator runs the motor with no battery emulation, talking only to node `0x20`, and supplies the motor's 12 V and 5 V rails from step-down converters. The "shuts off every 5 minutes" report came from a setup that kept the stock console, whose inactivity timer lives in the battery. With the console replaced, that timer goes away. Details in BMS_Protocol.md.
- The ~5.5 V and ~12 V rails the bypasser fed are the vControl and +5 V rails the pack normally supplies to the motor and console. Confirm by measuring the pack connector once awake (`0xAC/0xAD`, `0x4E/0x4F`).

## Pack-specific checklist

- [ ] Meter A1 to A2 on the dock. Record voltage.
- [~] Step 1: charger connected. **Observed 2026-10-25 ~14:30:** LED ring at the charge port **steady red**, no flashing over a 3.7 s clip. Pack emits a short buzz about once every few minutes: ~1 s long, two bursts, tonal content around 2.4 to 2.6 kHz with mains-rate amplitude modulation, consistent with the on-board charger's switching stage or relay engaging and dropping out. Reading: BMS is awake and in charge mode (`0x13` = 2), not locked. Intermittent cycling rather than continuous charge suggests the charger is starting, hitting a limit (likely one low group or precharge), pausing, retrying. Still to record: time to green, or whether it never reaches green.
- [ ] Step 2: paperclip test. Record beep count.
- [ ] If locked: open case, record all 13 group voltages.
- [ ] Record which step, if any, revived the pack.
- [ ] Once awake: capture stock CAN traffic before any further changes.

## Sources

1. BX Legacy technical support — Deep Sleep behavior, 26 V wake method, BBI references.  
   https://www.bx-legacy.com/pages/technical-support-information
2. Endless Sphere thread 75347 — "BionX 48V battery not responding but cells are ok": paperclip test, LED behavior, bypass experiments, error 26, 5-minute shutdown.  
   https://endless-sphere.com/sphere/threads/bionx-48v-battery-not-responding-but-cells-are-ok.75347/
3. Endless Sphere thread 120633 — "BionX 48V revival": lockout floor, 0 V group diagnosis, recell with ICR18650-26D.  
   https://endless-sphere.com/sphere/threads/bionx-48v-revival.120633/
4. BionXtool — CAN diagnostics once the pack is awake.  
   https://github.com/buxtronix/BionXtool
