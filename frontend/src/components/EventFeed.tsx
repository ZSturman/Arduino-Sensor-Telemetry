import type { SensorCatalogPayload, TelemetryEnvelope } from "../types/telemetry";
import { findSensorById } from "../lib/telemetry";

interface EventFeedProps {
  events: TelemetryEnvelope[];
  catalog: SensorCatalogPayload | null;
}

export function EventFeed({ events, catalog }: EventFeedProps) {
  return (
    <section className="panel">
      <div className="panel__header">
        <div>
          <p className="eyebrow">Event Stream</p>
          <h2>Recent Sensor Events</h2>
        </div>
      </div>
      <div className="event-list">
        {events.length === 0 ? <p>No events yet.</p> : null}
        {[...events].reverse().map((event) => {
          const sensor = findSensorById(catalog, event.event?.source ?? "");
          return (
            <article key={`${event.sequence}-${event.timestamp_ms}`} className="event-item">
              <div>
                <strong>{sensor?.name ?? event.event?.source}</strong>
                <p>{event.event?.name}</p>
              </div>
              <pre>{JSON.stringify(event.event?.fields ?? {}, null, 2)}</pre>
            </article>
          );
        })}
      </div>
    </section>
  );
}

