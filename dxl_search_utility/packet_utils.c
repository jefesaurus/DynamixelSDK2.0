#include <stdint.h>
#include <assert.h>

#include "packet_utils.h"
#include "crc.h"

/*
 * Packet Construction utilities.
 */
inline uint8_t LowBits(uint16_t word);

inline uint8_t HighBits(uint16_t word);

inline uint16_t MakeWord(uint8_t low_bits, uint8_t high_bits);

void AddPacketPrefix(uint8_t* data,
                     uint8_t dxl_id,
                     uint8_t instruction,
                     uint16_t num_parameters) {
  // Header
  data[0] = 0xFF;
  data[1] = 0xFF;
  data[2] = 0xFD;

  // Reserved
  // At the time of this writing, this register is inconsequential.
  // data[3] = 0x00;

  // The dynamixel ID to target. 0xFE is broadcast
  data[4] = dxl_id;

  // Packet Length
  // == Length after packet length field
  // == Number of parameters + 3
  int packet_length = num_parameters + 3;

  assert(packet_length <= UINT16_MAX);       // No 16bit overflow.
  data[5] = LowBits(packet_length);         // Low order bits of packet length.
  data[6] = HighBits(packet_length);  // High order bits of packet length.

  // Instruction byte.
  data[7] = instruction;
}

/*
 * Adds the CRC checksum. The entire data packet up until the 
 * CRC checksum must already be set, because the value is a function
 * of all the preceding bytes.
 * Returns the total number of bytes in the packet.
 */
void AddChecksum(uint8_t* data, uint16_t num_parameters) {
  // The prefix size is 8 bytes. Check overflow.
  assert(num_parameters <= MAX_PACKET_BYTES - PREFIX_SIZE);
  uint16_t num_bytes_before_checksum = num_parameters + PREFIX_SIZE;
  uint16_t crc = UpdateCRC(0, data, num_bytes_before_checksum);
  data[num_bytes_before_checksum] = LowBits(crc);
  data[num_bytes_before_checksum + 1] = HighBits(crc);
}
