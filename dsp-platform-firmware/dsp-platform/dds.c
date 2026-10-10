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

/* Driver configuration */
#include "ti_drivers_config.h"

#include "sine_lut.h"

static const uint32_t sine_lut[256] = SINE_LUT(255, 256);

LGPTimerLPF3_Params timerParams;
LGPTimerLPF3_Handle timerHandle;

HwiP_Params timerHwiParams;
HwiP_Handle timerHwiHandle;
HwiP_Struct timerHwiStruct;

/**
 *  @brief Function to write 8-bit word to R-2R DAC.
 */
static void dac_write(uint8_t code)
{
    HWREGB(GPIO_BASE + GPIO_O_DOUT3_0 + CONFIG_DAC_GPIO_7) = !!(code & 0x80); 
    HWREGB(GPIO_BASE + GPIO_O_DOUT3_0 + CONFIG_DAC_GPIO_6) = !!(code & 0x40); 
    HWREGB(GPIO_BASE + GPIO_O_DOUT3_0 + CONFIG_DAC_GPIO_5) = !!(code & 0x20); 
    HWREGB(GPIO_BASE + GPIO_O_DOUT3_0 + CONFIG_DAC_GPIO_4) = !!(code & 0x10); 
    HWREGB(GPIO_BASE + GPIO_O_DOUT3_0 + CONFIG_DAC_GPIO_3) = !!(code & 0x08); 
    HWREGB(GPIO_BASE + GPIO_O_DOUT3_0 + CONFIG_DAC_GPIO_2) = !!(code & 0x04); 
    HWREGB(GPIO_BASE + GPIO_O_DOUT3_0 + CONFIG_DAC_GPIO_1) = !!(code & 0x02); 
    HWREGB(GPIO_BASE + GPIO_O_DOUT3_0 + CONFIG_DAC_GPIO_0) = !!(code & 0x01); 
}

/* Phase accumulator. */
volatile uint32_t accumulator = 0;

/* Phase increment. */
volatile uint32_t phase_increment;

/**
 * @brief   Function to calculate the phase increment.
 */

void DDS_setFrequency(uint32_t frequency)
{
    phase_increment = (uint32_t)((((double)( 1ULL << 32 )) * (double)frequency) / (double)50000);
}

/**
 *   @brief         Timer callback function. 
 *   @details       Called every 20us. Executes the DDS algorithm.
 */

static void timerCallback(uintptr_t args)
{
    uint32_t index;

    uint32_t status;

    status = HWREG(LGPT3_BASE + LGPT_O_MIS);

    HWREG(LGPT3_BASE + LGPT_O_ICLR) = status;

    accumulator += phase_increment;

    index = accumulator >> (32 - 8);

    dac_write(sine_lut[index]);
}

void DDS_init()
{
    /* Setup timer. */

    LGPTimerLPF3_Params_init(&timerParams);

    timerParams.prescalerDiv = 1;

    timerHandle = LGPTimerLPF3_open(CONFIG_LGPTIMER_0, &timerParams);

    if(!timerHandle)
    {
        __asm volatile("bkpt 0");
    }

    LGPTimerLPF3_setInitialCounterTarget(timerHandle, (480 - 1), true);
    LGPTimerLPF3_start(timerHandle, LGPTimerLPF3_CTL_MODE_UP_PER);

    HwiP_Params_init(&timerHwiParams);

    timerHwiParams.enableInt = true;

    timerHwiHandle = HwiP_construct(&timerHwiStruct, INT_LGPT3_COMB, timerCallback, &timerHwiParams);
    
    if(!timerHwiHandle)
    {
        __asm volatile("bkpt 0");
    }

    HwiP_enableInterrupt(INT_LGPT3_COMB);

    HWREG(LGPT3_BASE + LGPT_O_IMASK) = LGPT_IMASK_TGT;
}
