#pragma once

#include <stdint.h>
#include "../../../../data_link/data_link.hpp"

#define SESSION_TYPE_REQUEST 		0
#define SESSION_TYPE_COMMAND 		1
#define SESSION_TYPE_UNSOLICITED 	2

#define REQUEST_FILE_DOWNLOAD 				0
#define REQUEST_WAYPOINTS_DOWNLOAD 			1
#define REQUEST_WAYPOINTS_NEXT_WAYPOINT 	2
#define REQUEST_FLIGHT_MODE 				3

#define COMMAND_FILE_DELETE 				0
#define COMMAND_WAYPOINTS_UPLOAD 			1
#define COMMAND_WAYPOINTS_DELETE 			2
#define COMMAND_WAYPOINTS_SKIP_WAYPOINT 	3
#define COMMAND_FLIGHT_MODE_TOGGLE 			4
#define COMMAND_CONTROLS 					5


struct Message {
	uint32_t session_id;
	uint8_t session_type;
	uint8_t channel;
	uint8_t data_len;
	uint8_t data[32];
};


namespace MessageHandler {
	void init(DataLink *data_link);
	void handle_message(struct Message *msg);
	void message_to_bytes(struct Message *msg, uint8_t *data, uint8_t *data_len);
	void bytes_to_message(uint8_t *data, uint8_t data_len, struct Message *msg);
}

