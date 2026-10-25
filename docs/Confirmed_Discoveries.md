# BionX G1 Resto-Mod — Confirmed Discoveries

**Discovery record:** 2026-10-25  
**Status:** Identification and architecture research; **not yet electrically tested**  
**Project objective:** Retain the original BionX rear hub motor **without opening or modifying it**; replace the obsolete battery, handlebar throttle, and display; preserve original motor connectors by building a removable adapter harness.

> **Safety gate:** Do not attach a replacement battery, cut the motor harness, assume a connector pinout, or energize the motor until its rated input-voltage range, power/control wiring, ground reference, and startup requirements have been checked.

## 1. Confirmed hardware identification

| Item | Evidence | Finding | Confidence |
|---|---|---|---|
| Rear hub motor | Photo of label on cable exiting hub | **BionX part no. `3554`, serial `A11157034`** | Confirmed by photograph |
| Motor communication protocol | BionX dealer manual serial-number compatibility table | **CAN bus**: motor part `3554` falls in the documented CAN-motor part-number range `3350–4848`; `3554` is also explicitly listed as a CAN motor | Confirmed by documentation |
| Approximate manufacture date | Motor serial and BionX dealer manual's serial format | `A` = assembly-location field, `11` = **2011**, `157` = Julian day **157 (June 6, 2011)**, `034` = sequence; date is decoded, not a separate printed rating | High confidence, decoded |
| Existing charger | Photo of charger label | **BionX `01-3635`, model `SA190B-24U`; output `26 V DC`, `3.45 A`, `89.7 W`; input `100–240 V AC`** | Confirmed by photograph |
| Charger's intended battery family | BX Legacy listing for BionX `01-3635` | This **26 V power supply** is sold specifically for **BionX 48 V battery systems**. Its output rating does **not** mean the traction pack is 26 V. | Confirmed for charger model, not pack identity |
| Battery and mounting | Photographs | White **rear-rack** battery enclosure and battery-rail/docking assembly; rail barcode label only partially legible (starts `01-357…`) | Confirmed physical configuration; partial ID |
| Battery pack identity | Pack label read 2026-10-25 | **BionX part no. `3562`, serial `C11090387`**. Independent cell-replacement shops (akkuplus.de, akku-mainz.de) list `3562` with `4073` and `3760` as the **48 V Li-ion** family, built from **52 × 18650 cells = 13S4P**, and pair it with the SL 250 generation. | Confirmed by label plus two third-party listings |
| Battery manufacture date | Pack serial, same format as motor serial | `C` = assembly-location field, `11` = **2011**, `090` = Julian day **90 (March 31, 2011)**, `387` = sequence. Ten weeks before the motor; motor and pack are a factory pairing. | High confidence, decoded |
| Existing controls | Photographs | Original BionX **G1-style handlebar console** and thumb-operated control visible | Confirmed by photograph |
| Motor connections | Photographs | Motor axle harness has **separate heavy-current power and miniature multipin communication connections** | Confirmed visually; exact wire assignment unverified |

**Important correction to the early working assumption:** The appearance of a G1-style console does **not** establish I²C communication. This particular motor's `3554` part number places it in the **CAN-bus** family. Consequently the project should not be designed as an I²C retrofit.

## 2. Battery voltage: what is and is not established

- **Confirmed:** The original pack is BionX part `3562`, a **48 V nominal, 13S4P Li-ion** rack battery (see Section 1). Fully charged it sat near **54.6 V**, so the `3554` motor has run at 13S voltage from the factory.
- **Confirmed:** The charger marked `01-3635` outputs **26 V DC** and is specified by BX Legacy for BionX **48 V battery systems**. The `3562` pack carries its own charge electronics inside the case, which is why the external supply is 26 V.
- **Not confirmed:** The `3554` controller's behavior with a **generic 13S pack that lacks the BionX BMS and CAN battery traffic**; whether the motor needs the pack's wake/enable or CAN startup messages to run; the present condition of the original pack's electronics.
- **Do not conclude:** That a generic 48 V pack is a drop-in. The voltage matches, the protocol handshake is the open question. The BionX Minimal Translator project exists because the motor expects battery-side CAN traffic.
- **Candidate replacement:** A conventional nominal-48-V (13S) pack now matches the original pack's voltage window. It remains a *candidate design* until the CAN startup requirement is understood.
- **Charging:** The original 26 V BionX charger must **not** be wired directly to a conventional 13S battery/BMS. A generic battery requires a charger suitable for its chemistry and series-cell count.

## 3. Confirmed connector families and sourcing

**Preferred method: preserve the original motor's connectors.** Buy a mating BionX extension cable and modify the **extension**, not the hub-motor pigtail. Verify that the supplied mating power plug matches the photographed motor before purchase or installation.

