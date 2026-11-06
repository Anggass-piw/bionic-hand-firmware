# Prototipe CAD 3D edukasional

File `hand_prototype.scad` adalah **sumber CAD OpenSCAD yang dapat diedit**, bukan gambar hasil generatif. Parameternya bisa diubah langsung di kode atau melalui Customizer OpenSCAD. Screenshot aplikasi memperlihatkan source, parameter model, serta viewport 3D secara bersamaan.

## Berkas CAD

- `hand_prototype.scad` — sumber parametris untuk telapak, link jari berporos, kanal tendon, dudukan servo, dan pod elektronik generik.
- `exports/electronics_base.stl` dan `exports/electronics_lid.stl` — enclosure elektronik generik yang bisa diubah ukurannya; bukan ukuran PCB tertentu.
- `exports/openscad-application.png` — dokumentasi screenshot dari aplikasi OpenSCAD.
- `exports/hand-preview.png` — render preview yang dibuat dari model OpenSCAD.
- `exports/*.stl` — file mesh hasil ekspor untuk slicer/cetak 3D.
- `export_parts.sh` — skrip untuk mengekspor ulang seluruh STL dan preview.

Buka `hand_prototype.scad` di OpenSCAD dan gunakan Customizer `part`, atau dari root proyek jalankan `bash mechanical/export_parts.sh`. Pilih `electronics_enclosure` untuk preview pod tertutup, atau `electronics_base` / `electronics_lid` untuk melihat tiap bagian. Ubah parameter `electronics_inner_*`, dinding, celah lid, lubang pemasangan, dan slot kabel di bagian atas file. Dimensi pod hanya placeholder generik: ukur PCB, konektor, tinggi komponen, strain relief, toleransi printer, dan bahan sebelum mencetak atau memasangnya. Slot kabel bukan gland atau perlindungan tarikan.

## Rakit awal (meja kerja saja)

1. Cetak telapak, bracket, dan link dengan orientasi datar; gunakan layer 0,2 mm dan infill sekitar 30% sebagai titik awal, lalu uji kekuatan bagian kecil terlebih dahulu.
2. Hubungkan link dengan pin/baut M3 dan mur pengunci yang cocok. Kanal tendon 2,6 mm adalah perkiraan; sesuaikan dengan diameter tali pancing yang dipakai.
3. Lewatkan tali melalui kanal jari dan hubungkan ke horn/spool servo. Tambahkan tali elastis sebagai return tendon agar jari membuka saat kabel ditarik balik. Jalur tali bersama dan gesekannya perlu disetel secara mekanik.
4. Dudukan default dibuat untuk servo kelas MG996R. Untuk SG90, ubah `servo_width`, `servo_depth`, `servo_height`, dan posisi pemasangan di CAD—jangan memaksa servo masuk.
5. Uji gerakan dengan linkage tanpa beban, rentang sudut rendah, dan pemutus daya yang mudah dijangkau. Periksa pin, retak, pinch point, dan temperatur servo.

## Batas penggunaan

Model ini tidak memperhitungkan anatomi, socket, ergonomi, beban genggam, kelelahan material, toleransi antarmesin, enclosure kedap air, pendinginan PCB, atau batas gaya. Pod elektronik belum divalidasi terhadap papan, komponen, motor, debu, cairan, maupun benturan. Tidak ada bukti bahwa model ini aman atau cocok dipakai seseorang. Jangan memasangnya pada tubuh atau menggunakannya sebagai perangkat bantu/medis; perlu tinjauan mekanik dan klinis profesional serta validasi terpisah sebelum penggunaan apa pun pada manusia.
