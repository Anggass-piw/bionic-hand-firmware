#!/usr/bin/env node
'use strict';

const { SerialPort } = require('serialport');
const { readdir } = require('node:fs/promises');
const { parseTelemetryLine } = require('./telemetry-parser');

const DEFAULT_BAUD_RATE = 115200;

function usage() {
  console.log(`Pemantau telemetry serial firmware

Pemakaian:
  node tools/serial-monitor.js --port <port> [--baud <rate>]
  node tools/serial-monitor.js --list-ports

Opsi:
  -p, --port <port>     Port serial, misalnya /dev/ttyACM0 atau COM3
  -b, --baud <rate>     Baud rate positif (default: ${DEFAULT_BAUD_RATE})
      --list-ports      Tampilkan port serial yang terdeteksi
  -h, --help            Tampilkan bantuan ini`);
}

function parseArguments(args) {
  const options = { baudRate: DEFAULT_BAUD_RATE, port: null, listPorts: false, help: false };
  for (let i = 0; i < args.length; i += 1) {
    const arg = args[i];
    if (arg === '--help' || arg === '-h') options.help = true;
    else if (arg === '--list-ports') options.listPorts = true;
    else if (arg === '--port' || arg === '-p') {
      if (!args[i + 1] || args[i + 1].startsWith('-')) throw new Error('Opsi --port memerlukan nama/path port.');
      options.port = args[++i];
    } else if (arg === '--baud' || arg === '--baud-rate' || arg === '-b') {
      const raw = args[++i];
      if (!raw || raw.startsWith('-') || !/^\d+$/.test(raw)) throw new Error('Baud rate harus berupa bilangan bulat positif.');
      const baudRate = Number(raw);
      if (!Number.isSafeInteger(baudRate) || baudRate < 1) throw new Error('Baud rate harus berupa bilangan bulat positif.');
      options.baudRate = baudRate;
    } else {
      throw new Error(`Opsi tidak dikenal: ${arg}`);
    }
  }
  return options;
}

function displayValue(record, key) {
  if (!Object.prototype.hasOwnProperty.call(record, key)) return '—';
  if (record[key] === null) return 'tidak valid';
  if (key === 'active') return record[key] ? 'aktif' : 'nonaktif';
  return String(record[key]);
}

function printTelemetry(line) {
  const record = parseTelemetryLine(line);
  if (Object.keys(record).length === 0) return;
  const status = Object.prototype.hasOwnProperty.call(record, 'status')
    ? displayValue(record, 'status')
    : displayValue(record, 'active');
  console.log(
    `ADC: ${displayValue(record, 'adc')} | MAV: ${displayValue(record, 'mav')} | ` +
    `RMS: ${displayValue(record, 'rms')} | Threshold: ${displayValue(record, 'threshold')} | ` +
    `Status: ${status}`
  );
  const extras = ['mode', 'feature', 'target', 'current']
    .filter((key) => Object.prototype.hasOwnProperty.call(record, key))
    .map((key) => `${key}: ${displayValue(record, key)}`);
  if (extras.length) console.log(`  ${extras.join(' | ')}`);
}

async function listPorts() {
  let ports;
  try {
    ports = await SerialPort.list();
  } catch (error) {
    if (process.platform !== 'linux' || error.code !== 'ENOENT' || !String(error.message).includes('udevadm')) {
      throw error;
    }
    const devices = await readdir('/dev');
    ports = devices
      .filter((name) => /^(ttyUSB|ttyACM|ttyS)\d+$/.test(name))
      .map((name) => ({ path: `/dev/${name}` }));
  }
  if (ports.length === 0) {
    console.log('Tidak ada port serial yang terdeteksi.');
    return;
  }
  console.log('Port serial yang terdeteksi:');
  for (const port of ports) {
    const description = [port.manufacturer, port.serialNumber && `SN ${port.serialNumber}`]
      .filter(Boolean).join(', ');
    console.log(`  ${port.path}${description ? ` — ${description}` : ''}`);
  }
}

function monitor(portPath, baudRate) {
  const port = new SerialPort({ path: portPath, baudRate, autoOpen: false });
  let closing = false;
  let lineBuffer = '';

  const finish = () => {
    if (closing) return;
    closing = true;
    if (port.isOpen) {
      port.close((error) => {
        if (error) console.error(`Gagal menutup port serial: ${error.message}`);
      });
    }
  };

  process.once('SIGINT', () => {
    console.log('\nDihentikan oleh pengguna; menutup port serial...');
    finish();
  });

  port.on('error', (error) => {
    console.error(`Kesalahan port serial (${portPath}): ${error.message}`);
    if (!port.isOpen) process.exitCode = 1;
    else finish();
  });

  port.on('data', (chunk) => {
    lineBuffer += chunk.toString('utf8');
    const lines = lineBuffer.split(/\r?\n/);
    lineBuffer = lines.pop();
    for (const line of lines) printTelemetry(line.trim());
  });

  port.open((error) => {
    if (error) {
      console.error(`Tidak dapat membuka port "${portPath}": ${error.message}`);
      process.exitCode = 1;
      return;
    }
    if (closing) {
      port.close((closeError) => {
        if (closeError) console.error(`Gagal menutup port serial: ${closeError.message}`);
      });
      return;
    }
    console.log(`Terhubung ke ${portPath} pada ${baudRate} baud. Tekan Ctrl-C untuk berhenti.`);
  });
}

async function main() {
  let options;
  try {
    options = parseArguments(process.argv.slice(2));
  } catch (error) {
    console.error(`Kesalahan argumen: ${error.message}`);
    usage();
    process.exitCode = 2;
    return;
  }
  if (options.help) {
    usage();
    return;
  }
  if (options.listPorts) {
    try {
      await listPorts();
    } catch (error) {
      console.error(`Gagal membaca daftar port serial: ${error.message}`);
      process.exitCode = 1;
    }
    return;
  }
  if (!options.port) {
    console.error('Pilih port dengan --port <port>, atau lihat port yang tersedia dengan --list-ports.');
    process.exitCode = 2;
    return;
  }
  monitor(options.port, options.baudRate);
}

if (require.main === module) main();

module.exports = { parseArguments, printTelemetry };
