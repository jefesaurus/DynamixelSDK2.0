#include "high_level_commands.h"
#include "packet_utils.h"
#include "communications.h"

bool SendPing(uint8_t dxl_id, int* modelnum, int* firmware_version) {
  unsigned char data[MAX_PACKET_BYTES];

  // Make packet.
  uint16_t num_parameters_tx = MakePingPacket(data, dxl_id);

  uint16_t num_parameters_rx = 4; // Error, model low, model high, firmware
  bool result = TXRXPacket(data, num_parameters_tx, num_parameters_rx);

  uint8_t error = GetByteParam(data, 0);
  if (result && error == 0) {
    *modelnum = GetWordParam(data, 1);
    *firmware_version = GetByteParam(data, 3);
    return true;
  } else {
    return false;
  }
}


bool ReadPosition(uint8_t dxl_id, uint16_t* position) {
  unsigned char data[MAX_PACKET_BYTES];

  int position_register = 37;
  int position_bytes = 2;
  int num_parameters_tx = MakeReadPacket(data, dxl_id, position_register, position_bytes);

  uint16_t num_parameters_rx = 1 + position_bytes; // Error, Position low, position high
  bool result = TXRXPacket(data, num_parameters_tx, num_parameters_rx);

  uint8_t error = GetByteParam(data, 0);

  if (result && error == 0) {
    *position = GetWordParam(data, 1);
  } else {
    return false;
  }
}
