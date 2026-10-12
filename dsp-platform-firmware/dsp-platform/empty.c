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
#include <ti/drivers/Power.h>

/* Application header files. */
#include "dds.h"
#include "dsp.h"
#include "remote.h"

static uint8_t sampleDspFunction(uint8_t sample, void* args)
{
    return sample;
}

void *mainThread(void *arg0)
{
    Board_init();
    
    Power_setConstraint(PowerLPF3_DISALLOW_STANDBY);

    GPIO_init();
    ADC_init();

    Remote_init();

    DDS_init();
    DDS_setFrequency(100);

    DSP_init();
    DSP_setFunction(sampleDspFunction, NULL);

    while (1)
    {
        Remote_process();
    }
}