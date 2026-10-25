# Proposed architecture — unverified

1. Appropriate battery/BMS/fuse/isolator: voltage UNKNOWN until independently checked.
2. Extension power harness to original motor power connection: wiring/polarity UNKNOWN until measured.
3. External CAN PHY, isolated/protected as needed, to original Hirose motor connector: connector contacts UNKNOWN until measured.
4. ESP32-C3 CAN/TWAI controller and hardware watchdog (not yet implemented).
5. Hall throttle with plausibility checks and **independent brake inhibit**; startup requires neutral and loss of signal triggers torque disable.
6. Optional LCD/OLED display (not yet implemented).

The motor's internal controller must remain unchanged. Never put traction current through the Hirose signal connector. Original 26 V BionX charger is *not* a compatible generic 48 V lithium charger.

The BionXtool reference documents 125 kbit/s CAN, but the rate has **not** been measured on this motor. Passive sniffing first; CAN transmit only after a documented failure-mode analysis, logic power and startup sequence validation.
