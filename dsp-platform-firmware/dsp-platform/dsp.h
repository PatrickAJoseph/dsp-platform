
#ifndef DSP_H
#define DSP_H

/**
 *  DSP.h
 * 
 *  The DSP context is controlled through this software component.
 *  This software component samples the value read through the ADC pin,
 *  executes the user-defined DSP algorithm and then stores the result in
 *  an output buffer.
 *  The samples present in the input buffer can be accessed by calling
 *  \ref DSP_getInputSample.
 *  The processed samples present in the output buffer can be accessed
 *  by calling \ref DSP_getOutputSample.
 *  The sampling rate of the DSP processing loop can be controlled by
 *  calling \ref DSP_setLooFrequency().
 *  The user-defined DSP algorithm is defined by calling \ref DSP_setFunction.
 */

#include <stdint.h>

#define DSP_SAMPLE_SIZE     (4096U)

typedef uint8_t (*dsp_function_t)(uint8_t sample, void* args);

extern void DSP_init();
extern void DSP_setLoopFrequency(int frequency);
extern void DSP_setFunction(dsp_function_t function, void* args);
extern void DSP_reset();
extern void DSP_start();
extern void DSP_stop();
extern int DSP_numberOfSamples();
extern int DSP_getSampleBufferSize();
extern uint8_t DSP_getInputSample(int index);
extern uint8_t DSP_getOutputSample(int index);

#endif /* DSP_H */