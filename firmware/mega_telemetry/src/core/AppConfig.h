#pragma once

#include <Arduino.h>

// Compile-time sensor toggles. Disable a module here without touching the rest
// of the telemetry pipeline.
#define ENABLE_SENSOR_DHT11 1
#define ENABLE_SENSOR_PIR 1
#define ENABLE_SENSOR_ULTRASONIC 1
#define ENABLE_SENSOR_SOUND 1
#define ENABLE_SENSOR_RAIN 1
#define ENABLE_SENSOR_JOYSTICK 1
#define ENABLE_SENSOR_TOUCH 1
#define ENABLE_SENSOR_ROTARY_ENCODER 1
#define ENABLE_SENSOR_PUSHBUTTON 1
#define ENABLE_SENSOR_IR_RECEIVER 1
#define ENABLE_SENSOR_RFID 1
#define ENABLE_SENSOR_KEYPAD 1

namespace Config {
constexpr char kSchemaVersion[] = "1.0.0";
constexpr char kDeviceId[] = "mega2560-sensor-rig";
constexpr char kActiveTransport[] = "usb_serial";
constexpr unsigned long kBaudRate = 115200;

constexpr unsigned long kSnapshotIntervalMs = 500;
constexpr size_t kSnapshotDocumentCapacity = 4096;
constexpr size_t kEventDocumentCapacity = 1024;
constexpr size_t kMaxSensors = 16;

constexpr unsigned int kUltrasonicMaxDistanceCm = 400;

// Pins 18/19 are intentionally left free for the next Bluetooth phase on
// Serial1. USB serial stays on Serial in Version 1.
constexpr uint8_t kDhtDataPin = 22;
constexpr uint8_t kPirPin = 23;
constexpr uint8_t kUltrasonicTriggerPin = 24;
constexpr uint8_t kUltrasonicEchoPin = 25;
constexpr uint8_t kJoystickButtonPin = 26;
constexpr uint8_t kTouchPin = 27;
constexpr uint8_t kEncoderButtonPin = 28;
constexpr uint8_t kRainDigitalPin = 29;
constexpr uint8_t kKeypadRowPins[4] = {30, 31, 32, 33};
constexpr uint8_t kKeypadColumnPins[4] = {34, 35, 36, 37};
constexpr uint8_t kIrReceiverPin = 6;
constexpr uint8_t kPushButtonPin = 7;
constexpr uint8_t kEncoderPinA = 2;
constexpr uint8_t kEncoderPinB = 3;
constexpr uint8_t kSoundAnalogPin = A0;
constexpr uint8_t kRainAnalogPin = A1;
constexpr uint8_t kJoystickXPin = A2;
constexpr uint8_t kJoystickYPin = A3;
constexpr uint8_t kRfidSsPin = 53;
constexpr uint8_t kRfidResetPin = 49;

constexpr unsigned long kDhtIntervalMs = 2000;
constexpr unsigned long kPirIntervalMs = 100;
constexpr unsigned long kUltrasonicIntervalMs = 250;
constexpr unsigned long kSoundIntervalMs = 100;
constexpr unsigned long kRainIntervalMs = 200;
constexpr unsigned long kJoystickIntervalMs = 100;
constexpr unsigned long kTouchIntervalMs = 75;
constexpr unsigned long kEncoderIntervalMs = 50;
constexpr unsigned long kPushButtonIntervalMs = 50;
constexpr unsigned long kIrPollIntervalMs = 20;
constexpr unsigned long kRfidPollIntervalMs = 100;
constexpr unsigned long kKeypadIntervalMs = 30;
}  // namespace Config

