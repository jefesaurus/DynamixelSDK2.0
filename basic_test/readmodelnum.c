#include "dxl_hal.h"
#include <stdio.h>
#include <stdbool.h>
#include <termio.h>
#include <unistd.h>
#include <assert.h>


int GetBaudRate(int baud_num) {
  switch(baud_num) {
  case 0:
    return 9600;
  case 1:
    return 57600;
  case 2:
    return 115200;
  case 3:
    return 1000000;
  default:
    printf("Invalid dynamixel baud number: %d\n", baud_num);
    assert(false);
  }
  return -1;
}

int dxl_initialize(int deviceIndex, int baudnum ) {
  if(dxl_hal_open(deviceIndex, GetBaudRate(baudnum)) == 0) {
    return 0;
  }

  return 1;
}

void dxl_terminate(void) {
  dxl_hal_close();
}

unsigned short update_crc(unsigned short crc_accum, unsigned char *data_blk_ptr, unsigned short data_blk_size) {
  unsigned short i, j;
  unsigned short crc_table[256] = {
    0x0000, 0x8005, 0x800F, 0x000A, 0x801B, 0x001E, 0x0014, 0x8011,
    0x8033, 0x0036, 0x003C, 0x8039, 0x0028, 0x802D, 0x8027, 0x0022,
    0x8063, 0x0066, 0x006C, 0x8069, 0x0078, 0x807D, 0x8077, 0x0072,
    0x0050, 0x8055, 0x805F, 0x005A, 0x804B, 0x004E, 0x0044, 0x8041,
    0x80C3, 0x00C6, 0x00CC, 0x80C9, 0x00D8, 0x80DD, 0x80D7, 0x00D2,
    0x00F0, 0x80F5, 0x80FF, 0x00FA, 0x80EB, 0x00EE, 0x00E4, 0x80E1,
    0x00A0, 0x80A5, 0x80AF, 0x00AA, 0x80BB, 0x00BE, 0x00B4, 0x80B1,
    0x8093, 0x0096, 0x009C, 0x8099, 0x0088, 0x808D, 0x8087, 0x0082,
    0x8183, 0x0186, 0x018C, 0x8189, 0x0198, 0x819D, 0x8197, 0x0192,
    0x01B0, 0x81B5, 0x81BF, 0x01BA, 0x81AB, 0x01AE, 0x01A4, 0x81A1,
    0x01E0, 0x81E5, 0x81EF, 0x01EA, 0x81FB, 0x01FE, 0x01F4, 0x81F1,
    0x81D3, 0x01D6, 0x01DC, 0x81D9, 0x01C8, 0x81CD, 0x81C7, 0x01C2,
    0x0140, 0x8145, 0x814F, 0x014A, 0x815B, 0x015E, 0x0154, 0x8151,
    0x8173, 0x0176, 0x017C, 0x8179, 0x0168, 0x816D, 0x8167, 0x0162,
    0x8123, 0x0126, 0x012C, 0x8129, 0x0138, 0x813D, 0x8137, 0x0132,
    0x0110, 0x8115, 0x811F, 0x011A, 0x810B, 0x010E, 0x0104, 0x8101,
    0x8303, 0x0306, 0x030C, 0x8309, 0x0318, 0x831D, 0x8317, 0x0312,
    0x0330, 0x8335, 0x833F, 0x033A, 0x832B, 0x032E, 0x0324, 0x8321,
    0x0360, 0x8365, 0x836F, 0x036A, 0x837B, 0x037E, 0x0374, 0x8371,
    0x8353, 0x0356, 0x035C, 0x8359, 0x0348, 0x834D, 0x8347, 0x0342,
    0x03C0, 0x83C5, 0x83CF, 0x03CA, 0x83DB, 0x03DE, 0x03D4, 0x83D1,
    0x83F3, 0x03F6, 0x03FC, 0x83F9, 0x03E8, 0x83ED, 0x83E7, 0x03E2,
    0x83A3, 0x03A6, 0x03AC, 0x83A9, 0x03B8, 0x83BD, 0x83B7, 0x03B2,
    0x0390, 0x8395, 0x839F, 0x039A, 0x838B, 0x038E, 0x0384, 0x8381,
    0x0280, 0x8285, 0x828F, 0x028A, 0x829B, 0x029E, 0x0294, 0x8291,
    0x82B3, 0x02B6, 0x02BC, 0x82B9, 0x02A8, 0x82AD, 0x82A7, 0x02A2,
    0x82E3, 0x02E6, 0x02EC, 0x82E9, 0x02F8, 0x82FD, 0x82F7, 0x02F2,
    0x02D0, 0x82D5, 0x82DF, 0x02DA, 0x82CB, 0x02CE, 0x02C4, 0x82C1,
    0x8243, 0x0246, 0x024C, 0x8249, 0x0258, 0x825D, 0x8257, 0x0252,
    0x0270, 0x8275, 0x827F, 0x027A, 0x826B, 0x026E, 0x0264, 0x8261,
    0x0220, 0x8225, 0x822F, 0x022A, 0x823B, 0x023E, 0x0234, 0x8231,
    0x8213, 0x0216, 0x021C, 0x8219, 0x0208, 0x820D, 0x8207, 0x0202
  };

  for(j = 0; j < data_blk_size; j++) {
    i = ((unsigned short)(crc_accum >> 8) ^ data_blk_ptr[j]) & 0xFF;
    crc_accum = (crc_accum << 8) ^ crc_table[i];
  }

  return crc_accum;
}

