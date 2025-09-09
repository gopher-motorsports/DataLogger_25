/* ==================================================================== */
/* ============================= INCLUDES ============================= */
/* ==================================================================== */

#include "sd.h"
#include "main.h"
#include "fatfs.h"

/* ==================================================================== */
/* ======================= EXTERNAL VARIABLES ========================= */
/* ==================================================================== */

extern RTC_HandleTypeDef hrtc;

/* ==================================================================== */
/* =================== GLOBAL FUNCTION DEFINITIONS ==================== */
/* ==================================================================== */

bool sd_init()
{
    bool sd_detected = HAL_GPIO_ReadPin(SDIO_CD_GPIO_Port, SDIO_CD_Pin) == GPIO_PIN_RESET;
    
    if (!sd_detected)
    {
        // printf("No SD card detected\n");
        return false;
    }

    FRESULT fr = f_mount(&SDFatFS, SDPath, 1);
    // printf("FR --> %d\n", fr);

	// if (f_mount(&SDFatFS, SDPath, 1) != FR_OK)
    if(fr != FR_OK)
	{
        // printf("SD card Mount Failed\n");
        return false;
    }


	// must call GetTime() before GetDate() according to HAL docs
	RTC_TimeTypeDef time;
	RTC_DateTypeDef date;
	HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);

	char filename[] = "YYYY-MM-DD-hh-mm-ss.gdat";
	sprintf(filename, "20%u-%02u-%02u-%02u-%02u-%02u.gdat", date.Year, date.Month, date.Date, time.Hours, time.Minutes, time.Seconds);

    if (f_open(&SDFile, filename, FA_WRITE | FA_CREATE_NEW) != FR_OK)
    {
        // printf("SD card Open Failed\n");
        return false;
    }

    printf("opened: %s\n", filename);

    if (f_printf(&SDFile, "/%s:\n", filename) <= 0)
    {
        printf("SD card print failed\n");
        return false;
    }

    return true;
}

void sd_deinit()
{
    f_close(&SDFile);
    f_mount(NULL, SDPath, 0);
}

bool sd_write(uint8_t* buffer, uint16_t size){
    unsigned int bytes_written = 0;
    if (f_write(&SDFile, buffer, size, &bytes_written) != FR_OK)
    {
        printf("SD card failed to write\n");
        return false;
    }
    	
    if (f_sync(&SDFile) != FR_OK)
    {
        printf("SD card sync fail\n");
        return false;
    }

    return true;
}
