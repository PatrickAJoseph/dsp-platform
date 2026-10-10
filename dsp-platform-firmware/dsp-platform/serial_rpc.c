
#include "serial_rpc.h"

static const uint8_t CRC_TABLE[256] = {
    0x00, 0x07, 0x0E, 0x09, 0x1C, 0x1B, 0x12, 0x15,
    0x38, 0x3F, 0x36, 0x31, 0x24, 0x23, 0x2A, 0x2D,
    0x70, 0x77, 0x7E, 0x79, 0x6C, 0x6B, 0x62, 0x65,
    0x48, 0x4F, 0x46, 0x41, 0x54, 0x53, 0x5A, 0x5D,
    0xE0, 0xE7, 0xEE, 0xE9, 0xFC, 0xFB, 0xF2, 0xF5,
    0xD8, 0xDF, 0xD6, 0xD1, 0xC4, 0xC3, 0xCA, 0xCD,
    0x90, 0x97, 0x9E, 0x99, 0x8C, 0x8B, 0x82, 0x85,
    0xA8, 0xAF, 0xA6, 0xA1, 0xB4, 0xB3, 0xBA, 0xBD,
    0xC7, 0xC0, 0xC9, 0xCE, 0xDB, 0xDC, 0xD5, 0xD2,
    0xFF, 0xF8, 0xF1, 0xF6, 0xE3, 0xE4, 0xED, 0xEA,
    0xB7, 0xB0, 0xB9, 0xBE, 0xAB, 0xAC, 0xA5, 0xA2,
    0x8F, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9D, 0x9A,
    0x27, 0x20, 0x29, 0x2E, 0x3B, 0x3C, 0x35, 0x32,
    0x1F, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0D, 0x0A,
    0x57, 0x50, 0x59, 0x5E, 0x4B, 0x4C, 0x45, 0x42,
    0x6F, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7D, 0x7A,
    0x89, 0x8E, 0x87, 0x80, 0x95, 0x92, 0x9B, 0x9C,
    0xB1, 0xB6, 0xBF, 0xB8, 0xAD, 0xAA, 0xA3, 0xA4,
    0xF9, 0xFE, 0xF7, 0xF0, 0xE5, 0xE2, 0xEB, 0xEC,
    0xC1, 0xC6, 0xCF, 0xC8, 0xDD, 0xDA, 0xD3, 0xD4,
    0x69, 0x6E, 0x67, 0x60, 0x75, 0x72, 0x7B, 0x7C,
    0x51, 0x56, 0x5F, 0x58, 0x4D, 0x4A, 0x43, 0x44,
    0x19, 0x1E, 0x17, 0x10, 0x05, 0x02, 0x0B, 0x0C,
    0x21, 0x26, 0x2F, 0x28, 0x3D, 0x3A, 0x33, 0x34,
    0x4E, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5C, 0x5B,
    0x76, 0x71, 0x78, 0x7F, 0x6A, 0x6D, 0x64, 0x63,
    0x3E, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2C, 0x2B,
    0x06, 0x01, 0x08, 0x0F, 0x1A, 0x1D, 0x14, 0x13,
    0xAE, 0xA9, 0xA0, 0xA7, 0xB2, 0xB5, 0xBC, 0xBB,
    0x96, 0x91, 0x98, 0x9F, 0x8A, 0x8D, 0x84, 0x83,
    0xDE, 0xD9, 0xD0, 0xD7, 0xC2, 0xC5, 0xCC, 0xCB,
    0xE6, 0xE1, 0xE8, 0xEF, 0xFA, 0xFD, 0xF4, 0xF3
};

uint8_t crc8ccitt(uint8_t * data, size_t size) {
	uint8_t val = 0;

	uint8_t * pos = (uint8_t *) data;
	uint8_t * end = pos + size;

	while (pos < end) {
		val = CRC_TABLE[val ^ *pos];
		pos++;
	}

	return val;
}

