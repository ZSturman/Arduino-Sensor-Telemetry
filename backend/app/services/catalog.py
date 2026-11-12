from __future__ import annotations

import json
from pathlib import Path
from typing import Any


class CatalogService:
    def __init__(self, catalog_path: Path) -> None:
        self._catalog_path = catalog_path
        self._catalog = self._load()

    def _load(self) -> dict[str, Any]:
        with self._catalog_path.open("r", encoding="utf-8") as handle:
            return json.load(handle)

    @property
    def payload(self) -> dict[str, Any]:
        return self._catalog

