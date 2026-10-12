#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <ti/drivers/UART2.h>

#include "remote.h"
#include "serial_rpc.h"

#include "ti_drivers_config.h"

/* Forward declaration of RPC handle. */
static serial_rpc_handle_t remote;

static UART2_Handle remoteUartHandle;
static UART2_Params remoteUartParams;

static void uartRemoteCallback(UART2_Handle handle, void *buf, size_t count, void *userArg, int_fast16_t status)
{
    size_t index;
    uint8_t* data = buf;

    for(index = 0 ; index < count ; index++)
    {
        serial_rpc_handle_rx_byte(&remote, data[index]);
    }
}

static void uartRemoteSend(uint8_t* data, size_t length)
{
    int status;

    status = UART2_write(remoteUartHandle, data, length, NULL);

    if(status)
    {
        __asm volatile("bkpt 0");
    }
}

void Remote_init()
{
    UART2_Params_init(&remoteUartParams);

    remoteUartParams.readMode = UART2_Mode_CALLBACK;
    remoteUartParams.readCallback = uartRemoteCallback;
    remoteUartParams.baudRate = 460800;

    remoteUartHandle = UART2_open(CONFIG_UART2_0, &remoteUartParams);

    if(!remoteUartHandle)
    {
        __asm volatile("bkpt 0");
    }

    serial_rpc_init(&remote);
}

static uint8_t Remote_rxByte;

void Remote_process()
{
    (void)UART2_read(remoteUartHandle, &Remote_rxByte, 1, NULL);
    serial_rpc_process(&remote);
}

/************************************** Application code starts here. **************************/

#include "dsp.h"
#include "dds.h"

/* Remote callback function definitions. */
static SERIAL_RPC_RESPONSE_CALLBACK_DEFINE(ping);
static SERIAL_RPC_RESPONSE_CALLBACK_DEFINE(input_signal_control);
static SERIAL_RPC_RESPONSE_CALLBACK_DEFINE(dsp_control);
static SERIAL_RPC_RESPONSE_CALLBACK_DEFINE(dsp_status);
static SERIAL_RPC_RESPONSE_CALLBACK_DEFINE(dsp_get_samples);

/* Response callback table. */
static SERIAL_RPC_RESPONSE_CALLBACK_TABLE_DEFINE(remote_callback_table) =
{
    SERIAL_RPC_RESPONSE_CALLBACK_TABLE_ENTRY_DEFINE(0, ping, NULL),
    SERIAL_RPC_RESPONSE_CALLBACK_TABLE_ENTRY_DEFINE(1, input_signal_control, NULL),
    SERIAL_RPC_RESPONSE_CALLBACK_TABLE_ENTRY_DEFINE(2, dsp_control, NULL),
    SERIAL_RPC_RESPONSE_CALLBACK_TABLE_ENTRY_DEFINE(3, dsp_status, NULL),
    SERIAL_RPC_RESPONSE_CALLBACK_TABLE_ENTRY_DEFINE(4, dsp_get_samples, NULL),
    SERIAL_RPC_RESPONSE_CALLBACK_TABLE_END
};

static SERIAL_RPC_HANDLE_DEFINE(remote, 0x00, remote_callback_table, uartRemoteSend);

static int ping_counter;

static SERIAL_RPC_RESPONSE_CALLBACK_DEFINE(ping)
{
    static uint8_t reset;

    SERIAL_RPC_REQUEST_PAYLOAD_GET_UINT8( (&remote), 0, (&reset) );

    if(reset)
    {
        ping_counter = 0;
    }

    ping_counter++;

    SERIAL_RPC_RESPONSE_SET_PAYLOAD_LENGTH((&remote), 4);
    SERIAL_RPC_RESPONSE_PAYLOAD_SET_UINT32( (&remote), 0, ping_counter );
}