int serial_rpc_init(serial_rpc_handle_t* handle)
{
    if(!handle) {
        SERIAL_RPC_LOG("Handle is NULL.");
        return -EINVAL;
    }
    
    if(!handle->response_callback_table)
    {
        SERIAL_RPC_LOG("No response callback table provided.");
        return -EINVAL;
    }
    
    if(!handle->send)
    {
        SERIAL_RPC_LOG("No application specific function for send operation defined.")
        return -EINVAL;
    }
    
    memset(handle->request.payload, 0, sizeof(handle->request.payload));
    memset(handle->response.payload, 0, sizeof(handle->response.payload));
    memset(handle->tx_buffer, 0, sizeof(handle->tx_buffer));
    memset(handle->rx_buffer, 0, sizeof(handle->rx_buffer));
    
    handle->processing_response = false;
    handle->rx_buffer_index = 0;
    
    handle->notification_buffer_count = 0;
    handle->notification_buffer_read_index = 0;
    handle->notification_buffer_write_index = 0;
    memset(handle->notification_packet_buffer, 0, sizeof(handle->notification_packet_buffer));
    
    return 0;
};

void serial_rpc_handle_rx_byte(serial_rpc_handle_t* handle, uint8_t rx_byte)
{
    if(!handle->processing_response) {
        
        if(handle->rx_buffer_index == 0)
        {
            memset(handle->rx_buffer, 0, sizeof(handle->rx_buffer));
        }
        
        handle->rx_buffer[handle->rx_buffer_index] = rx_byte;
        
        handle->rx_buffer_index++;
        
        if(handle->rx_buffer_index == SERIAL_RPC_PACKET_SIZE)
        {
            if(!crc8ccitt( (uint8_t*)handle->rx_buffer, SERIAL_RPC_PACKET_SIZE ))
            {
                if( ((serial_rpc_packet_t*)handle->rx_buffer)->address == handle->target_address )
                {
                    SERIAL_RPC_LOG("Received a valid packet.");
                    
                    handle->processing_response = true;
                }
            }
            else 
            {
                SERIAL_RPC_LOG("Received a corrupted packet.");
            }

            handle->rx_buffer_index = 0;
        }
    }
}

static int serial_rpc_read_notification(serial_rpc_handle_t* handle, serial_rpc_notification_packet_t* packet)
{
    if(handle->notification_buffer_count == 0)
    {
        return -EINVAL;
    }
    
    *packet = handle->notification_packet_buffer[handle->notification_buffer_read_index];
    
    handle->notification_buffer_read_index++;
    
    if(handle->notification_buffer_read_index >= SERIAL_RPC_NOTIFICATION_BUFFER_SIZE)
    {
        handle->notification_buffer_read_index = 0;
    }
    
    handle->notification_buffer_count--;
    
    return 0;
}

