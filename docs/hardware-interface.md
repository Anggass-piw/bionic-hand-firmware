# Antarmuka perangkat keras: rancangan konseptual

Dokumen ini menerjemahkan spesifikasi menjadi pembagian blok dan sinyal yang dapat ditinjau sebelum membuat rangkaian. Ini **bukan skema siap-rakit**, simulasi Tinkercad, atau validasi keselamatan. Repo belum berisi rangkaian elektroda, op-amp, LM393, maupun driver MOSFET.

## Arsitektur sinyal

```text
Sensor / elektroda
  → front-end EMG berisolasi dan terlindungi
  → penguatan, bias, dan filtering analog yang sesuai
  → keluaran analog dengan amplitudo terjaga dalam rentang ADC
  → ESP32 GPIO34: baseline, |x[n]|, MAV/RMS, threshold/hysteresis
  → ESP32 GPIO18: sinyal PWM servo

Jalur opsional terpisah: envelope analog → LM393 window comparator
  → keluaran digital dengan pull-up 3,3 V → pin digital yang dipilih setelah uji
```

Firmware saat ini mengambil **jalur analog amplitude-preserving** di GPIO34. Comparator LM393 biasanya hanya memberi high/low (open collector), sehingga ia tidak membawa informasi amplitudo yang dibutuhkan MAV/RMS. Jangan menyambungkan keluaran comparator langsung ke ADC sebagai pengganti bentuk gelombang analog. Bila kedua jalur dipakai, tentukan dan uji pin digital serta logika firmware secara terpisah; repo belum mengimplementasikannya.

## Peta pin dan daya prototipe saat ini

| Sinyal | Koneksi prototipe | Catatan |
|---|---|---|
| Input analog terkondisi | ESP32 GPIO34 / ADC1 | Input analog harus dibatasi sesuai spesifikasi board dan tidak melebihi 3,3 V; jangan hubungkan elektroda mentah. |
| PWM servo | ESP32 GPIO18 | Sinyal kontrol berlogika 3,3 V. Servo memakai suplai terpisah sesuai datasheet dan GND bersama; jangan mengambil arus servo dari pin board/USB. |
| Input comparator | Belum dipetakan | LM393 open-collector perlu pull-up yang benar; desain pull-up dan pin digital harus ditinjau, bukan dianggap bagian firmware saat ini. |
| Motor DC + driver MOSFET | Tidak ada pada prototipe | Model dan firmware sekarang mengendalikan servo, bukan motor DC. Motor DC memerlukan driver yang sesuai arus/tegangan dan proteksi; untuk arah dua arah gunakan topologi H-bridge yang dirancang dan diuji. |

Diagram Wokwi yang ada hanya memakai potensiometer sebagai mock 0–3,3 V dan satu servo. Ia bukan model elektroda, penguat, LM393, suplai motor, atau MOSFET.

## Enclosure elektronik di CAD

OpenSCAD sekarang menyediakan pilihan `electronics_base`, `electronics_lid`, dan `electronics_enclosure`. Enclosure adalah pod generik di bawah pergelangan pada preview tangan, dengan lubang pemasangan dan dua slot kabel untuk menunjukkan konsep routing. Ubah parameter `electronics_inner_*`, `electronics_wall`, `electronics_floor`, dan celah lid setelah ukuran papan, konektor, dan komponen diketahui.

Belum ada ukuran PCB atau komponen yang ditentukan. Enclosure tidak menjamin jarak listrik, pendinginan, penahan kabel, perlindungan debu/air, atau kekuatan pemasangan; jangan anggap pod sudah cocok untuk papan MOSFET/LM393 tertentu. Periksa ruang servo, bagian bergerak, konektor, radius tekuk kabel, dan akses pemutus daya sebelum mencetak.

## Rencana validasi sebelum hardware nyata

1. Tetapkan sensor/front-end yang terdokumentasi, catu, isolasi, perlindungan, dan batas output sebelum memilih nilai komponen.
2. Ukur output analog terhadap ground referensi pada rentang kondisi kerja yang disetujui; pastikan selalu dalam rentang ADC board. Jangan uji dengan elektroda pada manusia.
3. Jika comparator dipakai, uji sebagai jalur digital terpisah, termasuk pull-up 3,3 V, batas bawah/atas, polaritas output, dan keadaan input terlepas.
4. Uji catu servo dan sinyal PWM terpisah dari jalur analog; periksa drop tegangan, arus stall sesuai datasheet, gerakan tanpa beban, dan pemutusan daya.
5. Jika kelak mengganti servo dengan motor DC, redesain driver dan firmware; validasi rating transistor/driver, flyback/proteksi, arus, temperatur, dan mekanisme secara mandiri.
6. Setelah ukuran komponen tersedia, sesuaikan pod CAD, cetak satu enclosure uji, cek clearance dan routing kabel, lalu lakukan uji mekanik meja kerja.

Tidak ada skema Tinkercad maupun validasi perangkat keras yang disertakan pada perubahan ini. Sistem tetap demonstrator meja kerja, bukan perangkat medis/prostesis siap pakai.