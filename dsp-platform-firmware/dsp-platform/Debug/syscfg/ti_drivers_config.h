/*
 *  ======== ti_drivers_config.h ========
 *  Configured TI-Drivers module declarations
 *
 *  The macros defines herein are intended for use by applications which
 *  directly include this header. These macros should NOT be hard coded or
 *  copied into library source code.
 *
 *  Symbols declared as const are intended for use with libraries.
 *  Library source code must extern the correct symbol--which is resolved
 *  when the application is linked.
 *
 *  DO NOT EDIT - This file is generated for the LP_EM_CC2340R5
 *  by the SysConfig tool.
 */
#ifndef ti_drivers_config_h
#define ti_drivers_config_h

#define CONFIG_SYSCONFIG_PREVIEW

#define CONFIG_LP_EM_CC2340R5
#ifndef DeviceFamily_CC23X0R5
#define DeviceFamily_CC23X0R5
#endif

#include <ti/devices/DeviceFamily.h>

#include <stdint.h>

/* support C++ sources */
#ifdef __cplusplus
extern "C" {
#endif


/*
 *  ======== ADC ========
 */

extern const uint_least8_t              CONFIG_ADC_0_CONST;
#define CONFIG_ADC_0                    0
#define CONFIG_TI_DRIVERS_ADC_COUNT     1


/*
 *  ======== GPIO ========
 */
/* Owned by CONFIG_ADC_0_CHANNEL as  */
extern const uint_least8_t CONFIG_GPIO_ADC_0_CHANNEL_CONST;
#define CONFIG_GPIO_ADC_0_CHANNEL 0

extern const uint_least8_t CONFIG_DAC_GPIO_0_CONST;
#define CONFIG_DAC_GPIO_0 12

extern const uint_least8_t CONFIG_DAC_GPIO_1_CONST;
#define CONFIG_DAC_GPIO_1 13

extern const uint_least8_t CONFIG_DAC_GPIO_2_CONST;
#define CONFIG_DAC_GPIO_2 19

extern const uint_least8_t CONFIG_DAC_GPIO_3_CONST;
#define CONFIG_DAC_GPIO_3 11

extern const uint_least8_t CONFIG_DAC_GPIO_4_CONST;
#define CONFIG_DAC_GPIO_4 21

extern const uint_least8_t CONFIG_DAC_GPIO_5_CONST;
#define CONFIG_DAC_GPIO_5 6

extern const uint_least8_t CONFIG_DAC_GPIO_6_CONST;
#define CONFIG_DAC_GPIO_6 14

extern const uint_least8_t CONFIG_DAC_GPIO_7_CONST;
#define CONFIG_DAC_GPIO_7 15

extern const uint_least8_t CONFIG_TEST_GPIO_CONST;
#define CONFIG_TEST_GPIO 9

/* Owned by CONFIG_UART2_0 as  */
extern const uint_least8_t CONFIG_GPIO_UART2_0_TX_CONST;
#define CONFIG_GPIO_UART2_0_TX 20

/* Owned by CONFIG_UART2_0 as  */
extern const uint_least8_t CONFIG_GPIO_UART2_0_RX_CONST;
#define CONFIG_GPIO_UART2_0_RX 22

/* The range of pins available on this device */
extern const uint_least8_t GPIO_pinLowerBound;
extern const uint_least8_t GPIO_pinUpperBound;

/* LEDs are active high */
#define CONFIG_GPIO_LED_ON  (1)
#define CONFIG_GPIO_LED_OFF (0)

#define CONFIG_LED_ON  (CONFIG_GPIO_LED_ON)
#define CONFIG_LED_OFF (CONFIG_GPIO_LED_OFF)


/*
 *  ======== LGPTimer ========
 */

/*
 *  CH2: Unassigned
 *  CH2N: Unassigned
 */
extern const uint_least8_t                  CONFIG_LGPTIMER_0_CONST;
#define CONFIG_LGPTIMER_0                   0
/*
 *  CH2: Unassigned
 *  CH2N: Unassigned
 */
extern const uint_least8_t                  CONFIG_LGPTIMER_1_CONST;
#define CONFIG_LGPTIMER_1                   1
#define CONFIG_TI_DRIVERS_LGPTIMER_COUNT    2


/*
 *  ======== UART2 ========
 */

/*
 *  TX: DIO20A11
 *  RX: DIO22A9
 *  XDS110 UART
 */
extern const uint_least8_t                  CONFIG_UART2_0_CONST;
#define CONFIG_UART2_0                      0
#define CONFIG_TI_DRIVERS_UART2_COUNT       1


/*
 *  ======== Board_init ========
 *  Perform all required TI-Drivers initialization
 *
 *  This function should be called once at a point before any use of
 *  TI-Drivers.
 */
extern void Board_init(void);

/*
 *  ======== Board_initGeneral ========
 *  (deprecated)
 *
 *  Board_initGeneral() is defined purely for backward compatibility.
 *
 *  All new code should use Board_init() to do any required TI-Drivers
 *  initialization _and_ use <Driver>_init() for only where specific drivers
 *  are explicitly referenced by the application.  <Driver>_init() functions
 *  are idempotent.
 */
#define Board_initGeneral Board_init

#ifdef __cplusplus
}
#endif

#endif /* include guard */
