#!/usr/bin/env python3
"""Basic offline structural checks for the project's Wokwi diagram."""
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
DIAGRAM = ROOT / "diagram.json"
data = json.loads(DIAGRAM.read_text(encoding="utf-8"))
assert data.get("version") == 1, "Wokwi diagram version must be 1"
parts = data.get("parts", [])
connections = data.get("connections", [])
ids = [part.get("id") for part in parts]
assert len(ids) == len(set(ids)), "Part IDs must be unique"
assert {part.get("type") for part in parts} >= {
    "wokwi-esp32-devkit-v1",
    "wokwi-potentiometer",
    "wokwi-servo",
}, "Expected ESP32, potentiometer and servo parts"

normalized = {(a, b) for a, b, *_ in connections}
normalized |= {(b, a) for a, b, *_ in connections}
required = {
    ("emg_mock:SIG", "esp:34"),
    ("servo1:PWM", "esp:18"),
    ("emg_mock:VCC", "esp:3V3"),
    ("servo1:V+", "esp:5V"),
}
missing = required - normalized
assert not missing, f"Missing required Wokwi connections: {sorted(missing)}"
assert all(len(wire) >= 3 for wire in connections), "Each wire needs source, target and color"
print(f"Wokwi diagram structure passed ({len(parts)} parts, {len(connections)} wires).")
