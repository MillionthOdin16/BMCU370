#pragma once

#include "main.h"
#include "config.h"

#ifdef __cplusplus
extern "C"
{
#endif

// Frame protocol constants
#define SERIAL_BRIDGE_SYNC_BYTE 0xAA
#define FRAME_TYPE_DEBUG        0x01
#define FRAME_TYPE_BAMBUBUS     0x02

// Frame format: [SYNC(0xAA)] [TYPE] [LEN_H] [LEN_L] [DATA...]
// TYPE: 0x01=Debug text, 0x02=BambuBus packet
// LEN: 16-bit big-endian length of DATA field

/**
 * Initialize the serial bridge (called by Debug_log_init)
 */
void SerialBridge_init();

/**
 * Send a framed debug message to the computer
 * @param data Pointer to debug message data
 * @param length Length of debug message
 */
void SerialBridge_send_debug(const uint8_t *data, uint16_t length);

/**
 * Send a framed BambuBus packet to the computer
 * @param data Pointer to BambuBus packet data
 * @param length Length of BambuBus packet
 */
void SerialBridge_send_bambubus(const uint8_t *data, uint16_t length);

/**
 * Process received byte from computer (called from USART3 RX IRQ)
 * @param byte Received byte
 */
void SerialBridge_rx_byte(uint8_t byte);

/**
 * Check if a complete frame has been received and process it
 * Call this from main loop
 */
void SerialBridge_process();

#ifdef __cplusplus
}
#endif
