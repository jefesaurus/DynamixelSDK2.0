#include <stdint.h>
#include <stdbool.h>

/*
  These should be common use cases that require knowledge of the registry layout.
  So these will be specific to XL-320
*/

bool SendPing(uint8_t dxl_id, int* modelnum, int* firmware_version);
bool ReadPosition(uint8_t dxl_id, uint16_t* position);
bool ReadStatusReturnLevel(uint8_t dxl_id, uint8_t* return_level);
bool ReadMovingStatus(uint8_t dxl_id, bool* moving);

bool SetLED(uint8_t dxl_id, uint8_t color);
bool SetPosition(uint8_t dxl_id, uint16_t position);

bool SetTorqueEnable(uint8_t dxl_id, uint8_t is_enabled);
bool TorqueEnable(uint8_t dxl_id);
bool TorqueDisable(uint8_t dxl_id);
