#include "UltrasonicSensor.h"

#include "../core/AppConfig.h"

UltrasonicSensor::UltrasonicSensor()
    : SensorModule("hc_sr04"),
      sonar_(Config::kUltrasonicTriggerPin, Config::kUltrasonicEchoPin, Config::kUltrasonicMaxDistanceCm) {}

void UltrasonicSensor::begin(TelemetryPublisher& publisher) {
  (void)publisher;
}

void UltrasonicSensor::tick(unsigned long nowMs, TelemetryPublisher& publisher) {
  (void)publisher;
  if (nowMs - lastReadMs_ < Config::kUltrasonicIntervalMs) {
    return;
  }

  lastReadMs_ = nowMs;
  unsigned int centimeters = sonar_.ping_cm();
  distanceCm_ = static_cast<float>(centimeters);
}

void UltrasonicSensor::appendSnapshot(JsonObject metrics, JsonObject sensorStatus) {
  metrics["distance_cm"] = distanceCm_;

  JsonObject status = sensorStatus.createNestedObject(id());
  status["connected"] = true;
  status["healthy"] = true;
  status["last_update_ms"] = lastReadMs_;
}

