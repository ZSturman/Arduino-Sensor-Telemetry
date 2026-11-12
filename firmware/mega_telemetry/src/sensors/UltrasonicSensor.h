#pragma once

#include <NewPing.h>

#include "../core/SensorModule.h"

class UltrasonicSensor : public SensorModule {
 public:
  UltrasonicSensor();
  void begin(TelemetryPublisher& publisher) override;
  void tick(unsigned long nowMs, TelemetryPublisher& publisher) override;
  void appendSnapshot(JsonObject metrics, JsonObject sensorStatus) override;

 private:
  NewPing sonar_;
  unsigned long lastReadMs_ = 0;
  float distanceCm_ = 0.0f;
};

