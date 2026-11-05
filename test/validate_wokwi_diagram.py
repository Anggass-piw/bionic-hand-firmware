#!/usr/bin/env python3
"""Offline structural checks, not a replacement for Wokwi simulation."""
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
data = json.loads((ROOT / "diagram.json").read_text(encoding="utf-8"))
assert data.get("version") == 1, "Wokwi diagram version must be 1"
parts = data.get("parts", [])
connections = data.get("connections", [])
ids = [part.get("id") for part in parts]
assert len(ids) == len(set(ids)), "Part IDs must be unique"
electrical = [part for part in parts if part.get("type") != "wokwi-text"]
labels = [part for part in parts if part.get("type") == "wokwi-text"]
assert len(electrical) == 3, "Expected exactly three electrical parts"
assert {part.get("type") for part in electrical} == {
    "wokwi-esp32-devkit-v1", "wokwi-potentiometer", "wokwi-servo"
}, "Expected ESP32, potentiometer and servo"
assert all(label.get("attrs", {}).get("text") for label in labels), "Labels need text"
assert all(len(wire) >= 3 for wire in connections), "Each wire needs source, target and color"
normalized = {(a, b) for a, b, *_ in connections}
normalized |= {(b, a) for a, b, *_ in connections}
required = {
    ("emg_mock:SIG", "esp:D34"),
    ("servo1:PWM", "esp:D18"),
    ("emg_mock:VCC", "esp:3V3"),
    ("servo1:V+", "esp:VIN"),
    ("emg_mock:GND", "esp:GND.2"),
    ("servo1:GND", "esp:GND.1"),
}
missing = required - normalized
assert not missing, f"Missing required Wokwi connections: {sorted(missing)}"
assert len(connections) == 6, "Expected exactly six electrical wires"
print(f"Wokwi structure passed ({len(electrical)} electrical parts, {len(labels)} labels, {len(connections)} wires).")