static SERIAL_RPC_RESPONSE_CALLBACK_DEFINE(input_signal_control)
{
    uint32_t frequency;

    SERIAL_RPC_REQUEST_PAYLOAD_GET_UINT16( (&remote), 0, (&frequency) );

    DDS_setFrequency(frequency);

    SERIAL_RPC_RESPONSE_SET_PAYLOAD_LENGTH( (&remote), 1 );
    SERIAL_RPC_RESPONSE_PAYLOAD_SET_UINT8( (&remote), 0, 0 );
}

static SERIAL_RPC_RESPONSE_CALLBACK_DEFINE(dsp_control)
{
    uint8_t start;
    uint8_t stop;
    uint8_t reset;
    uint8_t set_sampling_rate;
    uint16_t sampling_rate;
    uint8_t set_number_of_samples;
    uint16_t number_of_samples;

    SERIAL_RPC_REQUEST_PAYLOAD_GET_UINT8( (&remote), 0, (&start) );
    SERIAL_RPC_REQUEST_PAYLOAD_GET_UINT8( (&remote), 1, (&stop) );
    SERIAL_RPC_REQUEST_PAYLOAD_GET_UINT8( (&remote), 2, (&reset) );
    SERIAL_RPC_REQUEST_PAYLOAD_GET_UINT8( (&remote), 3, (&set_sampling_rate));
    SERIAL_RPC_REQUEST_PAYLOAD_GET_UINT16( (&remote), 4, (&sampling_rate) );
    SERIAL_RPC_REQUEST_PAYLOAD_GET_UINT8( (&remote), 6, (&set_number_of_samples) );
    SERIAL_RPC_REQUEST_PAYLOAD_GET_UINT16( (&remote), 7, (&number_of_samples)  );

    if(stop)
    {
        DSP_stop();
    }

    if(reset)
    {
        DSP_reset();
    }

    if(set_sampling_rate)
    {
        DSP_setLoopFrequency(sampling_rate);
    }

    if(set_number_of_samples)
    {
        DSP_setNumberOfSamples(number_of_samples);
    }

    if(start)
    {
        DSP_start();
    }

    SERIAL_RPC_RESPONSE_SET_PAYLOAD_LENGTH( (&remote), 1);
    SERIAL_RPC_RESPONSE_PAYLOAD_SET_UINT8( (&remote), 0, 0 );
}

static SERIAL_RPC_RESPONSE_CALLBACK_DEFINE(dsp_status)
{
    uint16_t number_of_samples;
    uint16_t buffer_size;

    number_of_samples = DSP_numberOfSamples();
    buffer_size = DSP_getSampleBufferSize();

    SERIAL_RPC_RESPONSE_SET_PAYLOAD_LENGTH( (&remote), 4 );
    SERIAL_RPC_RESPONSE_PAYLOAD_SET_UINT16( (&remote), 0, buffer_size );
    SERIAL_RPC_RESPONSE_PAYLOAD_SET_UINT16( (&remote), 2, number_of_samples );
}

static SERIAL_RPC_RESPONSE_CALLBACK_DEFINE(dsp_get_samples)
{
    uint16_t index;
    uint8_t length;
    uint8_t input;
    uint8_t output;

    SERIAL_RPC_REQUEST_PAYLOAD_GET_UINT16( (&remote), 0, (&index) );
    SERIAL_RPC_REQUEST_PAYLOAD_GET_UINT8( (&remote), 2, (&length) );

    SERIAL_RPC_RESPONSE_SET_PAYLOAD_LENGTH( (&remote), (1 + (2*length)) );
    SERIAL_RPC_RESPONSE_PAYLOAD_SET_UINT8( (&remote), 0, length );

    for(int i = 0 ; i < length ; i++)
    {
        input = DSP_getInputSample(index + (uint16_t)i);
        output = DSP_getOutputSample(index + (uint16_t)i);

        SERIAL_RPC_RESPONSE_PAYLOAD_SET_UINT8( (&remote), (i+1), input );
        SERIAL_RPC_RESPONSE_PAYLOAD_SET_UINT8( (&remote), (i+1+length), output );
    }
}
