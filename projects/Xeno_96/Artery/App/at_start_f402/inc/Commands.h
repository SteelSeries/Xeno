
#pragma once
/*************************************************************************************************
 *                                          INCLUDES                                             *
 *************************************************************************************************/
#include "at32f402_405_board.h"
#include <stdbool.h>

/*************************************************************************************************
 *                                   GLOBAL MACRO DEFINITIONS                                    *
 *************************************************************************************************/
// firmware info
#define VERSION_OP                  0x10U // version

// MCU control
#define RESET_OP                    0x01U // recovery module

// For firmware/file update
#define ERASE_FILE_OP               0x02U // HCFS
#define FILE_ACCESS_OP              0x03U // HCFS
#define FILE_CRC32_OP               0x04U // HCFS

// internal - we can reuse those as USB APIs for Artery
#define INTERNAL_USB_SUSPEND_OP     0x0CU // USB entered suspend mode. Lower power usage
#define INTERNAL_USB_RESUME_OP      0x0DU // after suspension, resume from device received

#define NO_OP                       0x00U // command module

#define NORMAL_MODE                 0x01U
#define BOOTLOADER_MODE             0x10U

#define DESC_BCDDEVICE              0x0001

#define COMMAND_READ_FLAG           0x80U

/*************************************************************************************************
 *                                    GLOBAL TYPE DEFINITIONS                                    *
 *************************************************************************************************/

/*************************************************************************************************
 *                                  GLOBAL VARIABLE DECLARATIONS                                 *
 *************************************************************************************************/
extern bool SS_RESET_FLAG;

/*************************************************************************************************
 *                                  GLOBAL FUNCTION DECLARATIONS                                 *
 *************************************************************************************************/
void Commands_HandleSpiCommand(const uint8_t * in, size_t in_len);
