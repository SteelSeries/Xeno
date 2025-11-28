/*************************************************************************************************
 *                                         INCLUDES                                              *
 *************************************************************************************************/
#include "Commands_spi.h"
#include "Commands.h"
#include "spi_protocol.h"
#include "spi.h"
#include "version.h"

/*************************************************************************************************
 *                                  LOCAL MACRO DEFINITIONS                                      *
 *************************************************************************************************/
#define NUM_COMMANDS                 (sizeof(handler_table)/sizeof(handler_table[0]))

/*************************************************************************************************
 *                                  LOCAL TYPE DEFINITIONS                                       *
 *************************************************************************************************/
// define how command handlers look like
typedef Command_Status_t (*Command_Handler_t)(const uint8_t command[SPI_BUFFERSIZE]);

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
static Command_Status_t Command_HandleNoop(const uint8_t command[SPI_BUFFERSIZE]);
static Command_Status_t CommandVersion_ReadVersion(const uint8_t command[SPI_BUFFERSIZE]);
static Command_Status_t CommandHcfs_EraseFile(const uint8_t command[SPI_BUFFERSIZE]);
static Command_Status_t CommandHcfs_WriteFile(const uint8_t command[SPI_BUFFERSIZE]);
static Command_Status_t CommandHcfs_Crc32File(const uint8_t command[SPI_BUFFERSIZE]);

/*************************************************************************************************
 *                                STATIC VARIABLE DEFINITIONS                                    *
 *************************************************************************************************/
static uint8_t buffer[SPI_BUFFERSIZE] = {0};
static uint8_t txBuf[USBD_CUSTOMHID_OUTREPORT_BUF_SIZE] = {0};
extern USBD_HandleTypeDef hUsbDevice;
static const cmd_handler_t handler_table[] =
{
    {   .op = NO_OP,                        .read = Command_HandleNoop,                         .write = Command_HandleNoop },
    // info
    {   .op = VERSION_OP,                   .read = CommandVersion_ReadVersion,                 .write = Command_HandleNoop },
    // file/firmware update
    {   .op = ERASE_FILE_OP,                .read = Command_HandleNoop,                         .write = CommandHcfs_EraseFile },
    {   .op = FILE_ACCESS_OP,               .read = Command_HandleNoop,                         .write = CommandHcfs_WriteFile},
    {   .op = FILE_CRC32_OP,                .read = CommandHcfs_Crc32File,                      .write = Command_HandleNoop},
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
static Command_Status_t Command_HandleNoop(const uint8_t command[SPI_BUFFERSIZE])
{
    return COMMAND_STATUS_SUCCESS;
}

static Command_Status_t CommandVersion_ReadVersion(const uint8_t command[SPI_BUFFERSIZE])
{
    uint8_t temp_buff[12] = {0};
    uint8_t ArteryVerBuff[12] = {0};
    memset(txBuf, 0x00, USBD_CUSTOMHID_OUTREPORT_BUF_SIZE);
    txBuf[0] = VERSION_OP | COMMAND_READ_FLAG;
    Version_GetStmVersion(temp_buff, sizeof(temp_buff));
    memcpy(&txBuf[1], temp_buff, 8);
    
    memcpy(ArteryVerBuff, &command[1], 12);
    Version_GetStrVersion(ArteryVerBuff, temp_buff, sizeof(temp_buff));
    memcpy(&txBuf[13], temp_buff, 8);

    USBD_CUSTOM_HID_SendReport(&hUsbDevice, (uint8_t *)txBuf, USBD_CUSTOMHID_OUTREPORT_BUF_SIZE);
    return COMMAND_STATUS_SUCCESS;
}

static Command_Status_t CommandHcfs_EraseFile(const uint8_t command[SPI_BUFFERSIZE])
{
    memset(txBuf, 0x00, USBD_CUSTOMHID_OUTREPORT_BUF_SIZE);
    txBuf[0] = ERASE_FILE_OP;
    txBuf[1] = command[1];
    USBD_CUSTOM_HID_SendReport(&hUsbDevice, (uint8_t *)txBuf, USBD_CUSTOMHID_OUTREPORT_BUF_SIZE);
    return COMMAND_STATUS_SUCCESS;
}

static Command_Status_t CommandHcfs_WriteFile(const uint8_t command[SPI_BUFFERSIZE])
{
    memset(txBuf, 0x00, USBD_CUSTOMHID_OUTREPORT_BUF_SIZE);
    txBuf[0] = FILE_ACCESS_OP;
    txBuf[1] = command[1];
    USBD_CUSTOM_HID_SendReport(&hUsbDevice, (uint8_t *)txBuf, USBD_CUSTOMHID_OUTREPORT_BUF_SIZE);
    return COMMAND_STATUS_SUCCESS;
}

static Command_Status_t CommandHcfs_Crc32File(const uint8_t command[SPI_BUFFERSIZE])
{
    memset(txBuf, 0x00, USBD_CUSTOMHID_OUTREPORT_BUF_SIZE);
    txBuf[0] = FILE_CRC32_OP | COMMAND_READ_FLAG;
    memcpy(&txBuf[1], &command[1], 9);
    USBD_CUSTOM_HID_SendReport(&hUsbDevice, (uint8_t *)txBuf, USBD_CUSTOMHID_OUTREPORT_BUF_SIZE);
    return COMMAND_STATUS_SUCCESS;
}

