#pragma once

#include "../core/SensorModule.h"

class TouchSensor : public SensorModule {
 public:
  TouchSensor();
  void begin(TelemetryPublisher& publisher) override;
  void tick(unsigned long nowMs, TelemetryPublisher& publisher) override;
  void appendSnapshot(JsonObject metrics, JsonObject sensorStatus) override;

 private:
  bool touched_ = false;
  bool lastPublishedTouched_ = false;
  unsigned long lastReadMs_ = 0;
};

