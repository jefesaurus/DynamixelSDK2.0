#ifndef PACKET_UTILS_H
#define PACKET_UTILS_H

#include <stdint.h>

static const int MAX_PACKET_BYTES = 256;
static const int HEADER_SIZE = 3;
static const uint8_t HEADER[3] = {0xFF, 0xFF, 0xFD}; 
static const int PREFIX_SIZE = 8;
static const int CHECKSUM_SIZE = 2;

static const int BASE_PACKET_SIZE = 8 + 2; // Prefix + checksum

extern inline uint8_t LowBits(uint16_t word) {
  return word & 0xFF;
}

extern inline uint8_t HighBits(uint16_t word) {
  return (word >> 8) & 0xFF;
}

extern inline uint16_t MakeWord(uint8_t low_bits, uint8_t high_bits) {
  return (high_bits << 8) + low_bits;
}

void AddPacketPrefix(uint8_t* data, uint8_t dxl_id, uint8_t instruction, uint16_t num_parameters);

// Adds the checksum. This should be the final piece of the packet, so this also
// returns the total number of bytes in the final packet.
void AddChecksum(uint8_t* data, uint16_t num_parameters);

#endif // PACKET_UTILS_H