| Product / approach | Supplier | Observed listing on 2026-10-25 | Source |
|---|---|---|---|
| **300 mm motor extension, classic power connector**, includes separate power and communication extensions | BX Legacy | **$49.99** listed (seller currency to confirm) | https://www.bx-legacy.com/collections/battery-harness/products/motorextension-powerandcommunicationcables-300mm113-4-classicpowerconnector |
| **100 mm communication-only extension**, Hirose | BX Legacy | **$29.99** listed (seller currency to confirm) | https://www.bx-legacy.com/collections/battery-harness/products/extcommunicationcable100mm |
| Original-type **Hirose HR30 six-contact CAN/console connector family** | Hirose / electronics distributors | BionXtool identifies **`HR30-6J-6P` and `HR30-6P-6S`** as a mating pair for its inline CAN tap. Confirm exact housing/gender/contact variant against physical connector. | https://github.com/buxtronix/BionXtool/blob/master/README.md |

**Connector caveats:**

1. The Hirose is a **signal/low-current** connector; **never** route traction-battery motor current through its pins.
2. The **classic BionX power connector** is distinct from the later **new/gold power connector**. Select the classic variant only after matching your existing plug.
3. No wire colors, pin numbering, pinout, or supply voltage have yet been verified **on this individual motor harness**. Source documentation can guide a continuity test but is not a substitute for it.
4. The socket visible on the rear-rack battery case is associated with charging; do not confuse it with the motor communication connector.

## 4. Chosen retrofit direction

**Decision made:** **Keep the BionX 3554 motor and its internal motor-control electronics as-is.** Replace the battery, throttle, and display using reversible external electronics and wiring.

Proposed architecture (subject to electrical validation):

```text
Suitable battery + BMS + charger (voltage still TBD)
                  |
           fused DC power
                  |
       adapter made from genuine
       BionX classic power extension
                  |
     ORIGINAL BionX 3554 hub motor
        (internal controller retained)
                  |
      original CAN signal connection
                  |
        CAN transceiver/interface
                  |
          ESP32-C3 controller
             /         \
    Hall thumb throttle  New LCD/OLED display
       + brake input     + assist/status controls
```

- **Microcontroller proposal:** ESP32-C3 with an **external CAN transceiver** (the ESP32-C3 has a TWAI CAN controller, but not a physical CAN transceiver). The CAN transceiver/interface must use appropriate logic levels and electrical protection.
- **Known protocol reference:** BionXtool documents the BionX CAN protocol: **125 kbit/s**, 11-bit IDs, console `0x08` as master, battery `0x10`, motor `0x20`; query/reply/set frames of 2 or 4 bytes; a near-complete battery register map; and a capture of a stock power-on sequence. Published reference, not yet measured on this bicycle. Extracted to [BMS_Protocol.md](BMS_Protocol.md).
- **Existing proof-of-concept reference:** The open-source **BionX Minimal Translator** drives a BionX CAN motor with a generic battery and a Kunteng display on an **STM32F103**. **Confirmed from its source on 2026-10-25: it talks only to motor node `0x20`, emulates no battery node, and feeds the motor's 12 V and 5 V rails from buck converters.** Adapting it to ESP32-C3 is **proposed work**, not an already tested port.
- **Battery emulation is not required for the motor to run.** This follows from the Minimal Translator source. The earlier field report of the system shutting off every five minutes came from a setup that kept the stock console, whose inactivity timer is a battery register (`0x56/0x57`). Replacing the console removes that dependency. Emulating the battery node is needed only if the stock console is retained.
- **No CAN command wakes a BMS in Deep Sleep.** The battery register map contains shutdown (`0x25`), ship mode (`0x37`) and timers, but no wake or unlock-from-lockout register. Deep Sleep exits only on external DC at the charge input. Deep Sleep entry triggers: 60 days idle, 7 days under 10% SoC, or average cell voltage under 2.54 V. The last is the deep-discharge lockout; recovery is external cell charging followed by the 26 V supply.
- **Once awake, the BMS is fully diagnosable over CAN without opening the case:** alert flags mapped to console error codes (`0x1D/0xF0`), charger manager state (`0x13`), DC-in voltage (`0x9A/0x9B`), ship-mode flag (`0x37`), all 13 cell voltages via the cell-monitor gateway (`0x6C` → `0x6D/0x6E`). Protected writes go through `0x71` with a seconds value, never above `0x54`.
- **Desired retained features:** Native motor torque-sensing pedal assist and regenerative braking, **if confirmed functional and supported by the final control interface**.
- **Safety requirements:** No-motion on boot, throttle plausibility and out-of-range detection, brake override, CAN-command timeout, conservative torque/current limits, physical battery isolation and fusing, and testing with the drive wheel off the ground.

