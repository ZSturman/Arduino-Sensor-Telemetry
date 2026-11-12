import { describe, expect, it } from "vitest";

import { buildSensorCards, emptyRecentTelemetry, mergeEnvelope } from "./telemetry";
import type { SensorCatalogPayload, TelemetryEnvelope } from "../types/telemetry";

const catalog: SensorCatalogPayload = {
  schema_version: "1.0.0",
  board: "Arduino Mega 2560",
  firmware_sketch: "firmware/mega_telemetry/mega_telemetry.ino",
  transport_note: "USB now, Bluetooth next.",
  sensors: [
    {
      id: "dht11",
      name: "DHT11",
      category: "environment",
      purpose: "Reads climate.",
      required_parts: [],
      pins: [],
      wiring_summary: "",
      breadboard_description: "",
      ascii_diagram: "",
      firmware: {
        file: "firmware",
        module: "DhtSensor",
        function: "tick"
      },
      libraries: [],
      expected_fields: ["temperature_c", "humidity_pct"],
      frontend: {
        card_title: "Environment",
        appearance: "Card"
      },
      constraints: []
    }
  ]
};

describe("telemetry helpers", () => {
  it("merges telemetry envelopes into current state", () => {
    const envelope: TelemetryEnvelope = {
      schema_version: "1.0.0",
      message_type: "telemetry",
      timestamp_ms: 10,
      sequence: 1,
      device_id: "mega",
      transport: "usb_serial",
      metrics: {
        temperature_c: 23.5,
        humidity_pct: 50.1
      },
      sensor_status: {
        dht11: {
          healthy: true
        }
      }
    };

    const recent = mergeEnvelope(emptyRecentTelemetry(), envelope);
    expect(recent.latest_metrics.temperature_c).toBe(23.5);
    expect(recent.numeric_history.temperature_c).toHaveLength(1);
  });

  it("builds cards from catalog fields", () => {
    const recent = mergeEnvelope(emptyRecentTelemetry(), {
      schema_version: "1.0.0",
      message_type: "telemetry",
      timestamp_ms: 12,
      sequence: 2,
      device_id: "mega",
      transport: "usb_serial",
      metrics: {
        temperature_c: 24.1,
        humidity_pct: 49.8
      },
      sensor_status: {
        dht11: {
          healthy: true
        }
      }
    });
    const cards = buildSensorCards(catalog, recent);
    expect(cards[0]?.fields[0]?.value).toBe(24.1);
  });
});

