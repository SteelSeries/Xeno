/*************************************************************************************************
 *                                         INCLUDES                                              *
 *************************************************************************************************/
#include "Commands.h"
#include "spi.h"
#include "parser.h"
#include "power_control.h"
#include <string.h>

 /*************************************************************************************************
 *                                  LOCAL MACRO DEFINITIONS                                      *
 *************************************************************************************************/
#define NUM_COMMANDS                 (sizeof(handler_table)/sizeof(handler_table[0]))

/*************************************************************************************************
 *                                  LOCAL TYPE DEFINITIONS                                       *
 *************************************************************************************************/
typedef enum
{
    COMMAND_STATUS_SUCCESS = 0,
    COMMAND_STATUS_ERROR_INTERNAL,
    COMMAND_STATUS_ERROR_BAD_PARAM,
    COMMAND_STATUS_ERROR_NO_HANDLER,
} Command_Status_t;

typedef enum
{
    RECOVERY_MODE_APPLICATION = 0,
    RECOVERY_MODE_BOOTLOADER,
    RECOVERY_MODE_NUM_MODES
} Recovery_Mode_t;

typedef enum
{
    FILE_ID_LOCAL = 0,
    FILE_ID_PERIPHERAL,
    FILE_ID_NUM_MODES

} File_Id_t;

// define how command handlers look like
typedef Command_Status_t (*Command_Handler_t)(const uint8_t command[SPI_BUFFER_SIZE]);

typedef struct
{
   uint8_t           op;
   Command_Handler_t read;
   Command_Handler_t write;
} cmd_handler_t;

/*************************************************************************************************
 *                                GLOBAL VARIABLE DEFINITIONS                                    *
 *************************************************************************************************/
bool SS_RESET_FLAG = false;

/*************************************************************************************************
 *                                STATIC FUNCTION DECLARATIONS                                   *
 *************************************************************************************************/
static Command_Status_t Command_HandleNoop(const uint8_t command[SPI_BUFFER_SIZE]);
static Command_Status_t CommandVersion_ReadVersion(const uint8_t command[SPI_BUFFER_SIZE]);
static Command_Status_t CommandRecovery_Reset(const uint8_t command[SPI_BUFFER_SIZE]);
static Command_Status_t internal_usb_suspend_handler(const uint8_t command[SPI_BUFFER_SIZE]);
static Command_Status_t internal_usb_resume_handler(const uint8_t command[SPI_BUFFER_SIZE]);

/*************************************************************************************************
 *                                STATIC VARIABLE DEFINITIONS                                    *
 *************************************************************************************************/
static uint8_t buffer[SPI_BUFFER_SIZE] = {0};
static const cmd_handler_t handler_table[] =
{
    { .op = NO_OP,                    .read = Command_HandleNoop,                       .write = Command_HandleNoop },
    // mcu control
    { .op = RESET_OP,                 .read = Command_HandleNoop,                       .write = CommandRecovery_Reset },
    // info
    { .op = VERSION_OP,               .read = CommandVersion_ReadVersion,               .write = Command_HandleNoop },
    // internal
    { .op = INTERNAL_USB_SUSPEND_OP,  .read = Command_HandleNoop,                       .write = internal_usb_suspend_handler },
    { .op = INTERNAL_USB_RESUME_OP,   .read = Command_HandleNoop,                       .write = internal_usb_resume_handler },

};


/*************************************************************************************************
 *                                GLOBAL FUNCTION DEFINITIONS                                    *
 *************************************************************************************************/
void Commands_HandleSpiCommand(const uint8_t * in, size_t in_len)
{
    Command_Status_t status = COMMAND_STATUS_ERROR_NO_HANDLER;

    memcpy(buffer, in, in_len - 1);

    uint8_t command = (buffer[0]);
    uint8_t op = command & (~COMMAND_READ_FLAG);
    bool is_read = (command & COMMAND_READ_FLAG) == COMMAND_READ_FLAG;
    #if defined(DEBUG_PRINTF) && 0
    printf("[%s] CMD:%02X\n", __func__, command);
    #endif
    for (int i = 0; i < NUM_COMMANDS; ++i)
    {
        if (handler_table[i].op == op)
        {
            if (is_read)
            {
                status = handler_table[i].read(buffer);
                break;
            }
            else
            {
                status = handler_table[i].write(buffer);
                break;
            }
        }
    }
    if (status != COMMAND_STATUS_SUCCESS)
    {
        printf("USB COMMAND ERROR CMD:%02X \n", command);
    }
}

 /*************************************************************************************************
 *                                STATIC FUNCTION DEFINITIONS                                    *
 *************************************************************************************************/

static Command_Status_t Command_HandleNoop(const uint8_t command[SPI_BUFFER_SIZE])
{
    uint8_t txBuf[1] = {0};
    Parser_SendBuffer(txBuf, sizeof(txBuf));
    return COMMAND_STATUS_SUCCESS;
}

static Command_Status_t CommandVersion_ReadVersion(const uint8_t command[SPI_BUFFER_SIZE])
{
    uint8_t txBuf[4] = {0};
    txBuf[0] = VERSION_OP | COMMAND_READ_FLAG;
    txBuf[1] = (uint8_t)(DESC_BCDDEVICE >> 16) & 0xFF;
    txBuf[2] = (uint8_t)(DESC_BCDDEVICE >> 8) & 0xFF;
    txBuf[3] = (uint8_t)DESC_BCDDEVICE;
    Parser_SendBuffer(txBuf, sizeof(txBuf));
    return COMMAND_STATUS_SUCCESS;
}

static Command_Status_t CommandRecovery_Reset(const uint8_t command[SPI_BUFFER_SIZE])
{
    if (command[1] < RECOVERY_MODE_NUM_MODES)
    {
        if (command[1] == RECOVERY_MODE_BOOTLOADER)
        {
            if (command[2] == FILE_ID_PERIPHERAL)
            {
                SS_RESET_FLAG = true;
            }
        }
        else if (command[1] == RECOVERY_MODE_APPLICATION)
        {
            SS_RESET_FLAG = true;
        }
    }
    else
    {
        SS_RESET_FLAG = true;
    }
    return COMMAND_STATUS_SUCCESS;
}

static Command_Status_t internal_usb_suspend_handler(const uint8_t command[SPI_BUFFER_SIZE])
{
    PowerControl_EnterDeepSleep();
    return COMMAND_STATUS_SUCCESS;
}

static Command_Status_t internal_usb_resume_handler(const uint8_t command[SPI_BUFFER_SIZE])
{
    PowerControl_Resume();
    return COMMAND_STATUS_SUCCESS;
}

