
#ifndef __SERIAL_RPC_H__
#define __SERIAL_RPC_H__

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#ifndef SERIAL_RPC_LOG_ENABLE
#define SERIAL_RPC_LOG(x, ...)
#else
#define SERIAL_RPC_LOG(x, ...)      printf("[SERIAL RPC LOG] %s:%d ==> " #x "\n", __FILE__, __LINE__, ##__VA_ARGS__);
#endif /* SERIAL_RPC_LOG_ENABLE */

#define SERIAL_RPC_PACKET_SIZE                  (32U)
#define SERIAL_RPC_NOTIFICATION_BUFFER_SIZE     (32U)

#define SERIAL_RPC_PACKET_TYPE_REQUEST              (0U)
#define SERIAL_RPC_PACKET_TYPE_RESPONSE             (1U)
#define SERIAL_RPC_PACKET_TYPE_NOTIFICATION         (2U)
#define SERIAL_RPC_PACKET_TYPE_CONTROL_AND_STATUS   (3U)

/******************************* Helper macro functions **************************/

#define SERIAL_RPC_RESPONSE_CALLBACK_TABLE_ENTRY_DEFINE(__index__, __callback__, __args__)      \
{                                                                                               \
    .index          =   __index__,                                                              \
    .callback       =   __callback__,                                                           \
    .args           =   __args__,                                                               \
}

#define SERIAL_RPC_RESPONSE_CALLBACK_TABLE_END                                                  \
    SERIAL_RPC_RESPONSE_CALLBACK_TABLE_ENTRY_DEFINE(0, NULL, NULL)

#define SERIAL_RPC_RESPONSE_CALLBACK_DEFINE(__name__)                                           \
void __name__(serial_rpc_handle_t* handle)

#define SERIAL_RPC_RESPONSE_CALLBACK_TABLE_DEFINE(__name__)                                     \
serial_rpc_response_callback_table_entry_t __name__[]

#define SERIAL_RPC_HANDLE_DEFINE(__name__, __address__, __callback_table__, __send__)           \
serial_rpc_handle_t __name__ =                                                                  \
{                                                                                               \
    .rx_buffer_index = 0,                                                                       \
    .response_callback_table = __callback_table__,                                              \
    .processing_response = false,                                                               \
    .send = __send__,                                                                           \
    .target_address = __address__,                                                              \
};

#define SERIAL_RPC_PACKET_INDEX_LOW(x)              \
    (x & 0xFF)

#define SERIAL_RPC_PACKET_INDEX_HIGH(x)             \
    ((x >> 8) & 0x3)

#define SERIAL_RPC_PACKET_INDEX(_hi, _lo)           \
    ( (((uint16_t)(_hi)) << 8) | ((uint16_t)(_lo)) )

#define SERIAL_RPC_PACKET_TYPE_ENUM_STRING(x)                                                   \
    (x == SERIAL_RPC_PACKET_TYPE_REQUEST) ? "REQUEST"     :                                     \
    (x == SERIAL_RPC_PACKET_TYPE_RESPONSE) ? "RESPONSE"   :                                     \
    (x == SERIAL_RPC_PACKET_TYPE_NOTIFICATION) ? "NOTIFICATION"   :                             \
    (x == SERIAL_RPC_PACKET_TYPE_CONTROL_AND_STATUS) ? "CONTROL AND STATUS" : "UNKNOWN"         \

#define SERIAL_RPC_PACKET_PRINT_INFO(x)                                                         \
{                                                                                               \
    int payload_byte_index;                                                                     \
                                                                                                \
    SERIAL_RPC_LOG("Serial RPC packet information:");                                           \
    SERIAL_RPC_LOG("Bus target address: %d", x.address);                                        \
    SERIAL_RPC_LOG("Packet type: %s", SERIAL_RPC_PACKET_TYPE_ENUM_STRING(x.packet_type));       \
    SERIAL_RPC_LOG("Packet request/response/notification index: %d", SERIAL_RPC_PACKET_INDEX((x.index_high), (x.index_low)));                  \
    SERIAL_RPC_LOG("Packet payload length: %d", x.payload_length);                              \
                                                                                                \
    SERIAL_RPC_LOG("Payload data:\n\n");                                                        \
                                                                                                \
    for(payload_byte_index = 0 ; payload_byte_index < SERIAL_RPC_PACKET_SIZE-4 ; payload_byte_index++)      \
    {                                                                                                       \
        SERIAL_RPC_LOG("Byte[%d] = 0x%.2x", payload_byte_index, x.payload[payload_byte_index]);             \
    }                                                                                                       \
                                                                                                            \
    SERIAL_RPC_LOG("Packet CRC: 0x%.2x", x.crc);                                                            \
}

/***** Macros to get values from request packet payload and set values in the response packet payload *********/

