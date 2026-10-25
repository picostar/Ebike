// BionX G1 CAN controller research — BENCH ONLY
// Never attach to motor/battery. This program has no CAN TX or drive commands.

#include <Arduino.h>

#if !defined(BIONX_BENCH_ONLY)
#error "Only bench-only firmware is supported"
#endif

namespace {
constexpr int kTestAnalogPin = 0; // GPIO0 — isolated potentiometer 0..3.3V only
constexpr uint32_t kSampleIntervalMs = 250;
uint32_t lastSampleMs = 0;
}

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  pinMode(kTestAnalogPin, INPUT);
  Serial.println("BionX 3554 BENCH-ONLY; NO CAN TRANSMIT, NO MOTOR CONTROL");
}

void loop() {
  const uint32_t now = millis();
  if (now - lastSampleMs < kSampleIntervalMs) return;
  lastSampleMs = now;

  const int raw = analogRead(kTestAnalogPin);
  const float fraction = static_cast<float>(raw) / 4095.0f;
  Serial.printf("Bench GPIO0: raw=%d ratio=%.3f CAN_TX=DISABLED\n", raw, fraction);
}
