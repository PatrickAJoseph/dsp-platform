#include <unistd.h>
#include <stdint.h>
#include <stddef.h>

/* Driver Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/timer/LGPTimerLPF3.h>
#include <ti/devices/cc23x0r5/inc/hw_ints.h>
#include <ti/devices/cc23x0r5/inc/hw_types.h>
#include <ti/devices/cc23x0r5/inc/hw_lgpt.h>
#include <ti/devices/cc23x0r5/inc/hw_memmap.h>
#include <ti/devices/cc23x0r5/inc/hw_gpio.h>
#include <ti/drivers/dpl/HwiP.h>

/* Application header files. */
#include "dds.h"

void *mainThread(void *arg0)
{
    Board_init();

    DDS_init();

    while (1)
    {
        DDS_setFrequency(100);
        sleep(1);
    }
}