#define SERIAL_RPC_REQUEST_PAYLOAD_GET_UINT8(handle, index, pu8)                                \
{                                                                                               \
    *pu8 = (uint8_t)handle->request.payload[index];                                             \
}

#define SERIAL_RPC_REQUEST_PAYLOAD_GET_INT8(handle, index, pi8)                                 \
{                                                                                               \
    *pi8 = (int8_t)handle->request.payload[index];                                              \
}

#define SERIAL_RPC_REQUEST_PAYLOAD_GET_UINT16(handle, index, pu16)                              \
{                                                                                               \
    uint8_t* data = handle->request.payload;                                                    \
    *pu16 = (uint16_t)((((uint16_t)data[index]) << 8) | (uint16_t)data[index+1] );              \
}

#define SERIAL_RPC_REQUEST_PAYLOAD_GET_INT16(handle, index, pi16)                              \
{                                                                                               \
    uint8_t* data = handle->request.payload;                                                    \
    *pi16 = (int16_t)((((uint16_t)data[index]) << 8) | (uint16_t)data[index+1] );              \
}

#define SERIAL_RPC_REQUEST_PAYLOAD_GET_UINT32(handle, index, pu32)                              \
{                                                                                               \
    uint8_t* data = handle->request.payload;                                                    \
    *pu32 = (uint32_t)( (((uint32_t)data[index]) << 24) |                                       \
                        (((uint32_t)data[index + 1]) << 16) |                                   \
                        (((uint32_t)data[index + 2]) << 8) |                                    \
                        (uint32_t)data[index + 3] );                                            \
}

#define SERIAL_RPC_REQUEST_PAYLOAD_GET_INT32(handle, index, pi32)                              \
{                                                                                               \
    uint8_t* data = handle->request.payload;                                                    \
    *pi32 = (int32_t)( (((uint32_t)data[index]) << 24) |                                       \
                        (((uint32_t)data[index + 1]) << 16) |                                   \
                        (((uint32_t)data[index + 2]) << 8) |                                    \
                        (uint32_t)data[index + 3] );                                            \
}

#define SERIAL_RPC_REQUEST_PAYLOAD_GET_FLOAT(handle, index, pfloat)                              \
{                                                                                               \
    uint8_t* data = handle->request.payload;                                                    \
    *((uint32_t*)pfloat) = (uint32_t)( (((uint32_t)data[index]) << 24) |                      \
                        (((uint32_t)data[index + 1]) << 16) |                                   \
                        (((uint32_t)data[index + 2]) << 8) |                                    \
                        (uint32_t)data[index + 3] );                                            \
}

#define SERIAL_RPC_RESPONSE_SET_PAYLOAD_LENGTH(handle, _length)                              \
    handle->response.length = _length;

#define SERIAL_RPC_RESPONSE_PAYLOAD_SET_UINT8(handle, index, u8_value)                      \
{                                                                                           \
    uint8_t* data = handle->response.payload;                                               \
    data[index] = (uint8_t)u8_value;                                                        \
}

#define SERIAL_RPC_RESPONSE_PAYLOAD_SET_INT8(handle, index, i8_value)                      \
{                                                                                           \
    uint8_t* data = handle->response.payload;                                               \
    data[index] = (uint8_t)i8_value;                                                        \
}

#define SERIAL_RPC_RESPONSE_PAYLOAD_SET_UINT16(handle, index, u16_value)                    \
{                                                                                           \
    uint8_t* data = handle->response.payload;                                               \
    data[index] = (uint8_t)( u16_value >> 8 );                                              \
    data[index + 1] = (uint8_t)( u16_value & 0xFF );                                        \
}

#define SERIAL_RPC_RESPONSE_PAYLOAD_SET_INT16(handle, index, i16_value)                     \
{                                                                                           \
    uint16_t u16_value = (uint16_t)i16_value;                                               \
                                                                                            \
    uint8_t* data = handle->response.payload;                                               \
    data[index] = (uint8_t)( u16_value >> 8 );                                              \
    data[index + 1] = (uint8_t)( u16_value & 0xFF );                                        \
}

#define SERIAL_RPC_RESPONSE_PAYLOAD_SET_UINT32(handle, index, u32_value)                    \
{                                                                                           \
    uint8_t* data = handle->response.payload;                                               \
    data[index] = (uint8_t)( u32_value >> 24 );                                             \
    data[index + 1] = (uint8_t)( u32_value >> 16 );                                       \
    data[index + 2] = (uint8_t)( u32_value >> 8 );                                          \
    data[index + 3] = (uint8_t)( u32_value & 0xFF );                                        \
}

