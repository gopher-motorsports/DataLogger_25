/* ==================================================================== */
/* ============================= INCLUDES ============================= */
/* ==================================================================== */

#include "sd.h"
#include "main.h"
#include "fatfs.h"
// extern const Diskio_drvTypeDef SD_Driver;

/* ==================================================================== */
/* ======================= EXTERNAL VARIABLES ========================= */
/* ==================================================================== */

extern RTC_HandleTypeDef hrtc;

/* ==================================================================== */
/* =================== GLOBAL FUNCTION DEFINITIONS ==================== */
/* ==================================================================== */
bool sd_debounce(uint32_t debounce_ms){
    static uint32_t lastUpdate = 0;
    static bool initState = false, lastContact = false, stable = false;

    bool state = (HAL_GPIO_ReadPin(SDIO_CD_GPIO_Port, SDIO_CD_Pin) == GPIO_PIN_RESET);
    uint32_t now = HAL_GetTick();

    if (!initState){ // if the sd card is already in 
        initState = true;
        lastContact = state;
        stable = state;
        lastUpdate = now;
        return stable;
    }

    if (state != lastContact){
        lastContact =  state;
        lastUpdate = now;
    }

    if ((now - lastUpdate) >= debounce_ms){
        stable = state;
    }
    return stable;
}


bool sd_init()
{
    
    bool sd_detected = HAL_GPIO_ReadPin(SDIO_CD_GPIO_Port, SDIO_CD_Pin) == GPIO_PIN_RESET; //CD goes low
    
    // if (!sd_debounce(50)) {
    //     printf("No SD card detected (or bouncing)\n");
    //     return false;
    // }

    // MX_SDIO_SD_Init();
    // HAL_Delay(150); 

    if (!sd_detected)
    {
        printf("No SD card detected\n");
        return false;
    }

    // if (FATFS_LinkDriver(&SD_Driver, SDPath) != 0) {
    //     printf("FATFS_LinkDriver failed\n");
    // }

    FRESULT fr = f_mount(&SDFatFS, SDPath, 0);
    // printf("FR --> %d\n", fr);

	// if (f_mount(&SDFatFS, SDPath, 1) != FR_OK)
    if(fr != FR_OK)
	{
        printf("SD card Mount Failed, FRESULT = %d\n", fr);
        sd_deinit();
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
        printf("SD card Open Failed\n");
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
    FRESULT res = f_write(&SDFile, buffer, size, &bytes_written);
    if (res != FR_OK)
    {
        printf("SD card failed to write, FRESULT = %d\n", res);
        return false;
    }
    
    res = f_sync(&SDFile);
    if (res != FR_OK)
    {
        printf("SD card sync fail, FRESULT = %d\n", res);
        return false;
    }

    return true;
}
