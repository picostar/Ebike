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
| Existing controls | Photographs | Original BionX **G1-style handlebar console** and thumb-operated control visible | Confirmed by photograph |
| Motor connections | Photographs | Motor axle harness has **separate heavy-current power and miniature multipin communication connections** | Confirmed visually; exact wire assignment unverified |

**Important correction to the early working assumption:** The appearance of a G1-style console does **not** establish I²C communication. This particular motor's `3554` part number places it in the **CAN-bus** family. Consequently the project should not be designed as an I²C retrofit.

## 2. Battery voltage: what is and is not established

- **Confirmed:** The charger marked `01-3635` outputs **26 V DC** and is specified by BX Legacy for BionX **48 V battery systems**.
- **Strong indication, not yet confirmed:** The bicycle may originally have had a **48 V** BionX traction battery, assuming this charger belongs to the pictured battery.
- **Not confirmed:** The removable battery's actual voltage/chemistry/cell count; the `3554` motor's **permitted DC input-voltage range**; the internal controller's voltage tolerance; the present condition of the battery electronics.
- **Do not conclude:** That the motor may automatically accept a generic 48 V pack simply because it uses CAN bus or the existing charger is associated with BionX 48 V batteries. BionX CAN-bus systems also existed at lower voltages.
- **If 13S Li-ion is ultimately approved:** A conventional nominal-48-V (13S) pack reaches **54.6 V fully charged**. This is a *candidate design*, **not yet an approved replacement** for this motor.
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
- **Known protocol reference:** BionXtool documents diagnostics/sniffing for BionX CAN systems and states a CAN bitrate of **125 kbit/s**. This is a published reference, not yet measured on this bicycle.
- **Existing proof-of-concept reference:** The open-source **BionX Minimal Translator** describes driving a BionX CAN motor with a generic battery and a Kunteng display using an **STM32F103** board. Adapting it to ESP32-C3 is **proposed work**, not an already tested port.
- **Desired retained features:** Native motor torque-sensing pedal assist and regenerative braking, **if confirmed functional and supported by the final control interface**.
- **Safety requirements:** No-motion on boot, throttle plausibility and out-of-range detection, brake override, CAN-command timeout, conservative torque/current limits, physical battery isolation and fusing, and testing with the drive wheel off the ground.

## 5. Outstanding verification before power-up

- [ ] Find the **removable battery's complete model/voltage label** or otherwise identify its original cell arrangement.
- [ ] Confirm the **motor controller's allowable DC voltage** (through exact model reference, diagnostic registers, or inspected electronics as necessary; motor remains unopened under current project scope).
- [ ] Confirm **physical mating** of the classic-power and Hirose extension plugs with this particular motor.
- [ ] With everything unpowered, map the extension wires to actual connector contacts using continuity tests; independently check the power and CAN terminals.
- [ ] Determine whether the motor relies on a **separate low-voltage logic supply**, original battery wake/enable logic, CAN startup messages, or other BionX battery functions.
- [ ] Identify CAN protocol/firmware version and validate startup/command behavior with a protected bench setup.
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

---

**Project decision summary:** Retain the **BionX 3554 CAN-bus hub motor**, preserve its factory plugs, pursue a **classic-connector extension harness** and an **ESP32-C3 CAN control interface**. **48 V remains a leading battery hypothesis—not yet a verified safe supply voltage.**