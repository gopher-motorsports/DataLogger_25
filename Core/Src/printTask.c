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
    // printf("\e[1;1H\e[2J");
    // float data1 = fvcINS_status.data;
    // float data2 = fvcYaw.data;
    // float data3 = fvcPitch.data;
    // float data4 = fvcRoll.data;
    // float data5 = fvcGyroBodyX.data;
    // float data6 = fvcGyroBodyY.data;
    // float data7 = fvcGyroBodyZ.data;
    // printf("data1: %3.2f\n", data1);
    // printf("data2: %3.2f\n", data2);
    // printf("data3: %3.2f\n", data3);
    // printf("data4: %3.2f\n", data4);
    // printf("data5: %3.2f\n", data5);
    // printf("data6: %3.2f\n", data6);
    // printf("data7: %3.2f\n", data7);

    // sd_init();
}
