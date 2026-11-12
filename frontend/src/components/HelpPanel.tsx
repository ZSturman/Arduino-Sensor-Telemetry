import type { SensorCatalogPayload } from "../types/telemetry";

interface HelpPanelProps {
  catalog: SensorCatalogPayload | null;
}

export function HelpPanel({ catalog }: HelpPanelProps) {
  return (
    <section className="panel">
      <div className="panel__header">
        <div>
          <p className="eyebrow">Operator Help</p>
          <h2>Sensor Wiring & Firmware Map</h2>
        </div>
      </div>

      <div className="help-copy">
        <p>
          {catalog?.transport_note ??
            "Version 1 uses USB serial, with Bluetooth planned to reuse the same telemetry path."}
        </p>
      </div>

      <div className="help-grid">
        {catalog?.sensors.map((sensor) => (
          <article key={sensor.id} className="help-card">
            <h3>{sensor.name}</h3>
            <p>{sensor.wiring_summary}</p>
            <p>
              <strong>Pins:</strong>{" "}
              {sensor.pins.map((pin) => `${pin.mega_pin}→${pin.module_pin}`).join(", ")}
            </p>
            <p>
              <strong>Firmware:</strong> {sensor.firmware.module} in {sensor.firmware.file}
            </p>
            <p>
              <strong>Fields:</strong> {sensor.expected_fields.join(", ")}
            </p>
            <details>
              <summary>Show ASCII wiring and constraints</summary>
              <pre>{sensor.ascii_diagram}</pre>
              <ul>
                {sensor.constraints.map((constraint) => (
                  <li key={constraint}>{constraint}</li>
                ))}
              </ul>
            </details>
          </article>
        ))}
      </div>
    </section>
  );
}

