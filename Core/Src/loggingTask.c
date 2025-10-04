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
extern UART_HandleTypeDef huart1;


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

// RADIO BUFFER
static uint8_t RADIO_B[TM_RADIO_BUFFER_SIZE]; // radio buffer in memory?
static TM_BUFFER RADIO_SINGLE = { // TM_BUFFER struct that holds buffer params
    .bytes = RADIO_B, /* pointer variable to buffer, remember name of an array is the same as 
    &RADIO_B[0]*/
    .size = TM_RADIO_BUFFER_SIZE,
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
    // Floating Average coin cell voltage samples
	uint32_t adc_total = 0;
	for (size_t i = 0; i < ADC_VBAT_BUF_SIZE; i++) {
		adc_total += ADC_VBAT_BUF[i];
	}
	float adc_avg = (float) adc_total / ADC_VBAT_BUF_SIZE;
	tm_CoinBattery_V.data = adc_avg / 4096.0f * 3.3f;
	tm_CoinBattery_V.info.last_rx = HAL_GetTick();

    // Collect Data
    static uint32_t sd_last_log[NUM_OF_PARAMETERS] = {0};
    static uint32_t radio_last_tx[NUM_OF_PARAMETERS] = {0};
    
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
        
        //for single radio buffer
        if (param->last_rx > radio_last_tx[i] && (tick - radio_last_tx[i]) > TM_RADIO_TX_DELAY) {
			// parameter has been updated and hasn't been sent by the radio in a while
			// create packet and add to radio buffer
			bool res = tm_data_record(&RADIO_SINGLE, param);
			if (res) {
				radio_last_tx[i] = tick;
			} else {
				tm_RadioPacketsDropped_ul.data += 1;
			}
		}

	}

	float fillLevel = (float) SD_DB.buffers[SD_DB.write_index]->fill / SD_DB.buffers[SD_DB.write_index]->size * 100.0f;
    update_and_queue_param_float(&tm_SDBufferFill_percent, fillLevel);
    fillLevel = (float) RADIO_SINGLE.fill / RADIO_SINGLE.size * 100.0f;
    update_and_queue_param_float(&tm_RadioBufferFill_percent, fillLevel);

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

    static bool tx_in_progress = false;

	if (!tx_in_progress) {
		// waiting for a transfer to radio
		TM_BUFFER* buffer = (TM_BUFFER*) RADIO_B;
		if (buffer->fill > 0) {
			HAL_UART_Transmit_DMA(&huart1, buffer->bytes, buffer->fill);
			tx_in_progress = true;
		}
	}

    if (tx_in_progress) {
        printf("transmission in progress");
		taskENTER_CRITICAL();
		tm_RadioBytesTransferred_bytes.data += RADIO_SINGLE.fill;
		RADIO_SINGLE.fill = 0;
		tx_in_progress = false;
		taskEXIT_CRITICAL();
	}
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef* huart) {
    SD_DB.tx_cplt = false;
}
