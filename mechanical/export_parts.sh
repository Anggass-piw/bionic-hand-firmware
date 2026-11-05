#!/usr/bin/env bash
set -euo pipefail
HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SCAD="$HERE/hand_prototype.scad"
OUT="$HERE/exports"
mkdir -p "$OUT"

export_stl() {
  local name="$1"
  local selection="$2"
  echo "Exporting $name.stl ($selection)"
  openscad -o "$OUT/$name.stl" -D "part=\"$selection\"" "$SCAD"
}

export_stl palm palm
export_stl servo_bracket servo_bracket
export_stl finger_link_29mm finger_29
export_stl finger_link_23mm finger_23
export_stl finger_link_18mm finger_18
export_stl thumb_link_26mm thumb_26
export_stl thumb_link_20mm thumb_20
export_stl thumb_link_16mm thumb_16
export_stl print_layout print_layout

echo "Rendering assembly preview"
openscad -o "$OUT/hand-preview.png" \
  --imgsize=1280,900 --viewall --autocenter \
  --camera=0,20,10,68,0,320,245 \
  "$SCAD"
echo "Done. STL and preview files are in $OUT"
