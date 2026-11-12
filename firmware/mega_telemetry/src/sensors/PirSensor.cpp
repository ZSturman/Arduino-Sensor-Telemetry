#include "PirSensor.h"

#include "../core/AppConfig.h"
#include "../core/TelemetryPublisher.h"

PirSensor::PirSensor() : SensorModule("pir_motion") {}

void PirSensor::begin(TelemetryPublisher& publisher) {
  (void)publisher;
  pinMode(Config::kPirPin, INPUT);
}

void PirSensor::tick(unsigned long nowMs, TelemetryPublisher& publisher) {
  if (nowMs - lastReadMs_ < Config::kPirIntervalMs) {
    return;
  }

  lastReadMs_ = nowMs;
  motion_ = digitalRead(Config::kPirPin) == HIGH;
  if (motion_ == lastPublishedMotion_) {
    return;
  }

  lastPublishedMotion_ = motion_;
  DynamicJsonDocument eventDoc(Config::kEventDocumentCapacity);
  JsonObject fields;
  publisher.beginEvent(eventDoc, id(), "motion_changed", nowMs, fields);
  fields["motion"] = motion_;
  publisher.send(eventDoc);
}

void PirSensor::appendSnapshot(JsonObject metrics, JsonObject sensorStatus) {
  metrics["motion"] = motion_;

  JsonObject status = sensorStatus.createNestedObject(id());
  status["connected"] = true;
  status["healthy"] = true;
  status["last_update_ms"] = lastReadMs_;
}

