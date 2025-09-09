/* ==================================================================== */
/* ============================= INCLUDES ============================= */
/* ==================================================================== */

#include <stdint.h>
#include "loggingTask.h"
#include "main.h"
#include "data.h"
#include "sd.h"
#include "GopherCAN.h"
#include "gopher_sense.h"

/* ==================================================================== */
/* ============================= DEFINES ============================== */
/* ==================================================================== */

extern ADC_HandleTypeDef hadc1;


// buffer for ADC_VBAT samples (coin cell battery)
#define ADC_VBAT_BUF_SIZE 10
uint16_t ADC_VBAT_BUF[ADC_VBAT_BUF_SIZE];

// SD DOUBLE BUFFER
static uint8_t b1[TM_SD_BUFFER_SIZE];
static TM_BUFFER buffer1 = {
    .bytes = b1,
    .size = TM_SD_BUFFER_SIZE,
    .fill = 0
};

static uint8_t b2[TM_SD_BUFFER_SIZE];
static TM_BUFFER buffer2 = {
    .bytes = b2,
    .size = TM_SD_BUFFER_SIZE,
    .fill = 0
};

TM_DBL_BUFFER SD_DB = {
    .buffers = { &buffer1, &buffer2 },
    .write_index = 0,
    .tx_cplt = 1
};

/* ==================================================================== */
/* =================== LOCAL FUNCTION DECLARATIONS ==================== */
/* ==================================================================== */

/* ==================================================================== */
/* =================== LOCAL FUNCTION DEFINITIONS ===================== */
/* ==================================================================== */

/* ==================================================================== */
/* =================== GLOBAL FUNCTION DEFINITIONS ==================== */
/* ==================================================================== */

void initLoggingTask()
{
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)ADC_VBAT_BUF, ADC_VBAT_BUF_SIZE);
}

void runLoggingTask()
{
    // Average coin cell voltage samples
	uint32_t adc_total = 0;
	for (size_t i = 0; i < ADC_VBAT_BUF_SIZE; i++) {
		adc_total += ADC_VBAT_BUF[i];
	}
	float adc_avg = (float) adc_total / ADC_VBAT_BUF_SIZE;
	tm_CoinBattery_V.data = adc_avg / 4096.0f * 3.3f;
	tm_CoinBattery_V.info.last_rx = HAL_GetTick();

    // Collect Data
    static uint32_t sd_last_log[NUM_OF_PARAMETERS] = {0};

	for (uint32_t i = 1; i < NUM_OF_PARAMETERS; i++) {
		CAN_INFO_STRUCT* param = PARAMETERS[i];
		uint32_t tick = HAL_GetTick();

		if (param->last_rx > sd_last_log[i]) {
			// parameter has been updated
			// create packet and add to SD buffer
			bool res = tm_data_record(SD_DB.buffers[SD_DB.write_index], param);
			if (res) {
				sd_last_log[i] = tick;
			} else {
				tm_SDPacketsDropped_ul.data += 1;
			}
		}

	}

	float fillLevel = (float) SD_DB.buffers[SD_DB.write_index]->fill / SD_DB.buffers[SD_DB.write_index]->size * 100.0f;
    update_and_queue_param_float(&tm_SDBufferFill_percent, fillLevel);

    static uint32_t dataCollectCount = 0;
    dataCollectCount++;
    if(dataCollectCount >= 99)
    {
        dataCollectCount = 0;

        static bool sd_ready = 0;
        if (!sd_ready) {
            if (!sd_init())
            {
                sd_deinit();
            }	
            else
                sd_ready = 1;
        }

        if (sd_ready && !SD_DB.tx_cplt) {
            // FatFs initialized, waiting for a transfer
            TM_BUFFER* buffer = SD_DB.buffers[!SD_DB.write_index];
            if (buffer->fill > 0) {
                if (!sd_write(buffer->bytes, buffer->fill)) {
                    // write failed
                    sd_ready = 0;
                    sd_deinit();
                } else {
                    // successful write
                    SD_DB.tx_cplt = 1;
                }
            } else {
                // nothing to transfer
                SD_DB.tx_cplt = 1;
            }
        }

        // swap buffers after transfer is complete
        // critical section entry/exit is fast and fine for a quick swap
        if (SD_DB.tx_cplt) {
            taskENTER_CRITICAL();
            tm_SDBytesTransferred_bytes.data += SD_DB.buffers[!SD_DB.write_index]->fill;
            SD_DB.buffers[!SD_DB.write_index]->fill = 0;
            SD_DB.write_index = !SD_DB.write_index;
            SD_DB.tx_cplt = 0;
            taskEXIT_CRITICAL();
        }

    }
}