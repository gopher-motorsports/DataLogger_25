#ifndef INC_TM_DATA_H_
#define INC_TM_DATA_H_

/* ==================================================================== */
/* ============================= INCLUDES ============================= */
/* ==================================================================== */

#include "GopherCAN.h"
#include <stdbool.h>

/* ==================================================================== */
/* ============================= DEFINES ============================== */
/* ==================================================================== */

// packet control bytes
#define START_BYTE 0x7e // start of packet
#define ESCAPE_BYTE 0x7d // next byte is escaped
#define ESCAPE_XOR 0x20 // escape code

#define TM_SD_BUFFER_SIZE 25000

typedef struct 
{
    uint8_t* bytes;
    size_t size;
    size_t fill;
} TM_BUFFER;

typedef struct 
{
    TM_BUFFER* buffers[2];
    uint8_t write_index; // 0 or 1, index of write buffer
    bool tx_cplt; // flag set when transfer of read buffer is complete
} TM_DBL_BUFFER;

/* ==================================================================== */
/* =================== GLOBAL FUNCTION DEFINITIONS ==================== */
/* ==================================================================== */

bool tm_data_record(TM_BUFFER* buffer, CAN_INFO_STRUCT* param);

#endif /* INC_TM_DATA_H_ */
