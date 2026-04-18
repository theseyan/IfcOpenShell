// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "guid.h"

#include <algorithm>
#include <array>
#include <cstring>
#include <random>
#include <sstream>
#include <iomanip>

namespace ifcapi {

// IFC base64 alphabet: 0-9 A-Z a-z _ $
static const char IFC_B64[] =
    "0123456789"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "_$";

// Standard base64 alphabet
static const char STD_B64[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789"
    "+/";

static uint8_t hex_val(char c) {
    if (c >= '0' && c <= '9') return static_cast<uint8_t>(c - '0');
    if (c >= 'a' && c <= 'f') return static_cast<uint8_t>(c - 'a' + 10);
    if (c >= 'A' && c <= 'F') return static_cast<uint8_t>(c - 'A' + 10);
    return 0;
}

static char hex_char(uint8_t v) {
    return "0123456789abcdef"[v & 0xF];
}

// Build translation table: std_b64[i] -> ifc_b64[i]
static std::array<char, 128> build_std_to_ifc() {
    std::array<char, 128> table{};
    for (int i = 0; i < 64; ++i) {
        table[static_cast<unsigned char>(STD_B64[i])] = IFC_B64[i];
    }
    return table;
}

static std::array<char, 128> build_ifc_to_std() {
    std::array<char, 128> table{};
    for (int i = 0; i < 64; ++i) {
        table[static_cast<unsigned char>(IFC_B64[i])] = STD_B64[i];
    }
    return table;
}

static const auto STD_TO_IFC = build_std_to_ifc();
static const auto IFC_TO_STD = build_ifc_to_std();

// Minimal base64 encode for exactly 18 bytes → 24 chars (no padding needed)
static std::string base64_encode(const uint8_t* data, size_t len) {
    std::string out;
    out.reserve((len + 2) / 3 * 4);
    for (size_t i = 0; i < len; i += 3) {
        uint32_t n = static_cast<uint32_t>(data[i]) << 16;
        if (i + 1 < len) n |= static_cast<uint32_t>(data[i + 1]) << 8;
        if (i + 2 < len) n |= static_cast<uint32_t>(data[i + 2]);
        out.push_back(STD_B64[(n >> 18) & 0x3F]);
        out.push_back(STD_B64[(n >> 12) & 0x3F]);
        if (i + 1 < len) out.push_back(STD_B64[(n >> 6) & 0x3F]);
        if (i + 2 < len) out.push_back(STD_B64[n & 0x3F]);
    }
    return out;
}

// Minimal base64 decode
static std::vector<uint8_t> base64_decode(const std::string& encoded) {
    std::array<uint8_t, 128> dtable{};
    for (int i = 0; i < 64; ++i)
        dtable[static_cast<unsigned char>(STD_B64[i])] = static_cast<uint8_t>(i);

    std::vector<uint8_t> out;
    out.reserve(encoded.size() * 3 / 4);
    uint32_t val = 0;
    int bits = 0;
    for (char c : encoded) {
        if (c == '=' || c == '\0') break;
        val = (val << 6) | dtable[static_cast<unsigned char>(c)];
        bits += 6;
        if (bits >= 8) {
            bits -= 8;
            out.push_back(static_cast<uint8_t>((val >> bits) & 0xFF));
        }
    }
    return out;
}

std::string uuid4_hex() {
    static thread_local std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<uint32_t> dist(0, 255);

    uint8_t bytes[16];
    for (auto& b : bytes)
        b = static_cast<uint8_t>(dist(gen));

    // Set version 4
    bytes[6] = (bytes[6] & 0x0F) | 0x40;
    // Set variant 1
    bytes[8] = (bytes[8] & 0x3F) | 0x80;

    std::string hex;
    hex.reserve(32);
    for (auto b : bytes) {
        hex.push_back(hex_char(b >> 4));
        hex.push_back(hex_char(b & 0xF));
    }
    return hex;
}

std::string guid_compress(const std::string& uuid_hex) {
    // Strip dashes and lowercase
    std::string clean;
    clean.reserve(32);
    for (char c : uuid_hex) {
        if (c != '-')
            clean.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
    }

    // Pad with "0000" prefix (2 extra bytes) to get 18 bytes
    std::string padded = "0000" + clean;

    // Convert hex string to bytes
    uint8_t bytes[18];
    for (int i = 0; i < 18; ++i)
        bytes[i] = (hex_val(padded[2 * i]) << 4) | hex_val(padded[2 * i + 1]);

    // Base64 encode → 24 chars
    std::string b64 = base64_encode(bytes, 18);

    // Remove 2-char prefix from padding
    b64 = b64.substr(2);

    // Translate standard base64 → IFC base64
    for (char& c : b64)
        c = STD_TO_IFC[static_cast<unsigned char>(c)];

    return b64;
}

std::string guid_expand(const std::string& guid) {
    // Translate IFC base64 → standard base64
    std::string std_b64 = "AA" + guid;  // pad with base64 "zeros"
    for (char& c : std_b64) {
        if (c != 'A' || (&c - &std_b64[0]) >= 2)
            c = IFC_TO_STD[static_cast<unsigned char>(c)];
    }
    // First two chars are already 'A' which maps to 0 in std base64
    for (int i = 0; i < 2; ++i)
        std_b64[i] = 'A';

    // Decode to bytes
    auto bytes = base64_decode(std_b64);

    // Convert to hex, skip 2-byte padding
    std::string hex;
    hex.reserve(32);
    for (size_t i = 2; i < bytes.size() && hex.size() < 32; ++i) {
        hex.push_back(hex_char(bytes[i] >> 4));
        hex.push_back(hex_char(bytes[i] & 0xF));
    }

    return hex;
}

std::string guid_new() {
    return guid_compress(uuid4_hex());
}

} // namespace ifcapi