void serial_rpc_process(serial_rpc_handle_t* handle)
{
    if(handle->processing_response) {
        
        serial_rpc_packet_t* rx_packet = (serial_rpc_packet_t*)handle->rx_buffer;
        
        SERIAL_RPC_PACKET_PRINT_INFO((*rx_packet));
        
        switch(rx_packet->packet_type) 
        {
            case SERIAL_RPC_PACKET_TYPE_REQUEST:
            {
                serial_rpc_packet_t* tx_packet = (serial_rpc_packet_t*)handle->tx_buffer;
                serial_rpc_response_callback_table_entry_t* callback_entry = handle->response_callback_table;
                bool found = false;
                
                memset(tx_packet, 0, sizeof(serial_rpc_packet_t));
                
                while(callback_entry->callback) 
                {    
                    if(callback_entry->index == SERIAL_RPC_PACKET_INDEX( (rx_packet->index_high), (rx_packet->index_low) ))
                    {
                        found = true;
                        break;
                    }
                    
                    callback_entry++;
                }
                
                if(found)
                {
                    SERIAL_RPC_LOG("Processing request index: %d", callback_entry->index);
                    
                    handle->request.length = (size_t)rx_packet->payload_length;
                    memset( handle->request.payload, 0, sizeof(handle->request.payload));
                    memcpy( handle->request.payload, rx_packet->payload, handle->request.length);
                    handle->request.args = callback_entry->args;
                    
                    handle->response.length = 0;
                    memset(handle->response.payload, 0 , sizeof(handle->response.payload));
                    
                    callback_entry->callback(handle);
                    
                    memset(tx_packet, 0, sizeof(serial_rpc_packet_t));
                    
                    tx_packet->address = handle->target_address;
                    tx_packet->packet_type = SERIAL_RPC_PACKET_TYPE_RESPONSE;
                    tx_packet->index_high = rx_packet->index_high;
                    tx_packet->index_low = rx_packet->index_low;
                    tx_packet->payload_length = handle->response.length;
                    
                    memcpy(tx_packet->payload, handle->response.payload, sizeof(handle->response.payload));
                    
                    tx_packet->crc = crc8ccitt((uint8_t*)tx_packet, sizeof(serial_rpc_packet_t) - 1);
                    
                    memcpy(handle->tx_buffer, tx_packet, sizeof(serial_rpc_packet_t));
                    
                    SERIAL_RPC_PACKET_PRINT_INFO((*tx_packet));
                    
                    handle->send(handle->tx_buffer, sizeof(serial_rpc_packet_t));
                }
            }
            break;
            
            case SERIAL_RPC_PACKET_TYPE_CONTROL_AND_STATUS:
            {
                serial_rpc_control_and_status_packet_t* rx_packet = (serial_rpc_control_and_status_packet_t*)handle->rx_buffer;
                serial_rpc_control_and_status_packet_t* tx_packet = (serial_rpc_control_and_status_packet_t*)handle->tx_buffer;
                
                if(rx_packet->enable_notifications)
                {
                    SERIAL_RPC_LOG("Enabled notifications");
                    handle->notifications_enabled = true;
                }
                else
                {
                    SERIAL_RPC_LOG("Disabled notifications");
                    handle->notifications_enabled = false;
                }
                
                memset(tx_packet, 0, sizeof(serial_rpc_control_and_status_packet_t));
                
                tx_packet->address = handle->target_address;
                tx_packet->packet_type = SERIAL_RPC_PACKET_TYPE_CONTROL_AND_STATUS;
                tx_packet->enable_notifications = handle->notifications_enabled;
                tx_packet->notification_buffer_length = SERIAL_RPC_NOTIFICATION_BUFFER_SIZE;
                tx_packet->notification_buffer_count = handle->notification_buffer_count;
                tx_packet->crc = crc8ccitt( (uint8_t*)tx_packet, sizeof(serial_rpc_control_and_status_packet_t)  - 1 );
                
                memcpy(handle->tx_buffer, tx_packet, sizeof(serial_rpc_packet_t));
                
                handle->send(handle->tx_buffer, sizeof(serial_rpc_control_and_status_packet_t));
            }
            break;
            
            default:
                SERIAL_RPC_LOG("Invalid packet received");
        }
        
        handle->processing_response = false;
    }
    
    if(handle->notifications_enabled)
    { 
        while( ( serial_rpc_read_notification(handle, (serial_rpc_notification_packet_t*)handle->tx_buffer) == 0 ) )
        {
            handle->send(handle->tx_buffer, sizeof(serial_rpc_packet_t));
        }
    }
}

int serial_rpc_send_notification(serial_rpc_handle_t* handle, uint16_t index, void* data, size_t length)
{
    serial_rpc_notification_packet_t* packet = &handle->notification_packet_buffer[handle->notification_buffer_write_index];
    
    if(length > (size_t)(SERIAL_RPC_PACKET_SIZE - 4))
    {
        return -EINVAL;
    }
    
    if(handle->notification_buffer_count == SERIAL_RPC_NOTIFICATION_BUFFER_SIZE)
    {
        SERIAL_RPC_LOG("Notification buffer full");
        return -ENOSPC;
    }
    
    packet->address = handle->target_address;
    packet->packet_type = SERIAL_RPC_PACKET_TYPE_NOTIFICATION;
    packet->index_low = SERIAL_RPC_PACKET_INDEX_LOW(index);
    packet->index_high = SERIAL_RPC_PACKET_INDEX_HIGH(index);
    packet->payload_length = (uint8_t)length;
    
    memcpy(packet->payload, data, length);
    
    packet->crc = crc8ccitt((uint8_t*)packet, sizeof(serial_rpc_notification_packet_t) - 1);
    
    handle->notification_buffer_write_index++;
    handle->notification_buffer_count++;
    
    if(handle->notification_buffer_write_index >= SERIAL_RPC_NOTIFICATION_BUFFER_SIZE)
    {
        handle->notification_buffer_write_index = 0;
    }
    
    return 0;
}
