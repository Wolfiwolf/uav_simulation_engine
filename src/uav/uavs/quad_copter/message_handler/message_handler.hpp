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

// REQUESTS
#define REQUEST_MARCO 			    0
// TIME
#define REQUEST_TIMESTAMP 	        1
#define REQUEST_ALIVE_TIME 	        2
// LOGGING
#define REQUEST_DATA_LOG_SIZE 	    3
// AUTOPILOT
#define REQUEST_AUTOPILOT_FREE_MISSION_SPACE    4

// COMMANDS
// CALIBRATION
#define COMMAND_TOGGLE_CALIBRATION_MODE 		        0
#define COMMAND_GYRO_SET_OFFSETS 		                1
#define COMMAND_ACCELEROMETER_SET_OFFSETS 		        2
#define COMMAND_ACCELEROMETER_SET_SCALERS 		        3
#define COMMAND_MAGNETOMETER_SET_HARD_IRON_BIAS 		4
#define COMMAND_MAGNETOMETER_SET_X_SOFT_IRON_BIAS 		5
#define COMMAND_MAGNETOMETER_SET_Y_SOFT_IRON_BIAS 		6
#define COMMAND_MAGNETOMETER_SET_Z_SOFT_IRON_BIAS 		7
#define COMMAND_SET_CURRENT_ALTITUDE 		            8
// TIME
#define COMMAND_SET_TIMESTAMP 		                    9
// CONTROLS
#define COMMAND_CONTROL_PAN 		                    10
#define COMMAND_CONTROL_ELEVATION 		                11
// STREAMING
#define COMMAND_SET_STREAMING_POSITION                  12
#define COMMAND_SET_STREAMING_ORIENTATION               13
#define COMMAND_SET_STREAMING_GYRO                      14
#define COMMAND_SET_STREAMING_ACCELEROMETER             15
#define COMMAND_SET_STREAMING_MAGNETOMETER              16
#define COMMAND_SET_STREAMING_BAROMETER                 17
#define COMMAND_SET_STREAMING_MOTORS                    18
// LOGGING
#define COMMAND_LOGGING_DELETE_DATA_LOG                 19
// AUTOPILOT
#define COMMAND_AUTOPILOT_TOGGLE                        20
#define COMMAND_AUTOPILOT_ADD_MISSION                   21
#define COMMAND_AUTOPILOT_DELETE_MISSION                22
#define COMMAND_AUTOPILOT_SELECT_MISSION                23

// DOWNLOADS
#define DOWNLOAD_TEST 			0
#define DOWNLOAD_DATA_LOG       1

// UPLOADS
#define UPLOAD_TEST 			0
#define UPLOAD_MISSION 		    1

// UNSOLICITED
#define UNSOLICITED_POSITION        0
#define UNSOLICITED_ORIENTATION     1
#define UNSOLICITED_GYRO            2
#define UNSOLICITED_ACCELEROMETER   3
#define UNSOLICITED_MAGNETOMETER    4
#define UNSOLICITED_BAROMETER       5
#define UNSOLICITED_MOTORS          6

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
