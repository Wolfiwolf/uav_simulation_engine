#pragma once

#include "../../../../data_link/data_link.hpp"
#include "../quad_copter.hpp"
#include <stdint.h>
#include <stdbool.h>

#define SESSION_TYPE_REQUEST 		0
#define SESSION_TYPE_COMMAND 		1
#define SESSION_TYPE_UNSOLICITED 	2
#define SESSION_TYPE_DOWNLOAD 		3
#define SESSION_TYPE_UPLOAD 		4

#define REQUEST_MARCO 			0
#define REQUEST_GET_TIMESTAMP 	1
#define REQUEST_GET_ALIVE_TIME 	2

#define COMMAND_CALIBRATE_GYRO 		            0
#define COMMAND_CALIBRATE_ACCELEROMETER_XY 		1
#define COMMAND_CALIBRATE_ACCELEROMETER_Z 		2
#define COMMAND_SET_TIMESTAMP 		            3
#define COMMAND_CONTROL_PAN 		            4
#define COMMAND_CONTROL_ELEVATION 		        5

#define DOWNLOAD_TEST 			0

#define UPLOAD_TEST 			0

#define UNSOLICITED_UAV_POSITION 0
#define UNSOLICITED_UAV_ORIENTATION 1

#define MESSAGE_TYPE_DATA 	0
#define MESSAGE_TYPE_ACK 	1
#define MESSAGE_TYPE_NACK 	2

struct Message {
	uint8_t crc;
	uint32_t timestamp;
	uint32_t session_id;
	uint8_t session_type;
	uint8_t channel;
	uint8_t msg_type;
	uint16_t msg_index;
	uint8_t data_len;
	uint8_t data[16];
};


void MessageHandler_init(DataLink *data_link, QuadCopter *quad_copter);

void MessageHandler_handle(struct Message *msg);

void MessageHandler_update();

void MessageHandler_bytes_to_message(uint8_t *data, struct Message *msg);

void MessageHandler_message_to_bytes(struct Message *msg, uint8_t *buffer, uint8_t *len, bool include_crc);

void MessageHandler_send_msg(struct Message *msg);

uint32_t MessageHandler_calculate_crc(struct Message *msg);
