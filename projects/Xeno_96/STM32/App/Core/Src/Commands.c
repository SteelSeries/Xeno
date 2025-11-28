/*************************************************************************************************
 *                                         INCLUDES                                              *
 *************************************************************************************************/
#include "Commands.h"
#include "spi_protocol.h"
#include "spi.h"

/*************************************************************************************************
 *                                  LOCAL MACRO DEFINITIONS                                      *
 *************************************************************************************************/
#define NUM_COMMANDS                 (sizeof(handler_table)/sizeof(handler_table[0]))
/*************************************************************************************************
 *                                  LOCAL TYPE DEFINITIONS                                       *
 *************************************************************************************************/
// define how command handlers look like
typedef Command_Status_t (*Command_Handler_t)(const uint8_t command[USBD_CUSTOMHID_OUTREPORT_BUF_SIZE]);

typedef struct
{
   uint8_t           op;
   Command_Handler_t read;
   Command_Handler_t write;
} cmd_handler_t;
/*************************************************************************************************
 *                                GLOBAL VARIABLE DEFINITIONS                                    *
 *************************************************************************************************/
/*************************************************************************************************
 *                                STATIC FUNCTION DECLARATIONS                                   *
 *************************************************************************************************/
static Command_Status_t Command_HandleNoop(const uint8_t command[USBD_CUSTOMHID_OUTREPORT_BUF_SIZE]);
static Command_Status_t CommandVersion_ReadVersion(const uint8_t command[USBD_CUSTOMHID_OUTREPORT_BUF_SIZE]);
static Command_Status_t CommandRecovery_Reset(const uint8_t command[USBD_CUSTOMHID_OUTREPORT_BUF_SIZE]);
static Command_Status_t CommandHcfs_EraseFile(const uint8_t command[USBD_CUSTOMHID_OUTREPORT_BUF_SIZE]);
static Command_Status_t CommandHcfs_WriteFile(const uint8_t command[USBD_CUSTOMHID_OUTREPORT_BUF_SIZE]);
static Command_Status_t CommandHcfs_Crc32File(const uint8_t command[USBD_CUSTOMHID_OUTREPORT_BUF_SIZE]);
static Command_Status_t handle_debug_command(const uint8_t command[USBD_CUSTOMHID_OUTREPORT_BUF_SIZE]);

/*************************************************************************************************
 *                                STATIC VARIABLE DEFINITIONS                                    *
 *************************************************************************************************/
static uint8_t buffer[USBD_CUSTOMHID_OUTREPORT_BUF_SIZE] = {0};
// static uint8_t txBuf[USBD_CUSTOMHID_OUTREPORT_BUF_SIZE] = {0};
extern USBD_HandleTypeDef hUsbDevice;
static const cmd_handler_t handler_table[] =
{
    { .op = NO_OP,                    .read = Command_HandleNoop,                       .write = Command_HandleNoop },
    // mcu control
    { .op = RESET_OP,                 .read = Command_HandleNoop,                       .write = CommandRecovery_Reset },
    // file/firmware update
    { .op = ERASE_FILE_OP,            .read = Command_HandleNoop,                       .write = CommandHcfs_EraseFile },
    { .op = FILE_ACCESS_OP,           .read = Command_HandleNoop,                       .write = CommandHcfs_WriteFile },
    { .op = FILE_CRC32_OP,            .read = CommandHcfs_Crc32File,                    .write = Command_HandleNoop },
    // info
    { .op = VERSION_OP,               .read = CommandVersion_ReadVersion,               .write = Command_HandleNoop },

    // debug
    { .op = DEBUG_CUSTOM_OP,          .read = Command_HandleNoop,                       .write = handle_debug_command },
};

/*************************************************************************************************
 *                                GLOBAL FUNCTION DEFINITIONS                                    *
 *************************************************************************************************/
