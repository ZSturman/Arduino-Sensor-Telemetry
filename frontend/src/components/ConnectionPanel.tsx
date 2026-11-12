import type { ConnectRequest } from "../lib/api";
import type { ConnectionState, TransportDescriptor, TransportEndpoint } from "../types/telemetry";
import { StatusBadge } from "./StatusBadge";

interface ConnectionPanelProps {
  connection: ConnectionState;
  transports: TransportDescriptor[];
  endpoints: TransportEndpoint[];
  draft: ConnectRequest;
  onDraftChange: (next: ConnectRequest) => void;
  onConnect: () => void;
  onDisconnect: () => void;
}

function transportTone(state: ConnectionState): "ok" | "warn" | "danger" | "neutral" {
  switch (state.connection_state) {
    case "connected":
      return "ok";
    case "connecting":
    case "reconnecting":
      return "warn";
    case "error":
      return "danger";
    default:
      return "neutral";
  }
}

export function ConnectionPanel({
  connection,
  transports,
  endpoints,
  draft,
  onDraftChange,
  onConnect,
  onDisconnect
}: ConnectionPanelProps) {
  const isSimulator = draft.transport === "simulator";

  return (
    <section className="panel">
      <div className="panel__header">
        <div>
          <p className="eyebrow">Transport Control</p>
          <h2>Connection & Transport</h2>
        </div>
        <div className="status-row">
          <StatusBadge label="Backend online" tone="ok" />
          <StatusBadge label={connection.connection_state} tone={transportTone(connection)} />
        </div>
      </div>

      <div className="connection-grid">
        <label>
          Transport
          <select
            value={draft.transport}
            onChange={(event) =>
              onDraftChange({
                ...draft,
                transport: event.target.value,
                endpoint: null
              })
            }
          >
            {transports.map((transport) => (
              <option key={transport.id} value={transport.id} disabled={!transport.supported}>
                {transport.label}
                {!transport.supported ? " (planned)" : ""}
              </option>
            ))}
          </select>
        </label>

        <label>
          Endpoint / Port
          <select
            value={draft.endpoint ?? ""}
            onChange={(event) =>
              onDraftChange({
                ...draft,
                endpoint: event.target.value || null
              })
            }
          >
            <option value="">Select an endpoint</option>
            {endpoints.map((endpoint) => (
              <option key={endpoint.id} value={endpoint.id}>
                {endpoint.label}
              </option>
            ))}
          </select>
        </label>

        <label>
          Baud Rate
          <input
            type="number"
            min={9600}
            step={100}
            value={draft.baud_rate}
            onChange={(event) =>
              onDraftChange({
                ...draft,
                baud_rate: Number(event.target.value)
              })
            }
            disabled={isSimulator}
          />
        </label>

        <label>
          Logging
          <select
            value={draft.logging_mode}
            onChange={(event) =>
              onDraftChange({
                ...draft,
                logging_mode: event.target.value as ConnectRequest["logging_mode"]
              })
            }
          >
            <option value="off">Off</option>
            <option value="jsonl">JSONL</option>
            <option value="jsonl_csv">JSONL + CSV</option>
          </select>
        </label>
      </div>

      <div className="button-row">
        <button onClick={onConnect}>Connect</button>
        <button className="button--ghost" onClick={onDisconnect}>
          Disconnect
        </button>
      </div>

      <div className="meta-grid">
        <div>
          <span className="meta-label">Selected transport</span>
          <strong>{connection.transport ?? draft.transport}</strong>
        </div>
        <div>
          <span className="meta-label">Selected port</span>
          <strong>{connection.endpoint ?? draft.endpoint ?? "Not connected"}</strong>
        </div>
        <div>
          <span className="meta-label">Device</span>
          <strong>{connection.device_id ?? "Waiting for telemetry"}</strong>
        </div>
        <div>
          <span className="meta-label">Malformed lines</span>
          <strong>{connection.malformed_messages}</strong>
        </div>
      </div>
    </section>
  );
}

