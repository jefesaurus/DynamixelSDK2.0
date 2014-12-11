#include <stdio.h>
#include <stdbool.h>
#include <termio.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>
#include <time.h>

#include "dxl_hal.h"
#include "crc.h"
#include "packet_utils.h"


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

int InitDXL(int deviceIndex, int baudnum ) {
  if(dxl_hal_open(deviceIndex, GetBaudRate(baudnum)) == 0) {
    return 0;
  }

  return 1;
}

void TerminateDXL(void) {
  dxl_hal_close();
}

// Sends the data over opened DXL serial line.
// Returns false if not all the bytes were sent.
bool TXPacket(uint8_t* data, int num_bytes) {
  assert(num_bytes < MAX_PACKET_BYTES);
  int num_transmitted_bytes = dxl_hal_tx(data, num_bytes);
  if(num_bytes != num_transmitted_bytes) {
    return false;
  } else {
    return true;
  }
}

// Caller is resonsible for the received_data buffer's appropriate size.
// Returns the total number of bytes received for packet.
// If receive timed out, value of num_bytes_received is undefined.
bool RXPacket(uint8_t* received_data, int packet_length, int* num_bytes_received) {
  dxl_hal_set_timeout(packet_length);

  // First, read bytes until the header is found:
  int header_match_count = 0;
  int buffer_index = 0;
  int bytes_read = 0; 
  int count = 0;

  // Repeatedly attempt to read until either we have all of the bytes
  // requested or the timeout expires.
  // TODO replace this logic with something that isn't so CPU intensive.
  // like interrupt driven...
  while (bytes_read < packet_length) {
    if (dxl_hal_timeout() == 1) {
      return false;
    }
    bytes_read += dxl_hal_rx(&(received_data[header_match_count]), packet_length);
  }

  while (header_match_count < HEADER_SIZE) {
    if (buffer_index >= bytes_read) {
      return false;
    }
    if (received_data[buffer_index++] == HEADER[header_match_count]) {
      header_match_count++;
    } else {
      header_match_count = 0;
    }
  }

  // Now, the header is matched
  // Move bytes down to the beginning of the data buffer.
  int packet_start_index = buffer_index - HEADER_SIZE;
  bytes_read -= packet_start_index;
  memmove(received_data, &(received_data[packet_start_index]), bytes_read);

  // Now read the remaining bytes if necessary.
  bytes_read += dxl_hal_rx(&(received_data[bytes_read]), packet_length - bytes_read);
  while (bytes_read < packet_length) {
    bytes_read += dxl_hal_rx(&(received_data[bytes_read]), packet_length - bytes_read);
    if (dxl_hal_timeout() == 1) {
      return false;
    }
  }

  uint16_t expected_crc = UpdateCRC(0, received_data, bytes_read - CHECKSUM_SIZE);
  uint16_t received_crc = MakeWord(received_data[bytes_read - 2], received_data[bytes_read - 1]);
  if (expected_crc != received_crc) {
    // CRC check failed.
    return false;
  }

  *num_bytes_received = bytes_read;
  return true;
}

// Error within the status packet is considered to be "parameter 0"
// So Pings receive 4 bytes of parameter.
bool TXRXPacket(uint8_t* data, int num_params_sending, int num_params_receiving) {
  AddChecksum(data, num_params_sending);
  // TRANSMIT PACKET
  bool result = TXPacket(data, BASE_PACKET_SIZE + num_params_sending);
  if (!result) {
    // printf("Couldn't send packet successfully.\n");
    return false;
  }

  // RECEIVE PACKET
  int num_bytes_received;
  result = RXPacket(data, BASE_PACKET_SIZE + num_params_receiving, &num_bytes_received);
  if (!result) {
    // printf("Couldn't receive packet successfully.\n");
    return false;
  }
  return true;
}

bool SendPing(uint8_t dxl_id) {
  unsigned char data[MAX_PACKET_BYTES];

  // Make packet.
  uint8_t instruction = 1;
  uint16_t num_parameters_tx = 0;
  AddPacketPrefix(data, dxl_id, instruction, num_parameters_tx);

  uint16_t num_parameters_rx = 4; // Error, model low, model high, firmware
  bool result = TXRXPacket(data, num_parameters_tx, num_parameters_rx);

  if (result) {
    int modelnum_low = data[9];
    int modelnum_high = data[10];
    int modelnum = (modelnum_high << 8) + modelnum_low;
    int firmware_version = data[11];
    printf("ID: %d, Model number: %d, Firmware: %d\n", dxl_id, modelnum, firmware_version);
  }
  return result;
}

void CheckIDsAndBauds(int usb2ax_index) {
  // 1Mbps for dynamixels. Currently the only one that works on this setup for reasons unknown to me...
  int baud_num = 3;
  if(InitDXL(usb2ax_index, baud_num) == 0 ) {
    printf("Failed to open device: %d, baud num: %d\n", usb2ax_index, baud_num);
    TerminateDXL();
    return;
  } else {
    printf("Successfully opened device: %d, baud num: %d\n", usb2ax_index, baud_num);
  }

  printf("Searching ID range...\n");

  int current_id = 0;
  int dxl_count = 0;
  for (current_id = 0; current_id < 253; current_id++) {
    if (SendPing(current_id)) {
      dxl_count++;
    }
  }

  TerminateDXL();
  if (dxl_count == 1) {
    printf("Found %d unique dynamixel ID.\n", dxl_count);
  } else {
    printf("Found %d unique dynamixel ID's.\n", dxl_count);
  }
}

void ListDevices() {
  DIR *dir;
  struct dirent *ent;
  char* devices_dir = "/dev/";
  char* expected_prefix = "ttyACM";
  int i;
  if ((dir = opendir (devices_dir)) != NULL) {
    while ((ent = readdir (dir)) != NULL) {
      if (strlen(expected_prefix) <= strlen(ent->d_name) &&
        memcmp(ent->d_name, expected_prefix, strlen(expected_prefix)) == 0) {
        printf("%s\n", ent->d_name);
      }
    }
    closedir (dir);
  } else {
    printf("Couldn't find device directory: %s\n", devices_dir);
  }
}

int main(int argc, char* argv[]) {
  int device_num = 0; // Default to device ID 0
  if (argc >= 2) {
    char *end;
    long value = strtol(argv[1], &end, 10); 
    if (!(end == argv[1] || *end != '\0' || errno == ERANGE)) {
      CheckIDsAndBauds(device_num);
    } else {
      printf("Arg 1 (%s) was not recognized as an integer device number.\n", argv[1]);
    }
  } else {
    printf("Enter USB2AX ttyACM* number as argument 1!\n");
    printf("Here are some possible device ID's from /dev/*.\n");
    ListDevices();
  }
  return 0;
}
