#include "RainSensor.h"

#include "../core/AppConfig.h"

RainSensor::RainSensor() : SensorModule("rain_sensor") {}

void RainSensor::begin(TelemetryPublisher& publisher) {
  (void)publisher;
  pinMode(Config::kRainDigitalPin, INPUT);
}

void RainSensor::tick(unsigned long nowMs, TelemetryPublisher& publisher) {
  (void)publisher;
  if (nowMs - lastReadMs_ < Config::kRainIntervalMs) {
    return;
  }

  lastReadMs_ = nowMs;
  rainRaw_ = analogRead(Config::kRainAnalogPin);
  rainDetected_ = digitalRead(Config::kRainDigitalPin) == LOW;
}

void RainSensor::appendSnapshot(JsonObject metrics, JsonObject sensorStatus) {
  metrics["rain_raw"] = rainRaw_;
  metrics["rain_detected"] = rainDetected_;

  JsonObject status = sensorStatus.createNestedObject(id());
  status["connected"] = true;
  status["healthy"] = true;
  status["last_update_ms"] = lastReadMs_;
}