## 5. Outstanding verification before power-up

- [x] Find the **removable battery's complete model/voltage label** or otherwise identify its original cell arrangement. **Done 2026-10-25:** part `3562`, 48 V, 13S4P.
- [ ] Confirm the **motor controller's allowable DC voltage** beyond the factory 13S window (through exact model reference, diagnostic registers, or inspected electronics as necessary; motor remains unopened under current project scope). The factory pairing with a 13S pack establishes 13S as safe; no headroom above that is established.
- [ ] Confirm **physical mating** of the classic-power and Hirose extension plugs with this particular motor.
- [ ] With everything unpowered, map the extension wires to actual connector contacts using continuity tests; independently check the power and CAN terminals.
- [~] Determine whether the motor relies on a **separate low-voltage logic supply**, original battery wake/enable logic, CAN startup messages, or other BionX battery functions. **Partly resolved from source 2026-10-25:** the motor needs **12 V (vControl) and 5 V** rails that the BionX pack normally supplies, and the Minimal Translator provides them from buck converters. Periodic battery CAN frames are **not** required by the motor. Still to measure on this bike: which pins carry the rails, and the motor's own wake sequence (`SET 0x20 0x02`, `0x41`, `0x42`). See [BMS_Protocol.md](BMS_Protocol.md).
- [ ] Identify CAN protocol/firmware version and validate startup/command behavior with a protected bench setup. The published motor power-on sequence is: `SET 0x02 = 0` (wake), `SET 0x41 = 0`, `SET 0x42 = 1` (direction), `QRY 0x20` (fw rev), `QRY 0x6C` (gauge type), `SET 0xA5 = 0xAA` (unlock, 30 s), then current limits into `0x7A`-`0x7D`.
- [ ] If the original pack revives: capture a full stock power-on/off CAN trace with BionXtool before any other change.
- [ ] Select final battery voltage, chemistry, BMS (including handling of regenerative current), fuse, charger and connectorized harness.
- [ ] Select throttle and display only after confirming the chosen translator firmware and required control functions.
- [ ] Verify TRC/axle anti-rotation hardware, brakes, thermal monitoring, and safe low-power first operation.

## 6. Research references

1. **BionX dealer service manual** — motor `3554` in CAN family; serial number fields; distinction between CAN and I²C; lower-voltage CAN configurations.  
   https://manualzz.com/doc/23501868/document
2. **BX Legacy charger listing** — BionX `01-3635`: 26 V, 3.45 A supply for original 48 V BionX batteries.  
   https://www.bx-legacy.com/products/chargerpowersupply90w-26v-3-45aforall48vbatteries
3. **BX Legacy harness catalog** — original-style classic power and Hirose communication extension options.  
   https://www.bx-legacy.com/collections/battery-harness
4. **BX Legacy technical support** — protocol generations and warning against casually increasing older motor operating voltage.  
   https://www.bx-legacy.com/pages/technical-support-information
5. **BionXtool** — open-source CAN diagnostics, HR30 connector pair, and published 125 kbit/s interface setup.  
   https://github.com/buxtronix/BionXtool
6. **BionX Minimal Translator** — existing STM32-based generic-battery / Kunteng-display CAN motor translator.  
   https://github.com/stancecoke/BionX_Minimal_Translator
7. **akkuplus.de cell-swap listing** — BionX `4073` / `3562` / `3760` grouped as the 48 V Li-ion family, 52 × 18650 cells (13S4P).  
   https://akkuplus.de/Akkureparatur-Zellentausch-BionX-4073-3562-3760-48-Volt-Li-Ion-Akku
8. **akku-mainz.de BionX 3562 repair page** — second independent listing of `3562` as a 48 V pack.  
   https://akku-mainz.de/bionx-3562
9. **BionXtool `registers.h`, `PROTOCOL.md`, `bike-on-off.pcap`** — battery register map, frame format, stock power-on capture (console fw 112, battery hw 0x63).  
   https://github.com/buxtronix/BionXtool
10. **BionX Minimal Translator `main.c`** — motor-only control loop, no battery node, 12 V / 5 V from buck converters.  
    https://github.com/stancecoke/BionX_Minimal_Translator/blob/master/main.c

---

**Project decision summary:** Retain the **BionX 3554 CAN-bus hub motor**, preserve its factory plugs, pursue a **classic-connector extension harness** and an **ESP32-C3 CAN control interface**. **Original pack confirmed as 48 V 13S (part `3562`).** **The motor runs without a battery node on the bus (Minimal Translator source), so tier-1 ESP32 scope is a motor-only control loop plus 12 V / 5 V rails.** Reviving the original pack is useful for a stock CAN capture and as a fallback battery, not a prerequisite.