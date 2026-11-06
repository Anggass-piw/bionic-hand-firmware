# Alur pemrosesan EMG dan batas integrasi perangkat keras

Dokumen ini membedakan bagian yang sudah ada dalam prototipe dari rancangan perangkat keras yang masih perlu dibuat dan divalidasi. Repo saat ini tetap merupakan demonstrator edukasional satu-servo; diagram Wokwi memakai potensiometer sebagai input mock.

## Aliran data

```text
Sumber analog yang sudah dikondisikan
  → ADC ESP32 GPIO34
  → baseline rileks dikurangkan: x[n] = ADC[n] − baseline
  → rectification: |x[n]|
  → moving window 25 sampel
  → MAV dan RMS
  → kalibrasi threshold + hysteresis
  → target sudut servo bertahap
```

Firmware menargetkan sampling 500 Hz (`kSampleIntervalMs = 2`), sehingga jendela 25 sampel kira-kira mencakup 50 ms. Karena loop juga mengerjakan serial dan servo, ini adalah target nominal, bukan jaminan waktu sampling keras. Nilai ADC, MAV, RMS, fitur kontrol, threshold, status kontraksi, dan posisi servo dicetak tiap 100 ms.

Untuk sampel terpusat `x[n]`, fitur dihitung sebagai:

- **MAV** = `mean(|x[n]|)` — fitur kontrol bawaan, menjaga perilaku prototipe sebelumnya.
- **RMS** = `sqrt(mean(x[n]²))` — dihitung bersamaan dengan MAV.

Keduanya memakai jendela yang sama. Ubah `config::kUseRmsFeature` di `include/config.h` menjadi `true` untuk memakai RMS sebagai masukan kalibrasi dan kontrol. Setelah mengubah fitur atau sumber sensor, ulangi kalibrasi baseline rileks dengan `r`, kalibrasi kontraksi dengan `c`, lalu aktifkan otomatis dengan `a`. Nilai ambang yang tersimpan bergantung pada fitur dan rangkaian; angka bawaan bukan nilai klinis atau nilai universal.

## Apa yang sudah dan belum tersedia

- **Sudah ada:** pembacaan ADC, pengukuran baseline, rectification, moving average, MAV/RMS, kalibrasi, hysteresis, pemetaan besaran sinyal ke sudut servo, dan gerak servo bertahap.
- **Sudah ada untuk uji perangkat lunak:** potensiometer Wokwi sebagai mock analog, tes C++ native, dan pemantau telemetry serial Node.js.
- **Belum ada:** rangkaian elektroda/front-end analog, op-amp/instrumentation amplifier, LM393, MOSFET driver, penggerak motor DC, atau simulasi Tinkercad untuk bagian-bagian tersebut.
- **CAD:** sekarang ada pod elektronik generik dengan lubang pemasangan dan slot kabel, tetapi dimensinya belum dicocokkan dengan PCB/komponen tertentu dan belum divalidasi secara mekanik maupun elektrik.

## Catatan penting untuk rancangan analog

Firmware membaca **nilai analog yang mempertahankan variasi amplitudo** di GPIO34. Keluaran window comparator LM393 umumnya berupa keputusan digital high/low (dan bersifat open-collector), bukan bentuk gelombang amplitudo. Jika keluaran comparator langsung dipakai sebagai masukan ADC, informasi amplitudo untuk MAV/RMS hilang; gunakan keluaran analog yang telah dikondisikan untuk jalur fitur ini. Comparator dapat dirancang sebagai jalur trigger terpisah, tetapi interfacenya perlu dirancang dan divalidasi tersendiri. Lihat [peta interface hardware dan rencana validasi](hardware-interface.md).

Sinyal biologis tidak boleh dihubungkan langsung ke ESP32. Rancangan front-end, pembatasan tegangan, bias, filtering, isolasi, dan proteksi harus ditinjau orang yang kompeten sebelum pengujian. GPIO ESP32 bukan input 5 V; jangan biarkan tegangan ADC melampaui batas board. Diagram Wokwi sekarang hanya menguji jalur potensiometer 0–3,3 V dan tidak memvalidasi rangkaian analog maupun keamanan listrik.

## Aktuator dan model mekanik

Prototipe ini mengendalikan **servo** lewat sinyal PWM; servo bukan motor DC telanjang, sehingga firmware ini tidak memakai MOSFET untuk mengalirkan arus motor. Gunakan catu servo terpisah sesuai datasheet dan GND bersama seperti dijelaskan di [panduan Wokwi](../wokwi/README.md). Jika kelak memakai motor DC, diperlukan desain driver yang sesuai (misalnya pengendalian dua arah dengan H-bridge), proteksi, catu, serta pengujian termal dan arus—itu bukan perubahan yang dapat dilakukan hanya dengan mengganti nama pin.

CAD `mechanical/hand_prototype.scad` adalah demonstrator meja kerja tendon satu-servo dengan pod elektronik generik di bawah pergelangan. Pod belum disesuaikan dengan PCB, konektor, komponen, pendinginan, strain relief, atau toleransi tertentu dan belum diuji kekuatan, beban, atau keselamatan. Ia bukan socket tubuh atau desain elektronik tervalidasi.

## Pemantau serial Node.js

Pasang dependensi dari direktori proyek, lalu lihat port serial yang tersedia:

```bash
npm install
npm test
npm run serial-monitor -- --list-ports
```

Mulai monitor pada port perangkat (`/dev/ttyUSB0`, `/dev/ttyACM0`, atau `COM3` sebagai contoh); baud bawaan 115200:

```bash
npm run serial-monitor -- --port /dev/ttyUSB0
# Windows: npm run serial-monitor -- --port COM3
```

Baud dapat diubah dengan `--baud 115200`. Monitor menampilkan baris telemetry key-value, tidak mengirim perintah kalibrasi, dan dihentikan dengan Ctrl-C. `npm test` menguji parser tanpa board atau port serial.

## Batas penggunaan

Proyek ini untuk pembelajaran dan pengujian meja kerja, bukan perangkat medis atau prostesis yang siap dipakai. Jangan menghubungkan elektroda ke manusia atau memasang mekanisme pada tubuh. Potensiometer mock, tes native, dan model CAD tidak membuktikan akurasi EMG, keselamatan listrik, gaya genggam, ketahanan mekanik, atau kesesuaian klinis. Validasi dan tinjauan profesional terpisah dibutuhkan sebelum setiap penggunaan pada manusia.
