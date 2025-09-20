#include <stdbool.h>
#include "telemetry.h"
#include "stm32f4xx_hal.h"
#include "main.h"
#include "cmsis_os.h"
#include "GopherCAN.h"
#include "GopherCAN_network.h"

extern UART_HandleTypeDef huart1;
extern CAN_HandleTypeDef hcan1;
extern CAN_HandleTypeDef hcan2;
extern ADC_HandleTypeDef hadc1;
extern RTC_HandleTypeDef hrtc;

extern TM_DBL_BUFFER SD_DB;

#define TM_DELAY_SERVICE_CAN 1

#define TM_DELAY_COLLECT_DATA 1

#define TM_DELAY_TRANSMIT_DATA 100

#define TM_RADIO_TX_DELAY 100

TM_BUFFER RADIO_DB;

void telemetry_init(){
uint32_t tick = HAL_GetTick();
}


void tm_service_can() {
    service_can_tx(&hcan1);
    service_can_tx(&hcan2);
    service_can_rx_buffer();

    osDelay(TM_DELAY_SERVICE_CAN);
}

void tm_collect_data() {
	static uint32_t sd_last_log[NUM_OF_PARAMETERS] = {0};
	static uint32_t radio_last_tx[NUM_OF_PARAMETERS] = {0};

	for (uint8_t i = 1; i < NUM_OF_PARAMETERS; i++) {
		CAN_INFO_STRUCT* param = PARAMETERS[i];
		uint32_t tick = HAL_GetTick();

		if (param->last_rx > sd_last_log[i]) {
			// parameter has been updated
			// create packet and add to SD buffer
            
            //tm_data_record defined in data.c, takes in a TM_BUFFER* and pointer to CAN_INFO_STRUCT 
			TM_RES res = tm_data_record(SD_DB.buffers[SD_DB.write_index], param);
			//checks if the data packet was actually recorded
            if (res == TM_OK) {
				sd_last_log[i] = tick;
			} else {
				tm_SDPacketsDropped_ul.data += 1;
			}
		}

		if (param->last_rx > radio_last_tx[i] && (tick - radio_last_tx[i]) > TM_RADIO_TX_DELAY) {
			// parameter has been updated and hasn't been sent in a while
			// create packet and add to radio buffer
			TM_RES res = tm_data_record(RADIO_DB, param);
			if (res == TM_OK) {
				radio_last_tx[i] = tick;
			} else {
				tm_RadioPacketsDropped_ul.data += 1;
			}
		}
	}

	// tm_SDBufferFill_percent.data = (float) SD_DB.buffers[SD_DB.write_index]->fill / SD_DB.buffers[SD_DB.write_index]->size * 100.0f;
	// tm_RadioBufferFill_percent.data = (float) RADIO_DB.buffers[RADIO_DB.write_index]->fill / RADIO_DB.buffers[RADIO_DB.write_index]->size * 100.0f;

	osDelay(TM_DELAY_COLLECT_DATA);
}


void tm_transmit_data() {
	static bool tx_in_progress = false;

	if (!tx_in_progress) {
		// waiting for a transfer to radio
		TM_BUFFER* buffer = &RADIO_DB;
		if (buffer->fill > 0) {
            //If theres anything in the buffer, we send it over UART
			HAL_UART_Transmit_DMA(&huart1, buffer->bytes, buffer->fill);
			tx_in_progress = true;
		} else {
			// nothing to transfer
			tx_in_progress = false;
            // RADIO_DB.tx_cplt = 1;
		}
	}


	osDelay(TM_DELAY_TRANSMIT_DATA);
}