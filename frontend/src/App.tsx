import { useEffect, useMemo, useState } from "react";

import {
  connectTelemetry,
  disconnectTelemetry,
  getCatalog,
  getConnection,
  getEndpoints,
  getHealth,
  getRecentTelemetry,
  getTransports,
  type ConnectRequest
} from "./lib/api";
import {
  buildSensorCards,
  emptyRecentTelemetry,
  getNumericMetricKeys,
  getUnmappedMetrics,
  mergeSocketMessage
} from "./lib/telemetry";
import { useTelemetrySocket } from "./hooks/useTelemetrySocket";
import { ConnectionPanel } from "./components/ConnectionPanel";
import { EventFeed } from "./components/EventFeed";
import { HelpPanel } from "./components/HelpPanel";
import { RawConsole } from "./components/RawConsole";
import { SensorCard } from "./components/SensorCard";
import type {
  ConnectionState,
  HealthPayload,
  RecentTelemetryResponse,
  SensorCatalogPayload,
  SocketMessage,
  TransportDescriptor,
  TransportEndpoint
} from "./types/telemetry";

const defaultConnectionState: ConnectionState = {
  backend_connected: true,
  connection_state: "disconnected",
  transport: null,
  endpoint: null,
  baud_rate: null,
  auto_reconnect: true,
  malformed_messages: 0,
  last_error: null,
  device_id: null,
  last_sequence: null,
  last_message_at: null
};

const defaultDraft: ConnectRequest = {
  transport: "usb_serial",
  endpoint: null,
  baud_rate: 115200,
  auto_reconnect: true,
  logging_mode: "jsonl",
  simulator_profile: "default"
};

export default function App() {
  const [health, setHealth] = useState<HealthPayload | null>(null);
  const [connection, setConnection] = useState<ConnectionState>(defaultConnectionState);
  const [transports, setTransports] = useState<TransportDescriptor[]>([]);
  const [endpoints, setEndpoints] = useState<TransportEndpoint[]>([]);
  const [catalog, setCatalog] = useState<SensorCatalogPayload | null>(null);
  const [recent, setRecent] = useState<RecentTelemetryResponse>(emptyRecentTelemetry());
  const [draft, setDraft] = useState<ConnectRequest>(defaultDraft);
  const [socketErrors, setSocketErrors] = useState<string[]>([]);

  useEffect(() => {
    async function load() {
      const [healthPayload, transportPayload, connectionPayload, catalogPayload, recentPayload] =
        await Promise.all([
          getHealth(),
          getTransports(),
          getConnection(),
          getCatalog(),
          getRecentTelemetry()
        ]);
      setHealth(healthPayload);
      setTransports(transportPayload);
      setConnection(connectionPayload);
      setCatalog(catalogPayload);
      setRecent(recentPayload);
    }

    void load();
  }, []);

  useEffect(() => {
    async function loadEndpoints() {
      if (!draft.transport) {
        setEndpoints([]);
        return;
      }
      const nextEndpoints = await getEndpoints(draft.transport);
      setEndpoints(nextEndpoints);
      if (!draft.endpoint && nextEndpoints.length > 0) {
        setDraft((current) => ({ ...current, endpoint: nextEndpoints[0].id }));
      }
    }

    void loadEndpoints();
  }, [draft.transport]);

  const handleSocketMessage = (message: SocketMessage) => {
    if (message.type === "connection_state") {
      setConnection(message.payload);
      return;
    }
    setRecent((current) => mergeSocketMessage(current, message));
  };

  useTelemetrySocket({
    onMessage: handleSocketMessage,
    onError: (error) => {
      setSocketErrors((current) => [...current.slice(-2), error]);
    }
  });

  const sensorCards = useMemo(() => buildSensorCards(catalog, recent), [catalog, recent]);
  const unmappedMetrics = useMemo(() => getUnmappedMetrics(catalog, recent), [catalog, recent]);
  const numericKeys = useMemo(() => getNumericMetricKeys(recent), [recent]);

  async function handleConnect() {
    const nextState = await connectTelemetry(draft);
    setConnection(nextState);
  }

  async function handleDisconnect() {
    const nextState = await disconnectTelemetry();
    setConnection(nextState);
  }

  return (
    <main className="shell">
      <section className="hero">
        <p className="eyebrow">Arduino Mega 2560 Telemetry</p>
        <h1>Live sensor telemetry over USB serial, staged for Bluetooth next.</h1>
        <p className="hero__copy">
          The firmware emits newline-delimited JSON, the FastAPI backend manages transport and
          streaming, and this dashboard renders both known sensor cards and new telemetry keys as
          they appear.
        </p>
        <div className="hero__meta">
          <span>Backend: {health?.status ?? "loading"}</span>
          <span>Last message: {connection.last_message_at ?? "none yet"}</span>
          <span>Latest device timestamp: {recent.latest_message?.timestamp_ms ?? "n/a"} ms</span>
        </div>
      </section>

      <ConnectionPanel
        connection={connection}
        transports={transports}
        endpoints={endpoints}
        draft={draft}
        onDraftChange={setDraft}
        onConnect={handleConnect}
        onDisconnect={handleDisconnect}
      />

      {socketErrors.length > 0 ? (
        <section className="panel panel--warning">
          <h2>Socket Warnings</h2>
          {socketErrors.map((error) => (
            <p key={error}>{error}</p>
          ))}
        </section>
      ) : null}

      <section className="panel">
        <div className="panel__header">
          <div>
            <p className="eyebrow">Telemetry Overview</p>
            <h2>Known Sensor Cards</h2>
          </div>
          <div className="hero__meta">
            <span>Metrics tracked: {numericKeys.length}</span>
            <span>Unmapped fields: {unmappedMetrics.length}</span>
          </div>
        </div>
        <div className="sensor-grid">
          {sensorCards.map((card) => (
            <SensorCard key={card.id} card={card} />
          ))}
        </div>
      </section>

      <section className="panel">
        <div className="panel__header">
          <div>
            <p className="eyebrow">Schema Flexibility</p>
            <h2>Unmapped / Newly Discovered Fields</h2>
          </div>
        </div>
        <div className="unmapped-grid">
          {unmappedMetrics.length === 0 ? <p>No unmapped metrics yet.</p> : null}
          {unmappedMetrics.map(([key, value]) => (
            <div key={key} className="metric-tile">
              <span className="metric-tile__label">{key}</span>
              <strong className="metric-tile__value">{String(value)}</strong>
            </div>
          ))}
        </div>
      </section>

      <EventFeed events={recent.recent_events} catalog={catalog} />
      <RawConsole lines={recent.raw_console} />
      <HelpPanel catalog={catalog} />
    </main>
  );
}

