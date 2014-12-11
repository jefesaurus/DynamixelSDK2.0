#include <stdint.h>
#include <assert.h>

#include "packet_utils.h"
#include "crc.h"

/*
 * Packet Construction utilities.
 *
 * Unlike the "high level commands" these are supposed to be general across Dynamixel SDK 2.0
 * That is, they are supposed to work on both XL-320 and DXL-PRO
 * Haven't test on a DXL-PRO though...
 */
inline uint8_t LowBits(uint16_t word);

inline uint8_t HighBits(uint16_t word);

inline uint16_t MakeWord(uint8_t low_bits, uint8_t high_bits);

void SetPacketPrefix(uint8_t* data,
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
void SetChecksum(uint8_t* data, uint16_t num_parameters) {
  // The prefix size is 8 bytes. Check overflow.
  assert(num_parameters <= MAX_PACKET_BYTES - PREFIX_SIZE);
  uint16_t num_bytes_before_checksum = num_parameters + PREFIX_SIZE;
  uint16_t crc = UpdateCRC(0, data, num_bytes_before_checksum);
  data[num_bytes_before_checksum] = LowBits(crc);
  data[num_bytes_before_checksum + 1] = HighBits(crc);
}


void SetByteParam(uint8_t* data, uint16_t param_byte_offset, uint8_t value) {
  data[PREFIX_SIZE + param_byte_offset] = value;
}

void SetWordParam(uint8_t* data, uint16_t param_byte_offset, uint16_t value) {
  data[PREFIX_SIZE + param_byte_offset] = LowBits(value);
  data[PREFIX_SIZE + param_byte_offset + 1] = HighBits(value);
}

uint8_t GetByteParam(uint8_t* data, uint16_t param_byte_offset) {
  return data[PREFIX_SIZE + param_byte_offset];
}

uint16_t GetWordParam(uint8_t* data, uint16_t param_byte_offset) {
  return MakeWord(data[PREFIX_SIZE + param_byte_offset],
                  data[PREFIX_SIZE + param_byte_offset + 1]);
}


// Higher Level Packet Construction
// Returns number of parameter bytes added.
uint16_t MakePingPacket(uint8_t* data, uint8_t dxl_id) {
  uint8_t instruction = 1;
  uint16_t num_parameters_tx = 0;
  SetPacketPrefix(data, dxl_id, instruction, num_parameters_tx);
  return num_parameters_tx;
}

// Returns number of parameter bytes added.
uint16_t MakeReadPacket(uint8_t* data, uint8_t dxl_id, uint16_t start_addr, uint16_t num_bytes) {
  uint8_t instruction = 2;
  uint16_t num_parameters_tx = 4;
  SetPacketPrefix(data, dxl_id, instruction, num_parameters_tx);
  SetByteParam(data, 0, start_addr); 
  SetByteParam(data, 2, num_bytes); 
  return num_parameters_tx;
}

// TODO Write
// TODO Reg write
// TODO Action
// TODO Factory reset
// TODO Reboot
// TODO Sync read
// TODO Sync write
// TODO Bulk read
// TODO Bulk write
