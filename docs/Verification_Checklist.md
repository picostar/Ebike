# Safety and validation gate

- [x] Identify battery chemistry, original nominal voltage and maximum pack voltage. (2026-10-25: BionX `3562`, Li-ion 13S4P, 48 V nominal, ~54.6 V full.)
- [ ] Verify internal BionX motor controller's permissible power input range.
- [ ] Source genuine mating extension cables; independently verify genders/keying.
- [ ] Continuity-map *every* power and Hirose contact with equipment unpowered.
- [ ] Determine logic/wake requirements, ground domains and BionX CAN firmware version.
- [ ] Select current-limited bench supply and isolated measurement/protection.
- [ ] Verify motor braking, torque reaction collar, axle retention and bicycle brakes.
- [ ] Specify BMS, fusing, overvoltage handling and regenerative charging acceptance.
- [ ] Implement startup-neutral, brake override, throttle plausibility, watchdog and communications timeout.
- [ ] Bench-test motor wheel off ground with emergency disconnect, current/torque limits and safe containment.
- [ ] Obtain independent review before enabling any motion commands.

**No checklist items have been signed off. DO NOT DRIVE MOTOR.**
