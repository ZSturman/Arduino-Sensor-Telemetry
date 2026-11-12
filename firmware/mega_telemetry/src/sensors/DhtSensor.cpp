#include "DhtSensor.h"

#include "../core/AppConfig.h"

DhtSensor::DhtSensor() : SensorModule("dht11"), dht_(Config::kDhtDataPin, DHT11) {}

void DhtSensor::begin(TelemetryPublisher& publisher) {
  (void)publisher;
  dht_.begin();
}

void DhtSensor::tick(unsigned long nowMs, TelemetryPublisher& publisher) {
  (void)publisher;
  if (nowMs - lastReadMs_ < Config::kDhtIntervalMs) {
    return;
  }

  lastReadMs_ = nowMs;
  float humidity = dht_.readHumidity();
  float temperature = dht_.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    healthy_ = false;
    return;
  }

  humidityPct_ = humidity;
  temperatureC_ = temperature;
  healthy_ = true;
}

void DhtSensor::appendSnapshot(JsonObject metrics, JsonObject sensorStatus) {
  JsonObject status = sensorStatus.createNestedObject(id());
  status["connected"] = true;
  status["healthy"] = healthy_;
  status["last_update_ms"] = lastReadMs_;
  if (!healthy_) {
    status["notes"] = "Check wiring or wait for DHT11 warmup.";
    return;
  }

  metrics["temperature_c"] = temperatureC_;
  metrics["humidity_pct"] = humidityPct_;
}