void Commands_HandleUsbCommand(const uint8_t *in, size_t in_len)
{
    Command_Status_t status = COMMAND_STATUS_ERROR_NO_HANDLER;

    memcpy(buffer, in, in_len);

    uint8_t command = (buffer[0]);
    uint8_t op = command & (~COMMAND_READ_FLAG);
    bool is_read = (command & COMMAND_READ_FLAG) == COMMAND_READ_FLAG;
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

static Command_Status_t Command_HandleNoop(const uint8_t command[USBD_CUSTOMHID_OUTREPORT_BUF_SIZE])
{
    return COMMAND_STATUS_SUCCESS;
}

static Command_Status_t CommandVersion_ReadVersion(const uint8_t command[USBD_CUSTOMHID_OUTREPORT_BUF_SIZE])
{
    uint8_t buff[] = {command[0], command[1]};
    SpiProtocol_SendBuffer(buff, sizeof(buff));
    return COMMAND_STATUS_SUCCESS;
}

static Command_Status_t CommandRecovery_Reset(const uint8_t command[USBD_CUSTOMHID_OUTREPORT_BUF_SIZE])
{
    if (command[1] < RECOVERY_MODE_NUM_MODES)
    {
        if (command[1] == RECOVERY_MODE_BOOTLOADER)
        {
            if (command[2] == FILE_ID_LOCAL)
            {
                SS_RESET_FLAG = true;
            }
            else if (command[2] == FILE_ID_PERIPHERAL)
            {
                uint8_t buff[] = {command[0], command[1], command[2]};
                SpiProtocol_SendBuffer(buff, sizeof(buff));
            }
        }
        else if (command[1] == RECOVERY_MODE_APPLICATION)
        {
            if (command[2] == FILE_ID_LOCAL)
            {
                SS_RESET_FLAG = true;
            }
            else if (command[2] == FILE_ID_PERIPHERAL)
            {
                uint8_t buff[] = {command[0], command[1], command[2]};
                SpiProtocol_SendBuffer(buff, sizeof(buff));
            }
        }
    }
    else
    {
        SS_RESET_FLAG = true;
    }
    return COMMAND_STATUS_SUCCESS;
}

static Command_Status_t CommandHcfs_EraseFile(const uint8_t command[USBD_CUSTOMHID_OUTREPORT_BUF_SIZE])
{
    if (command[1] < FILE_ID_NUM_MODES)
    {
        if (command[1] == FILE_ID_PERIPHERAL)
        {
            uint8_t buff[] = {command[0], command[1], command[2]};
            SpiProtocol_SendBuffer(buff, sizeof(buff));
        }
    }
    return COMMAND_STATUS_SUCCESS;
}

static Command_Status_t CommandHcfs_WriteFile(const uint8_t command[USBD_CUSTOMHID_OUTREPORT_BUF_SIZE])
{
    if (command[1] < FILE_ID_NUM_MODES)
    {
        if (command[1] == FILE_ID_PERIPHERAL)
        {
            uint8_t buff[SPI_BUFFERSIZE] = {0x00};
            memcpy(buff, command, SPI_BUFFERSIZE);
            SpiProtocol_SendBuffer(buff, SPI_BUFFERSIZE - 1);
        }
    }
    return COMMAND_STATUS_SUCCESS;
}

static Command_Status_t CommandHcfs_Crc32File(const uint8_t command[USBD_CUSTOMHID_OUTREPORT_BUF_SIZE])
{
    if (command[1] < FILE_ID_NUM_MODES)
    {
        if (command[1] == FILE_ID_PERIPHERAL)
        {
            uint8_t buff[] = {command[0], command[1], command[2]};
            SpiProtocol_SendBuffer(buff, 28);
        }
    }
    return COMMAND_STATUS_SUCCESS;
}

static Command_Status_t handle_debug_command(const uint8_t command[USBD_CUSTOMHID_OUTREPORT_BUF_SIZE])
{
    switch (command[1])
    {
    case 0x01:
    {
        SpiProtocol_SendBuffer(&command[2], SPI_BUFFERSIZE - 2);
        break;
    }
    
    default:
        break;
    }
    return COMMAND_STATUS_SUCCESS;
}


