#include "SoundSensor.h"

#include "../core/AppConfig.h"

SoundSensor::SoundSensor() : SensorModule("sound_sensor") {}

void SoundSensor::begin(TelemetryPublisher& publisher) {
  (void)publisher;
}

void SoundSensor::tick(unsigned long nowMs, TelemetryPublisher& publisher) {
  (void)publisher;
  if (nowMs - lastReadMs_ < Config::kSoundIntervalMs) {
    return;
  }

  lastReadMs_ = nowMs;
  soundRaw_ = analogRead(Config::kSoundAnalogPin);
}

void SoundSensor::appendSnapshot(JsonObject metrics, JsonObject sensorStatus) {
  metrics["sound_raw"] = soundRaw_;

  JsonObject status = sensorStatus.createNestedObject(id());
  status["connected"] = true;
  status["healthy"] = true;
  status["last_update_ms"] = lastReadMs_;
}

