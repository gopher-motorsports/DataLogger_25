
// quick fix to test telemetry functionality

#define TM_RADIO_BUFFER_SIZE 10000

typedef struct {
    uint8_t* bytes;
    size_t size;
    size_t fill;
} TM_BUFFER;

//for this scuff we dont need double buffers
typedef struct {
    TM_BUFFER* buffers[2];
    uint8_t write_index; // 0 or 1, index of write buffer
    bool tx_cplt; // flag set when transfer of read buffer is complete
} TM_DBL_BUFFER;

typedef enum {
    TM_OK     = 0,
    TM_ERR    = 1
} TM_RES;

void tm_transmit_data();
void tm_collect_data();

