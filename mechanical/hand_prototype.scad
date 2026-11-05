// Parametric educational prosthetic-hand demonstrator, not a medical device.
// Set `part` in OpenSCAD Customizer or with: -D 'part="palm"'
part = "assembly"; // [assembly, palm, servo_bracket, finger_29, finger_23, finger_18, thumb_26, thumb_20, thumb_16, print_layout]

$fn = 40;

// Nominal dimensions in millimeters; tune to the chosen hardware and printer.
palm_width = 78;
palm_length = 74;
palm_thickness = 12;
palm_radius = 9;

servo_width = 41;     // MG996R-class body; adjust for SG90 or another servo
servo_depth = 22;
servo_height = 42;
servo_wall = 2.4;
servo_tab = 5;
servo_floor = 4;

pin_diameter = 3.4;   // M3 clearance; verify with the actual printer
line_diameter = 2.6;  // fishing-line/tendon channel; verify line diameter

module rounded_plate_2d(width, length, radius) {
  hull() {
    for (x = [-width / 2 + radius, width / 2 - radius])
      for (y = [radius, length - radius])
        translate([x, y]) circle(r = radius);
  }
}

module palm() {
  difference() {
    union() {
      linear_extrude(height = palm_thickness)
        rounded_plate_2d(palm_width, palm_length, palm_radius);
      translate([0, -34, 0])
        linear_extrude(height = palm_thickness)
          rounded_plate_2d(52, 38, 8);
    }

    // Base hinge clearance for four fingers and the angled thumb.
    for (x = [-28, -10, 10, 28])
      translate([x, 69, palm_thickness / 2])
        rotate([0, 90, 0]) cylinder(d = pin_diameter, h = 18, center = true);
    translate([-32, 38, palm_thickness / 2])
      rotate([0, 90, 0]) cylinder(d = pin_diameter, h = 16, center = true);

    // Mounting holes for the servo bracket.
    for (x = [-25, 25])
      translate([x, -18, -0.1]) cylinder(d = pin_diameter, h = palm_thickness + 0.2);

    // Tendon/elastic anchor holes near the wrist end.
    for (x = [-18, -9, 0, 9, 18])
      translate([x, -27, palm_thickness / 2])
        rotate([90, 0, 0]) cylinder(d = 2.8, h = 12, center = true);
  }
}

// Each link has a pin bore at both ends and an offset longitudinal tendon path.
module finger_link(length = 29, width = 14, thickness = 8) {
  end_offset = length / 2 - width / 2;
  difference() {
    linear_extrude(height = thickness, center = true)
      hull() {
        translate([0, -end_offset]) circle(d = width);
        translate([0, end_offset]) circle(d = width);
      }

    for (y = [-end_offset, end_offset])
      translate([0, y, 0])
        rotate([0, 90, 0]) cylinder(d = pin_diameter, h = width + 1, center = true);

    translate([0, 0, thickness * 0.25])
      rotate([90, 0, 0]) cylinder(d = line_diameter, h = length + 1, center = true);
  }
}

module servo_bracket() {
  outer_width = servo_width + 2 * servo_wall + 2 * servo_tab;
  outer_depth = servo_depth + 2 * servo_wall;
  hole_x = servo_width / 2 + servo_wall + servo_tab / 2;

  difference() {
    union() {
      translate([0, 0, servo_floor / 2])
        cube([outer_width, outer_depth, servo_floor], center = true);
      for (x = [-servo_width / 2 - servo_wall / 2,
                 servo_width / 2 + servo_wall / 2])
        translate([x, 0, servo_floor + servo_height / 2])
          cube([servo_wall, outer_depth, servo_height], center = true);
    }

    // Open-top pocket: the servo drops in from above.
    translate([0, 0, servo_floor + 0.5 + servo_height / 2])
      cube([servo_width + 0.4, servo_depth + 0.4, servo_height + 1], center = true);

    for (x = [-hole_x, hole_x])
      translate([x, 0, servo_floor / 2])
        cylinder(d = pin_diameter, h = servo_floor + 1, center = true);
  }
}

module finger_chain(lengths = [29, 23, 18], widths = [14, 12, 10],
                    bends = [12, 18]) {
  d1 = lengths[0] / 2 - widths[0] / 2;
  d2 = lengths[1] / 2 - widths[1] / 2;
  d3 = lengths[2] / 2 - widths[2] / 2;

  // The proximal pin center is at the local origin. Each bend pivots about X.
  translate([0, d1, 0]) {
    finger_link(lengths[0], widths[0], 8);
    translate([0, 2 * d1, 0]) rotate([bends[0], 0, 0]) {
      translate([0, d2, 0]) {
        finger_link(lengths[1], widths[1], 7);
        translate([0, 2 * d2, 0]) rotate([bends[1], 0, 0])
          translate([0, d3, 0]) finger_link(lengths[2], widths[2], 6);
      }
    }
  }
}

module assembly_preview() {
  color("slategray") palm();

  // Four long fingers, with small display-only bends at the joints.
  for (x = [-28, -10, 10, 28])
    color("steelblue")
      translate([x, 69, palm_thickness])
        finger_chain([29, 23, 18], [14, 12, 10], [10, 16]);

  // Opposing thumb, rotated outward from the palm.
  color("royalblue")
    translate([-32, 38, palm_thickness]) rotate([0, 0, -45])
      finger_chain([26, 20, 16], [13, 11, 9], [18, 24]);

  // Visual placeholder for a standard servo and its printable bracket.
  color("dimgray") translate([0, -18, palm_thickness]) servo_bracket();
  color("silver")
    translate([0, -18, palm_thickness + servo_floor + servo_height / 2])
      cube([servo_width, servo_depth, servo_height], center = true);
  color("orange")
    translate([0, -18, palm_thickness + servo_floor + servo_height + 1.5])
      cylinder(d = 14, h = 3, center = true);
}

module print_layout() {
  // Five rows of three replaceable links, plus palm and servo bracket.
  for (row = [0:4]) {
    lengths = row < 4 ? [29, 23, 18] : [26, 20, 16];
    widths = row < 4 ? [14, 12, 10] : [13, 11, 9];
    thicknesses = [8, 7, 6];
    for (col = [0:2])
      translate([(col - 1) * 34, row * 34, thicknesses[col] / 2])
        finger_link(lengths[col], widths[col], thicknesses[col]);
  }
  translate([-92, 20, 0]) palm();
  translate([92, -18, 0]) servo_bracket();
}

if (part == "assembly") {
  assembly_preview();
} else if (part == "palm") {
  palm();
} else if (part == "servo_bracket") {
  servo_bracket();
} else if (part == "finger_29") {
  finger_link(29, 14, 8);
} else if (part == "finger_23") {
  finger_link(23, 12, 7);
} else if (part == "finger_18") {
  finger_link(18, 10, 6);
} else if (part == "thumb_26") {
  finger_link(26, 13, 8);
} else if (part == "thumb_20") {
  finger_link(20, 11, 7);
} else if (part == "thumb_16") {
  finger_link(16, 9, 6);
} else if (part == "print_layout") {
  print_layout();
}
