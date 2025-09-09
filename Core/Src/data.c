/* ==================================================================== */
/* ============================= INCLUDES ============================= */
/* ==================================================================== */

#include "data.h"

/* ==================================================================== */
/* =================== LOCAL FUNCTION DECLARATIONS ==================== */
/* ==================================================================== */

static void append_byte(TM_BUFFER* buffer, uint8_t byte);

/* ==================================================================== */
/* =================== LOCAL FUNCTION DEFINITIONS ===================== */
/* ==================================================================== */

static void append_byte(TM_BUFFER *buffer, uint8_t byte) {
    // check for a control byte
    if (byte == START_BYTE || byte == ESCAPE_BYTE) {
        // append escape byte
        buffer->bytes[buffer->fill++] = ESCAPE_BYTE;
        // append the desired byte, escaped
        buffer->bytes[buffer->fill++] = byte ^ ESCAPE_XOR;
    } else {
        // append the raw byte
        buffer->bytes[buffer->fill++] = byte;
    }
}

/* ==================================================================== */
/* =================== GLOBAL FUNCTION DEFINITIONS ==================== */
/* ==================================================================== */

bool tm_data_record(TM_BUFFER* buffer, CAN_INFO_STRUCT* param)
{
    uint32_t timestamp = param->last_rx;
    uint16_t id = param->ID;
    void* data = NULL;
    uint8_t checksum = 0;

    // make sure packet will fit
    uint8_t packet_size = 1 + sizeof(timestamp) + sizeof(id) + param->SIZE + sizeof(checksum);
    if (packet_size * 2 > buffer->size - buffer->fill)
        return false;

    // get pointer to data
    switch (param->TYPE) {
        case UNSIGNED8:
            data = &((U8_CAN_STRUCT*)param)->data;
            break;
        case UNSIGNED16:
            data = &((U16_CAN_STRUCT*)param)->data;
            break;
        case UNSIGNED32:
            data = &((U32_CAN_STRUCT*)param)->data;
            break;
        case UNSIGNED64:
            data = &((U64_CAN_STRUCT*)param)->data;
            break;
        case SIGNED8:
            data = &((S8_CAN_STRUCT*)param)->data;
            break;
        case SIGNED16:
            data = &((S16_CAN_STRUCT*)param)->data;
            break;
        case SIGNED32:
            data = &((S32_CAN_STRUCT*)param)->data;
            break;
        case SIGNED64:
            data = &((S64_CAN_STRUCT*)param)->data;
            break;
        case FLOATING:
            data = &((FLOAT_CAN_STRUCT*)param)->data;
            break;
        default:
            return false;
    }

    // begin writing packet to buffer
    buffer->bytes[buffer->fill++] = START_BYTE;
    checksum += START_BYTE;

    // append components with MSB first
    U8* bytes;
	U8 i;

	bytes = (U8*) &(timestamp);
	for (i = sizeof(timestamp); i > 0; i--)
	{
		append_byte(buffer, bytes[i - 1]);
		checksum += bytes[i - 1];
	}

	bytes = (U8*) &(id);
	for (i = sizeof(id); i > 0; i--)
	{
		append_byte(buffer, bytes[i - 1]);
		checksum += bytes[i - 1];
	}

	bytes = (U8*) data;
	for (i = param->SIZE; i > 0; i--)
	{
		append_byte(buffer, bytes[i - 1]);
		checksum += bytes[i - 1];
	}

	append_byte(buffer, checksum);

    return true;
}
