#include "SerialBridge.h"
#include "Debug_log.h"
#include "BambuBus.h"
#include <string.h>

// RX state machine
typedef enum {
    WAITING_SYNC,
    WAITING_TYPE,
    WAITING_LEN_H,
    WAITING_LEN_L,
    RECEIVING_DATA
} RxState;

static RxState rx_state = WAITING_SYNC;
static uint8_t rx_frame_type = 0;
static uint16_t rx_frame_length = 0;
static uint16_t rx_data_index = 0;
static uint8_t rx_buffer[1024];  // Buffer for incoming frames
static volatile uint8_t rx_frame_ready = 0;

// TX frame buffer (for encoding frames before DMA send)
static uint8_t tx_frame_buffer[1024];

void SerialBridge_init()
{
    rx_state = WAITING_SYNC;
    rx_frame_ready = 0;
}

/**
 * Send a framed message via USART3
 * Frame format: [SYNC(0xAA)] [TYPE] [LEN_H] [LEN_L] [DATA...]
 */
static void send_frame(uint8_t frame_type, const uint8_t *data, uint16_t length)
{
    // Build frame in buffer
    tx_frame_buffer[0] = SERIAL_BRIDGE_SYNC_BYTE;
    tx_frame_buffer[1] = frame_type;
    tx_frame_buffer[2] = (length >> 8) & 0xFF;  // Length high byte
    tx_frame_buffer[3] = length & 0xFF;          // Length low byte

    // Copy payload data
    memcpy(&tx_frame_buffer[4], data, length);

    // Send via USART3 using raw DMA (bypass framing to avoid recursion)
    extern void Debug_log_write_num_raw(const void *data, int num);
    Debug_log_write_num_raw(tx_frame_buffer, 4 + length);
}

void SerialBridge_send_debug(const uint8_t *data, uint16_t length)
{
    send_frame(FRAME_TYPE_DEBUG, data, length);
}

void SerialBridge_send_bambubus(const uint8_t *data, uint16_t length)
{
    send_frame(FRAME_TYPE_BAMBUBUS, data, length);
}

/**
 * Process received byte from USART3 (called from interrupt)
 * Implements state machine to parse framed messages
 */
void SerialBridge_rx_byte(uint8_t byte)
{
    switch (rx_state) {
        case WAITING_SYNC:
            if (byte == SERIAL_BRIDGE_SYNC_BYTE) {
                rx_state = WAITING_TYPE;
            }
            break;

        case WAITING_TYPE:
            rx_frame_type = byte;
            rx_state = WAITING_LEN_H;
            break;

        case WAITING_LEN_H:
            rx_frame_length = (uint16_t)byte << 8;
            rx_state = WAITING_LEN_L;
            break;

        case WAITING_LEN_L:
            rx_frame_length |= byte;

            // Sanity check on length
            if (rx_frame_length > 1000 || rx_frame_length == 0) {
                rx_state = WAITING_SYNC;  // Invalid length, reset
            } else {
                rx_data_index = 0;
                rx_state = RECEIVING_DATA;
            }
            break;

        case RECEIVING_DATA:
            rx_buffer[rx_data_index++] = byte;

            if (rx_data_index >= rx_frame_length) {
                // Frame complete
                rx_frame_ready = 1;
                rx_state = WAITING_SYNC;
            }
            break;
    }
}

/**
 * Process complete received frames (call from main loop)
 * Forwards BambuBus packets to the BambuBus UART
 */
void SerialBridge_process()
{
    if (rx_frame_ready) {
        // Process the received frame
        if (rx_frame_type == FRAME_TYPE_BAMBUBUS) {
            // Forward to BambuBus UART (USART1)
            // Validate it starts with 0x3D (BambuBus start byte)
            if (rx_buffer[0] == 0x3D && rx_frame_length >= 6) {
                // Send to BambuBus using existing send_uart function
                extern void send_uart(const unsigned char *data, uint16_t length);
                send_uart(rx_buffer, rx_frame_length);
            }
        }
        // Debug frames (TYPE 0x01) are ignored - just for info from computer

        rx_frame_ready = 0;
    }
}
