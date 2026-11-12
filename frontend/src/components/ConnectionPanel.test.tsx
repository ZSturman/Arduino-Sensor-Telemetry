import { fireEvent, render, screen } from "@testing-library/react";
import { describe, expect, it, vi } from "vitest";

import { ConnectionPanel } from "./ConnectionPanel";
import type { ConnectRequest } from "../lib/api";
import type { ConnectionState, TransportDescriptor, TransportEndpoint } from "../types/telemetry";

const connection: ConnectionState = {
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

const transports: TransportDescriptor[] = [
  {
    id: "usb_serial",
    label: "USB Serial",
    description: "USB",
    supported: true,
    hardware_backed: true
  }
];

const endpoints: TransportEndpoint[] = [
  {
    id: "/dev/cu.usbmodem123",
    label: "/dev/cu.usbmodem123",
    description: "Arduino",
    available: true,
    metadata: {}
  }
];

const draft: ConnectRequest = {
  transport: "usb_serial",
  endpoint: "/dev/cu.usbmodem123",
  baud_rate: 115200,
  auto_reconnect: true,
  logging_mode: "jsonl",
  simulator_profile: "default"
};

describe("ConnectionPanel", () => {
  it("fires the connect callback", () => {
    const onConnect = vi.fn();
    render(
      <ConnectionPanel
        connection={connection}
        transports={transports}
        endpoints={endpoints}
        draft={draft}
        onDraftChange={vi.fn()}
        onConnect={onConnect}
        onDisconnect={vi.fn()}
      />
    );

    fireEvent.click(screen.getByRole("button", { name: "Connect" }));
    expect(onConnect).toHaveBeenCalledTimes(1);
  });
});
