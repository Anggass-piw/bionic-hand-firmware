# Simulasi rangkaian di Wokwi

Folder proyek memakai `diagram.json` dan `wokwi.toml` di root sesuai format Wokwi. Diagram berisi ESP32 DevKit V1, potensiometer sebagai **mock** keluaran analog EMG, dan satu micro servo.

## Wiring simulasi

| Komponen | Pin komponen | Pin ESP32 Wokwi |
|---|---|---|
| Potensiometer (mock EMG) | VCC | 3V3 |
| Potensiometer (mock EMG) | GND | GND.1 |
| Potensiometer (mock EMG) | SIG | GPIO34 / ADC1 |
| Servo | PWM | GPIO18 |
| Servo | V+ | 5V simulasi |
| Servo | GND | GND.2 |

## Jalankan

1. Build firmware PlatformIO dari root proyek dengan `pio run`.
2. Buka folder root di VS Code dan pasang ekstensi Wokwi for VS Code bila belum ada.
3. Jika ekstensi meminta lisensi, ikuti alur aktivasi lisensi Wokwi.
4. Pilih **Wokwi: Start Simulator** dari Command Palette (F1).
5. Di Serial Monitor, kirim `r` saat potensiometer di posisi rendah/rileks selama 3 detik.
6. Putar potensiometer ke posisi tinggi dan kirim `c` selama pengukuran 3 detik; sesudah kalibrasi berhasil, kirim `a`.
7. Putar kenop potensiometer untuk menguji perubahan sudut servo. Gunakan `o` dan `m` hanya untuk uji gerak tanpa beban.

Diagram sudah diperiksa sebagai JSON, dengan tiga komponen dan enam koneksi utama. Simulasi Wokwi cloud belum dijalankan di komputer ini; Wokwi CLI membutuhkan token proyek/akun. Setelah lisensi ekstensi tersedia, jalankan simulasi langsung dari VS Code.

## Perbedaan simulator dan rangkaian nyata

Potensiometer hanya memberi nilai analog yang dapat diputar; ia **tidak** merekam EMG atau meniru elektroda, noise, penguatan, dan dinamika otot. Wokwi juga tidak membuktikan arus stall, tegangan drop, kekuatan tendon, gaya genggam, atau keselamatan gerakan. Pin servo 5V pada diagram adalah sumber virtual agar simulasi berjalan; pada perangkat nyata, servo wajib memakai catu 5–6V terpisah yang mampu menanggung arus stall servo. Satukan ground catu servo dengan ground ESP32/sensor, tetapi jangan menyalakan servo MG996R dari pin 5V/3V3 board atau USB.

## Referensi format/pin

- [Wokwi diagram.json format](https://docs.wokwi.com/diagram-format)
- [Potentiometer component](https://docs.wokwi.com/parts/wokwi-potentiometer)
- [Servo component](https://docs.wokwi.com/parts/wokwi-servo)
- [Wokwi PlatformIO configuration](https://docs.wokwi.com/vscode/project-config)
