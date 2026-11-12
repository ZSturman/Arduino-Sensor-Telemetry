from __future__ import annotations

from abc import ABC, abstractmethod
from collections.abc import Awaitable, Callable

from app.models.connection import ConnectRequest, TransportDescriptor, TransportEndpoint

LineHandler = Callable[[str], Awaitable[None]]
DisconnectHandler = Callable[[str | None], Awaitable[None]]


class TransportAdapter(ABC):
    descriptor: TransportDescriptor

    @abstractmethod
    async def list_endpoints(self) -> list[TransportEndpoint]:
        raise NotImplementedError

    @abstractmethod
    async def connect(
        self,
        request: ConnectRequest,
        line_handler: LineHandler,
        disconnect_handler: DisconnectHandler,
    ) -> None:
        raise NotImplementedError

    @abstractmethod
    async def disconnect(self) -> None:
        raise NotImplementedError

