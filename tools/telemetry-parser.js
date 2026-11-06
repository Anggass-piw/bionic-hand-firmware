'use strict';

const NUMERIC_FIELDS = new Set(['adc', 'mav', 'rms', 'feature', 'threshold', 'target', 'current']);
const KNOWN_FIELDS = new Set([...NUMERIC_FIELDS, 'active', 'status', 'mode']);
const NUMBER_PATTERN = /^[+-]?(?:\d+\.?\d*|\.\d+)(?:[eE][+-]?\d+)?$/;

function parseFiniteNumber(value) {
  if (!NUMBER_PATTERN.test(value)) return null;
  const number = Number(value);
  return Number.isFinite(number) ? number : null;
}

function parseActive(value) {
  switch (value.toLowerCase()) {
    case '1':
    case 'true':
    case 'on':
    case 'yes':
      return true;
    case '0':
    case 'false':
    case 'off':
    case 'no':
      return false;
    default:
      return null;
  }
}

/**
 * Parse satu baris telemetry firmware. Field yang tidak ada tidak dimasukkan;
 * field bernilai invalid tetap dicantumkan dengan nilai null.
 *
 * @param {string} line Baris berisi pasangan key=value dipisahkan spasi/koma.
 * @returns {Record<string, number|string|boolean|null>}
 */
function parseTelemetryLine(line) {
  const result = {};
  if (typeof line !== 'string') return result;

  for (const token of line.trim().split(/[\s,]+/)) {
    if (!token) continue;
    const separator = token.indexOf('=');
    if (separator <= 0) continue;
    const key = token.slice(0, separator).toLowerCase();
    const rawValue = token.slice(separator + 1);
    if (!KNOWN_FIELDS.has(key)) continue;

    if (NUMERIC_FIELDS.has(key)) result[key] = parseFiniteNumber(rawValue);
    else if (key === 'active') result[key] = parseActive(rawValue);
    else result[key] = rawValue.length ? rawValue : null;
  }

  return result;
}

module.exports = { parseTelemetryLine };
