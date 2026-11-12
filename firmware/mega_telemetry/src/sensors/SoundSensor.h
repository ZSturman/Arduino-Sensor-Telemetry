#pragma once

#include "../core/SensorModule.h"

class SoundSensor : public SensorModule {
 public:
  SoundSensor();
  void begin(TelemetryPublisher& publisher) override;
  void tick(unsigned long nowMs, TelemetryPublisher& publisher) override;
  void appendSnapshot(JsonObject metrics, JsonObject sensorStatus) override;

 private:
  int soundRaw_ = 0;
  unsigned long lastReadMs_ = 0;
};

