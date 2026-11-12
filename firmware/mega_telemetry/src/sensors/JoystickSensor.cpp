#include "JoystickSensor.h"

#include "../core/AppConfig.h"
#include "../core/TelemetryPublisher.h"

JoystickSensor::JoystickSensor() : SensorModule("joystick") {}

void JoystickSensor::begin(TelemetryPublisher& publisher) {
  (void)publisher;
  pinMode(Config::kJoystickButtonPin, INPUT_PULLUP);
}

void JoystickSensor::tick(unsigned long nowMs, TelemetryPublisher& publisher) {
  if (nowMs - lastReadMs_ < Config::kJoystickIntervalMs) {
    return;
  }

  lastReadMs_ = nowMs;
  x_ = analogRead(Config::kJoystickXPin);
  y_ = analogRead(Config::kJoystickYPin);
  buttonPressed_ = digitalRead(Config::kJoystickButtonPin) == LOW;

  if (buttonPressed_ == lastPublishedButton_) {
    return;
  }

  lastPublishedButton_ = buttonPressed_;
  DynamicJsonDocument eventDoc(Config::kEventDocumentCapacity);
  JsonObject fields;
  publisher.beginEvent(eventDoc, id(), "joystick_button_changed", nowMs, fields);
  fields["joy_button"] = buttonPressed_;
  publisher.send(eventDoc);
}

void JoystickSensor::appendSnapshot(JsonObject metrics, JsonObject sensorStatus) {
  metrics["joy_x"] = x_;
  metrics["joy_y"] = y_;
  metrics["joy_button"] = buttonPressed_;

  JsonObject status = sensorStatus.createNestedObject(id());
  status["connected"] = true;
  status["healthy"] = true;
  status["last_update_ms"] = lastReadMs_;
}

