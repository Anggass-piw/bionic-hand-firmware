'use strict';

const test = require('node:test');
const assert = require('node:assert/strict');
const { parseTelemetryLine } = require('../../tools/telemetry-parser');

test('mem-parsing field firmware valid yang dipisahkan koma', () => {
  assert.deepEqual(
    parseTelemetryLine('adc=1234, mav=0.81, rms=12.5, feature=4, mode=MAV, threshold=10, active=1, target=20, current=18'),
    {
      adc: 1234,
      mav: 0.81,
      rms: 12.5,
      feature: 4,
      mode: 'MAV',
      threshold: 10,
      active: true,
      target: 20,
      current: 18,
    }
  );
});

test('menerima pasangan dipisahkan spasi dan status teks', () => {
  assert.deepEqual(
    parseTelemetryLine('ADC=-2.5e1 mav=.5 status=siap active=off'),
    { adc: -25, mav: 0.5, status: 'siap', active: false }
  );
});

test('nilai numerik invalid tidak dikonversi menjadi angka', () => {
  const parsed = parseTelemetryLine('adc=NaN mav=Infinity rms=12x threshold= target=0x10');
  assert.deepEqual(parsed, {
    adc: null,
    mav: null,
    rms: null,
    threshold: null,
    target: null,
  });
  for (const value of Object.values(parsed)) assert.notEqual(typeof value, 'number');
});

test('menerima telemetry parsial tanpa mengarang field yang tidak dikirim', () => {
  assert.deepEqual(parseTelemetryLine('adc=42 current=invalid'), {
    adc: 42,
    current: null,
  });
});

test('field opsional dan input non-string tidak menyebabkan kegagalan', () => {
  assert.deepEqual(parseTelemetryLine('feature=3 status= active=maybe'), {
    feature: 3,
    status: null,
    active: null,
  });
  assert.deepEqual(parseTelemetryLine(null), {});
  assert.deepEqual(parseTelemetryLine('noise tanpa pasangan'), {});
});
