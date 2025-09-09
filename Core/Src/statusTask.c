/* ==================================================================== */
/* ============================= INCLUDES ============================= */
/* ==================================================================== */

#include <stdint.h>
#include "statusTask.h"
#include "main.h"

/* ==================================================================== */
/* ============================= DEFINES ============================== */
/* ==================================================================== */

#define HEARTBEAT_BLINK_MS      300
#define HEARTBEAT_PERIOD_MS     1000

/* ==================================================================== */
/* =================== LOCAL FUNCTION DECLARATIONS ==================== */
/* ==================================================================== */

static void updateHeartbeat();

/* ==================================================================== */
/* =================== LOCAL FUNCTION DEFINITIONS ===================== */
/* ==================================================================== */

static void updateHeartbeat()
{
    static uint8_t hbState = 0;
    static uint32_t lastHeartBeatUpdate = 0;
    if(hbState)
    {
        if((HAL_GetTick() - lastHeartBeatUpdate) > HEARTBEAT_BLINK_MS)
        {
            HAL_GPIO_WritePin(MCU_HBEAT_GPIO_Port, MCU_HBEAT_Pin, GPIO_PIN_RESET);
            hbState = 0;
            lastHeartBeatUpdate = HAL_GetTick();
        }
    }
    else
    {
        if((HAL_GetTick() - lastHeartBeatUpdate) > (HEARTBEAT_PERIOD_MS - HEARTBEAT_BLINK_MS))
        {
            HAL_GPIO_WritePin(MCU_HBEAT_GPIO_Port, MCU_HBEAT_Pin, GPIO_PIN_SET);
            hbState = 1;
            lastHeartBeatUpdate = HAL_GetTick();
        }
    }
}

/* ==================================================================== */
/* =================== GLOBAL FUNCTION DEFINITIONS ==================== */
/* ==================================================================== */

void initStatusTask()
{
    HAL_GPIO_WritePin(MCU_HBEAT_GPIO_Port, MCU_HBEAT_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MCU_FAULT_GPIO_Port, MCU_FAULT_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MCU_GSENSE_GPIO_Port, MCU_GSENSE_Pin, GPIO_PIN_RESET);
}

void runStatusTask()
{
    // Update hearbeat led
    updateHeartbeat();
}