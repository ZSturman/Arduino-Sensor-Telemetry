#include "TouchSensor.h"

#include "../core/AppConfig.h"
#include "../core/TelemetryPublisher.h"

TouchSensor::TouchSensor() : SensorModule("touch_sensor") {}

void TouchSensor::begin(TelemetryPublisher& publisher) {
  (void)publisher;
  pinMode(Config::kTouchPin, INPUT);
}

void TouchSensor::tick(unsigned long nowMs, TelemetryPublisher& publisher) {
  if (nowMs - lastReadMs_ < Config::kTouchIntervalMs) {
    return;
  }

  lastReadMs_ = nowMs;
  touched_ = digitalRead(Config::kTouchPin) == HIGH;
  if (touched_ == lastPublishedTouched_) {
    return;
  }

  lastPublishedTouched_ = touched_;
  DynamicJsonDocument eventDoc(Config::kEventDocumentCapacity);
  JsonObject fields;
  publisher.beginEvent(eventDoc, id(), "touch_changed", nowMs, fields);
  fields["touch"] = touched_;
  publisher.send(eventDoc);
}

void TouchSensor::appendSnapshot(JsonObject metrics, JsonObject sensorStatus) {
  metrics["touch"] = touched_;

  JsonObject status = sensorStatus.createNestedObject(id());
  status["connected"] = true;
  status["healthy"] = true;
  status["last_update_ms"] = lastReadMs_;
}

