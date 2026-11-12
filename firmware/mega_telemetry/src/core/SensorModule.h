#pragma once

#include <ArduinoJson.h>

class TelemetryPublisher;

class SensorModule {
 public:
  explicit SensorModule(const char* sensorId) : sensorId_(sensorId) {}
  virtual ~SensorModule() = default;

  virtual void begin(TelemetryPublisher& publisher) = 0;
  virtual void tick(unsigned long nowMs, TelemetryPublisher& publisher) = 0;
  virtual void appendSnapshot(JsonObject metrics, JsonObject sensorStatus) = 0;

  const char* id() const { return sensorId_; }

 private:
  const char* sensorId_;
};

