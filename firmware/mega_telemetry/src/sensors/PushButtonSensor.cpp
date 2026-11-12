#include "PushButtonSensor.h"

#include "../core/AppConfig.h"
#include "../core/TelemetryPublisher.h"

PushButtonSensor::PushButtonSensor() : SensorModule("pushbutton") {}

void PushButtonSensor::begin(TelemetryPublisher& publisher) {
  (void)publisher;
  pinMode(Config::kPushButtonPin, INPUT_PULLUP);
}

void PushButtonSensor::tick(unsigned long nowMs, TelemetryPublisher& publisher) {
  if (nowMs - lastReadMs_ < Config::kPushButtonIntervalMs) {
    return;
  }

  lastReadMs_ = nowMs;
  pressed_ = digitalRead(Config::kPushButtonPin) == LOW;
  if (pressed_ == lastPublishedPressed_) {
    return;
  }

  lastPublishedPressed_ = pressed_;
  DynamicJsonDocument eventDoc(Config::kEventDocumentCapacity);
  JsonObject fields;
  publisher.beginEvent(eventDoc, id(), "button_changed", nowMs, fields);
  fields["button_pressed"] = pressed_;
  publisher.send(eventDoc);
}

void PushButtonSensor::appendSnapshot(JsonObject metrics, JsonObject sensorStatus) {
  metrics["button_pressed"] = pressed_;

  JsonObject status = sensorStatus.createNestedObject(id());
  status["connected"] = true;
  status["healthy"] = true;
  status["last_update_ms"] = lastReadMs_;
}

