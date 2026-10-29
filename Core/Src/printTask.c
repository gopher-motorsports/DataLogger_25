/* ==================================================================== */
/* ============================= INCLUDES ============================= */
/* ==================================================================== */

#include "printTask.h"
#include "sd.h"
#include "GopherCAN.h"

/* ==================================================================== */
/* =================== GLOBAL FUNCTION DEFINITIONS ==================== */
/* ==================================================================== */

void initPrintTask()
{
    printf("\e[1;1H\e[2J");
}

void runPrintTask()
{
    // Clear terminal output
    printf("\e[1;1H\e[2J");
    float data1 = wheelSpeedFrontLeft_mph.data;
    float data2 = wheelSpeedFrontRight_mph.data;
    float data3 = wheelSpeedRearLeft_mph.data;
    float data4 = wheelSpeedRearRight_mph.data;
    float data5 = radWaterTempIn_C.data;
    float data6 = radWaterTempOut_C.data;
    float data7 = brakeLightOn_state.data;
    float data8 = shockPosRearLeft_mm.data;
    float data9 = shockPosRearRight_mm.data;
    printf("data1: %3.2f\n", data1);
    printf("data2: %3.2f\n", data2);
    printf("data3: %3.2f\n", data3);
    printf("data4: %3.2f\n", data4);
    printf("data5: %3.2f\n", data5);
    printf("data6: %3.2f\n", data6);
    printf("data7: %3.2f\n", data7);
    printf("data8: %3.2f\n", data8);
    printf("data9: %3.2f\n", data9);

    // sd_init();
}