void SendPing(unsigned char dxl_id) {
  unsigned char data[128];

  /*
    CONSTRUCT PACKET
  */

  // Header
  data[0] = 0xff;
  data[1] = 0xff;
  data[2] = 0xfd;

  // Reserved
  data[3] = 0x00;

  // ID
  data[4] = dxl_id; // 0xFE is broadcast

  // Packet Length, length after packet length field, number of parameters + 3
  data[5] = 0x03; // Ping has no parameters: 0 + 3
  data[6] = 0x00; // Most signficant bits are all 0.

  // Ping instruction
  data[7] = 0x01;

  // Calculate the CRC
  unsigned short crc = update_crc(0, data, 8);
  data[8] = crc & (0x00FF);         // Set the CRC low byte
  data[9] = (crc >> 8) & (0x00FF);  // Set the CRC high byte


  /*
    TRANSMIT PACKET
  */
  int num_bytes = 10;

  // dxl_hal_clear();
  int num_transmitted_bytes = dxl_hal_tx(data, num_bytes);
  if(num_bytes != num_transmitted_bytes) {
    // printf("Some bytes were not transmitted...\n");
    return;
  }

  int base_return_bytes = 11;
  int ping_return_bytes = 3;
  dxl_hal_set_timeout(base_return_bytes + ping_return_bytes);

  /*
    RECEIVE PACKET
  */
  unsigned char received_data[128];
  bool done_receiving = false;
  int bytes_read = 0;

  int packet_length = 14;
  while (!done_receiving) {
    bytes_read += dxl_hal_rx(&(received_data[bytes_read]), packet_length - bytes_read);
    if (dxl_hal_timeout() == 1) {
      // printf("Timed out after receiving %d bytes.\n", bytes_read);
      return;
    }
    if (bytes_read >= packet_length) {
      done_receiving = true;
    }
  }
  int i;
  // printf("Received bytes: ");
  for (i = 0; i < packet_length; i++) {
    // printf("%02X, ",(int)received_data[i]);
  }

  int modelnum_low = received_data[9];
  int modelnum_high = received_data[10];
  int modelnum = (modelnum_high << 8) + modelnum_low;
  int firmware_version = received_data[11];


  // Check response CRC
  unsigned short expected_crc = update_crc(0, received_data, packet_length - 2);
  unsigned short received_crc = (received_data[packet_length - 1] << 8) + received_data[packet_length - 2];
  if (expected_crc != received_crc) {
    // printf("CRC check failed.\n");
  } else {
    // printf("CRC check succeeded.\n");
    printf("ID: %d, Model number: %d, Firmware: %d\n", dxl_id, modelnum, firmware_version);
  }
}

void CheckIDsAndBauds(int usb2ax_index) {
  int current_baud = 0;
  int current_id = 0;
  // XL-320 baud nums go from 0 to 3
  for (current_baud = 3; current_baud < 4; current_baud++) {
    if(dxl_initialize(usb2ax_index, current_baud) == 0 ) {
      printf("Failed to open device: %d, baud num: %d\n", usb2ax_index, current_baud);
      dxl_terminate();
      continue;
    } else {
      printf("Successfully opened device: %d, baud num: %d\n", usb2ax_index, current_baud);
    }
    for (current_id = 0; current_id < 253; current_id++) {
      SendPing(current_id);
    }

    dxl_terminate();
  }
}

void ResponsiveCheck(int deviceIndex) {
  int dxl_id = 2;
  int baudnum = 1;

  printf( "\n\nRead Model Number example for Linux\n\n" );
  ///////// Open USB2AX ////////////
  if(dxl_initialize(deviceIndex, baudnum) == 0 ) {
    printf( "Failed to open USB2AX!\n" );
    printf( "Press Enter key to terminate...\n" );
    getchar();
    return;
  } else {
    printf( "Succeeded in opening USB2AX!\n" );
  }

  while(1) {
    printf( "Press Enter key to continue!(press ESC and Enter to quit)\n" );
    if(getchar() == 0x1b) {
      break;
    }

    SendPing(dxl_id);
    SendPing(1);
  }

  // Close device
  dxl_terminate();
  printf( "Press Enter key to terminate...\n" );
  getchar();
}

int main() {
  CheckIDsAndBauds(1);

  return 0;
}
