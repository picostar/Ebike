# BionX 3554 ESP32-C3 Controller — research scaffold

**BENCH-ONLY: does not control a motor.** 

Goal: preserve the BionX 3554 CAN-bus hub motor and its internal controller. Investigate a reversible extension-cable interface for a modern battery, Hall thumb throttle, and new display. Original pack confirmed as BionX `3562`, 48 V nominal 13S4P Li-ion. CAN pinout and startup protocol remain **UNVERIFIED**.

Discovery and connector sources: [Confirmed_Discoveries.md](docs/Confirmed_Discoveries.md).

## Components
- ESP32-C3 prototype controller
- CAN transceiver and electrical protection (not selected/installed)
- Original-style BionX extension cables rather than modifying the motor harness
- Hall throttle, brake interlock, and display (not implemented)
- Replacement battery and BMS only after voltage and regeneration limits are proven

## Current firmware
The PlatformIO project only prints ADC readings from a 0–3.3 V bench potentiometer on GPIO0 over USB serial. It contains NO motor control, NO CAN transmission, and is not riding firmware. Do not connect it to the BionX system.

To test: open firmware/ as a PlatformIO project, choose the actual ESP32-C3 board, connect a test potentiometer ends to 3.3 V and GND and the wiper to GPIO0, flash over USB, then open a 115200-baud serial monitor. Do not connect any unknown-voltage throttle or bike wiring.

## Development order
1. ~~Original battery cell count~~ (done: 13S, part `3562`). Verify hub controller headroom above 13S and motor power wiring.
2. Purchase and continuity-map mating classic power and Hirose extension harnesses.
3. Specify safe power supply, protection, braking, watchdog, and regenerative operation.
4. Implement receive-only CAN observation with correct external transceiver, safe bus loading, and known-ground topology.
5. Analyze BionXtool and BionX Minimal Translator as protocol references.
6. Implement control firmware only after documented hazard analysis and bench protocol validation.

References: https://github.com/buxtronix/BionXtool and https://github.com/stancecoke/BionX_Minimal_Translator.

Not licensed or tested as a roadworthy safety controller.