#define SERIAL_RPC_RESPONSE_PAYLOAD_SET_INT32(handle, index, i32_value)                    \
{                                                                                           \
    uint32_t u32_value = (uint32_t)i32_value;                                               \
                                                                                            \
    uint8_t* data = handle->response.payload;                                               \
    data[index] = (uint8_t)( u32_value >> 24 );                                             \
    data[index + 1] = (uint8_t)( u32_value >> 16 );                                       \
    data[index + 2] = (uint8_t)( u32_value >> 8 );                                          \
    data[index + 3] = (uint8_t)( u32_value & 0xFF );                                        \
}

#define SERIAL_RPC_RESPONSE_PAYLOAD_SET_FLOAT(handle, index, float_value)                    \
{                                                                                           \
    uint32_t u32_value = *(((uint32_t*)&float_value));                                               \
                                                                                            \
    uint8_t* data = handle->response.payload;                                               \
    data[index] = (uint8_t)( u32_value >> 24 );                                             \
    data[index + 1] = (uint8_t)( u32_value >> 16 );                                         \
    data[index + 2] = (uint8_t)( u32_value >> 8 );                                          \
    data[index + 3] = (uint8_t)( u32_value & 0xFF );                                        \
}

/*********** RPC packet definitions **********/

/* Generic serial RPC packet. */

typedef struct {
    uint8_t index_high      :   2;
    uint8_t packet_type     :   2;
    uint8_t address         :   4;
    uint8_t index_low;
    uint8_t payload_length;
    uint8_t payload[SERIAL_RPC_PACKET_SIZE - 4];
    uint8_t crc;
} __attribute__((packed)) serial_rpc_packet_t;

/* Request, response and notification packets. */

typedef serial_rpc_packet_t serial_rpc_request_packet_t;
typedef serial_rpc_packet_t serial_rpc_response_packet_t;
typedef serial_rpc_packet_t serial_rpc_notification_packet_t;

typedef struct {
    uint8_t reserved_1      :   2;
    uint8_t packet_type     :   2;    
    uint8_t address         :   4;
    uint8_t enable_notifications;
    uint8_t notification_buffer_length;
    uint8_t notification_buffer_count;
    uint8_t reserved_3[SERIAL_RPC_PACKET_SIZE - 5];
    uint8_t crc;
} __attribute__((packed)) serial_rpc_control_and_status_packet_t;

/* Forward declaration of RPC handle struct */
struct serial_rpc_handle;

typedef struct serial_rpc_handle serial_rpc_handle_t;

typedef void (*serial_rpc_response_callback_t)(serial_rpc_handle_t*);

typedef struct {
    uint16_t index;
    serial_rpc_response_callback_t callback;
    void* args;
} serial_rpc_response_callback_table_entry_t;

struct serial_rpc_handle {

  /* Structure to hold the current request */
  struct {
    uint8_t payload[SERIAL_RPC_PACKET_SIZE - 4];
    size_t length;
    void* args;
  } request;

  /* Structure to hold the current response */
  struct {
    uint8_t payload[SERIAL_RPC_PACKET_SIZE - 4];
    size_t length;
  } response;
  
  /* Temporary buffer to store the bytes to be transmitted */
  uint8_t tx_buffer[SERIAL_RPC_PACKET_SIZE];
  
  /* Temporary buffer to store the received bytes */
  uint8_t rx_buffer[SERIAL_RPC_PACKET_SIZE];
  
  /* Receive buffer byte index. */
  uint8_t rx_buffer_index;
  
  /* 
   * Callback table containing the response callback function for each
   * request index.
   */
  serial_rpc_response_callback_table_entry_t* response_callback_table;
  
  /* Statistics for debugging purposes. */
  struct {
    uint32_t number_of_requests_processed;
    uint32_t number_of_notifications_sent;
    uint16_t current_notification;
  } stats;
  
  /* Processing response flag. Indicates the the bus target has received and yet to process it. */
  bool processing_response;
  
  /* Notification enabled. */
  bool notifications_enabled;
  
  /* Notification packet ring buffer. */
  serial_rpc_notification_packet_t notification_packet_buffer[SERIAL_RPC_NOTIFICATION_BUFFER_SIZE];
  
  /* Notification buffer count. */
  uint32_t notification_buffer_count;
  
  /* Notification buffer read index. */
  uint32_t notification_buffer_read_index;
  
  /* Notification buffer write index. */
  uint32_t notification_buffer_write_index;
  
  /* Application specific function to sending out bytes to the system bus */ 
  void (*send)(uint8_t* data, size_t length);
  
  /* Address of the bus target. */
  uint8_t target_address;
};

/******************** Public facing APIs *****************************/

extern int serial_rpc_init(serial_rpc_handle_t* handle);
extern void serial_rpc_handle_rx_byte(serial_rpc_handle_t* handle, uint8_t rx_byte);
extern void serial_rpc_process(serial_rpc_handle_t* handle);
extern int serial_rpc_send_notification(serial_rpc_handle_t* handle, uint16_t index, void* data, size_t length);

#endif /* __SERIAL_RPC_H__ */