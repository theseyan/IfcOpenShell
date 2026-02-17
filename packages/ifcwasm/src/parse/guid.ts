const CHARS64_STD = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
const CHARS64_IFC = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_$";

const ifcToStd = new Map<string, string>();
const stdToIfc = new Map<string, string>();
for (let i = 0; i < 64; i++) {
  ifcToStd.set(CHARS64_IFC[i], CHARS64_STD[i]);
  stdToIfc.set(CHARS64_STD[i], CHARS64_IFC[i]);
}

function translate(s: string, table: Map<string, string>): string {
  let out = "";
  for (const ch of s) out += table.get(ch) ?? ch;
  return out;
}

function hexToBytes(hex: string): Uint8Array {
  const bytes = new Uint8Array(hex.length / 2);
  for (let i = 0; i < hex.length; i += 2) {
    bytes[i / 2] = parseInt(hex.substring(i, i + 2), 16);
  }
  return bytes;
}

function bytesToHex(bytes: Uint8Array): string {
  let hex = "";
  for (const b of bytes) hex += b.toString(16).padStart(2, "0");
  return hex;
}

function btoa_(bytes: Uint8Array): string {
  let binary = "";
  for (const b of bytes) binary += String.fromCharCode(b);
  return btoa(binary);
}

function atob_(s: string): Uint8Array {
  const binary = atob(s);
  const bytes = new Uint8Array(binary.length);
  for (let i = 0; i < binary.length; i++) bytes[i] = binary.charCodeAt(i);
  return bytes;
}

export function compress(uuid: string): string {
  uuid = uuid.toLowerCase().replace(/[^0-9a-f]/g, "");
  uuid = "0000" + uuid;
  const bytes = hexToBytes(uuid);
  let guid = btoa_(bytes);
  guid = guid.substring(2);
  guid = translate(guid, stdToIfc);
  return guid;
}

export function expand(guid: string): string {
  let translated = translate(guid, ifcToStd);
  translated = "AA" + translated;
  const bytes = atob_(translated);
  let hex = bytesToHex(bytes);
  hex = hex.substring(4);
  return hex;
}

export function split(uuid: string): string {
  return [
    uuid.substring(0, 8),
    uuid.substring(8, 12),
    uuid.substring(12, 16),
    uuid.substring(16, 20),
    uuid.substring(20),
  ].join("-");
}

export function newGuid(): string {
  const bytes = new Uint8Array(16);
  if (typeof crypto !== "undefined" && crypto.getRandomValues) {
    crypto.getRandomValues(bytes);
  } else {
    for (let i = 0; i < 16; i++) bytes[i] = Math.floor(Math.random() * 256);
  }
  bytes[6] = (bytes[6] & 0x0f) | 0x40;
  bytes[8] = (bytes[8] & 0x3f) | 0x80;
  const hex = bytesToHex(bytes);
  return compress(hex);
}
