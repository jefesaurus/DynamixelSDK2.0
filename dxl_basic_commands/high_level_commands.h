#include <stdint.h>
#include <stdbool.h>

/*
  These should be common use cases that require knowledge of the registry layout.
  So these will be specific to XL-320
*/

bool SendPing(uint8_t dxl_id, int* modelnum, int* firmware_version);
bool ReadPosition(uint8_t dxl_id, uint16_t* position);
