#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="$(mktemp -d "${TMPDIR:-/tmp}/bionic-hand-native-test.XXXXXX")"
trap 'rm -rf "$BUILD_DIR"' EXIT
g++ -std=c++11 -Wall -Wextra -Werror \
  -I"$ROOT/include" \
  "$ROOT/test/native/logic_test.cpp" \
  "$ROOT/src/emg_filter.cpp" \
  "$ROOT/src/signal_logic.cpp" \
  -o "$BUILD_DIR/logic_test"
"$BUILD_DIR/logic_test"
