# Prototipe Tangan Prostetik Myoelektrik — ESP32

Proyek edukasional untuk menguji alur sinyal EMG → filter → target servo pada ESP32. Isinya mencakup firmware PlatformIO, model CAD parametris OpenSCAD yang bisa diedit, STL untuk prototipe meja kerja, dan diagram rangkaian Wokwi. Ini **bukan perangkat medis/prostesis siap pakai**.

## Isi proyek

- `src/` dan `include/` — firmware C++: baseline rileks, kalibrasi kontraksi, rectification + moving-average, threshold/hysteresis, dan gerak servo bertahap.
- `platformio.ini` — build untuk ESP32 DevKit (`esp32dev`) dan library `ESP32Servo`.
- `mechanical/hand_prototype.scad` — file sumber CAD OpenSCAD; bisa dibuka, diedit, dan diubah parameternya.
- `mechanical/exports/*.stl` — telapak, bracket servo, link jari/ibu jari, dan layout cetak.
- `mechanical/exports/openscad-application.png` — screenshot langsung dari OpenSCAD yang memperlihatkan source, parameter Customizer, dan model 3D; ini dokumentasi aplikasi CAD, bukan gambar AI.
- `diagram.json` + `wokwi.toml` — ESP32, potensiometer sebagai mock sensor EMG, dan servo untuk simulasi.
- `test/` — tes logika C++ native dan pemeriksaan struktur diagram Wokwi.

## Build dan tes firmware

Build untuk ESP32:

```bash
pio run
```

Tes lokal tanpa board:

```bash
./test/run_native_tests.sh
python3 test/validate_wokwi_diagram.py
```

Di VS Code, buka folder proyek, gunakan PlatformIO IDE, lalu pilih **PlatformIO: Build**. Untuk upload/monitor hardware gunakan board dan port yang benar; firmware memakai Serial Monitor 115200 baud.

## Kalibrasi firmware

Kirim karakter melalui Serial Monitor:

- `r` — ukur baseline 3 detik saat otot rileks.
- `c` — ukur kontraksi 3 detik setelah baseline berhasil.
- `a` — aktifkan kontrol otomatis setelah dua kalibrasi berhasil.
- `o` / `m` — perintah manual untuk buka / sudut genggam uji tanpa beban.
- `?` — bantuan.

Pin, threshold, rentang sudut, dan waktu halus ada di `include/config.h`. Sudut bawaan hanya titik awal; sesuaikan mekanisme secara bertahap di meja kerja.

## Model CAD 3D

Buka `mechanical/hand_prototype.scad` di aplikasi OpenSCAD untuk mengedit model. Atur pilihan `part` dan dimensi lewat Customizer, atau langsung ubah parameter di file. Ekspor ulang STL dan preview dengan:

```bash
./mechanical/export_parts.sh
```

![OpenSCAD: source CAD, parameter Customizer, dan model 3D](mechanical/exports/openscad-application.png)

Petunjuk dimensi, cetak, dan perakitan ada di `mechanical/README.md`. Bentuknya demonstrator satu-servo dengan tendon dan return elastis, bukan socket yang dipasangkan ke tubuh.

## Rangkaian Wokwi

Diagram dan firmware mapping ada di `diagram.json` dan `wokwi.toml`. Build dahulu dengan `pio run`, lalu di VS Code pilih **Wokwi: Start Simulator** melalui Command Palette (F1). Petunjuk lengkap dan tabel pin ada di `wokwi/README.md`.

Potensiometer di simulator hanya meniru nilai analog yang dapat diputar; ia tidak merekam atau meniru fisiologi EMG, elektroda, noise, arus motor, beban tendon, atau gaya genggam. Rangkaian fisik wajib memakai keluaran sensor yang sudah dikondisikan dan aman untuk ADC.

## Keselamatan

- GPIO34 menerima input analog; jangan sambungkan elektroda mentah atau tegangan di atas batas ADC ESP32.
- Servo perlu catu 5–6 V terpisah yang mampu menangani arus stall servo; satukan GND catu servo, sensor, dan ESP32. Jangan menyalakan servo dari pin 3V3/5V board atau USB pada perangkat nyata.
- Uji mekanisme tanpa beban, gunakan pemutus daya yang mudah dijangkau, dan hindari pinch point. Input sensor terlepas/mengambang belum dideteksi firmware dan dapat menghasilkan gerakan tak terduga.
- Model CAD belum diuji kekuatan, ukuran, kenyamanan, atau keselamatan. Jangan dipasang pada tubuh maupun dipakai sebagai perangkat bantu/medis tanpa tinjauan profesional dan validasi keselamatan.
