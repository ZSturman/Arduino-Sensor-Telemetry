import asyncio

from app.models.connection import ConnectRequest
from app.transports.simulator import SimulatorTransport


async def test_simulator_emits_lines() -> None:
    transport = SimulatorTransport()
    lines: list[str] = []

    async def handle_line(line: str) -> None:
        lines.append(line)
        if len(lines) >= 2:
            await transport.disconnect()

    async def handle_disconnect(reason: str | None) -> None:
        del reason

    await transport.connect(ConnectRequest(transport="simulator"), handle_line, handle_disconnect)
    await asyncio.sleep(1.2)

    assert len(lines) >= 1

