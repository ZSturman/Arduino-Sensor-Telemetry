#pragma once

#include <Encoder.h>

#include "../core/SensorModule.h"

class RotaryEncoderSensor : public SensorModule {
 public:
  RotaryEncoderSensor();
  void begin(TelemetryPublisher& publisher) override;
  void tick(unsigned long nowMs, TelemetryPublisher& publisher) override;
  void appendSnapshot(JsonObject metrics, JsonObject sensorStatus) override;

 private:
  void emitMovementEvent(long delta, unsigned long nowMs, TelemetryPublisher& publisher);

  Encoder encoder_;
  long position_ = 0;
  long delta_ = 0;
  bool buttonPressed_ = false;
  bool lastPublishedButton_ = false;
  unsigned long lastReadMs_ = 0;
};

