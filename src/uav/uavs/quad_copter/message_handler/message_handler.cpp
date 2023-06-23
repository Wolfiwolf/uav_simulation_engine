#include "message_handler.hpp"
#include <cstring>
#include <iostream>

static void handle_command(struct Message *msg);
static void command_file_delete(struct Message *msg);
static void command_waypoints_upload(struct Message *msg);

static DataLink *_data_link;

void MessageHandler::init(DataLink *data_link) {
	_data_link = data_link;
}

void MessageHandler::handle_message(struct Message *msg) {
	switch (msg->session_type) {
		case SESSION_TYPE_COMMAND:
			handle_command(msg);
			break;
		case SESSION_TYPE_REQUEST:
			break;
	}
}

static void handle_command(struct Message *msg) {
	switch (msg->channel) {
		case COMMAND_FILE_DELETE:
			command_file_delete(msg);
			break;
		case COMMAND_WAYPOINTS_UPLOAD:
			command_waypoints_upload(msg);
			break;
		case COMMAND_WAYPOINTS_DELETE:
			break;
		case COMMAND_WAYPOINTS_SKIP_WAYPOINT:
			break;
		case COMMAND_FLIGHT_MODE_TOGGLE:
			break;
		case COMMAND_CONTROLS:
			break;
	}
}

static void command_file_delete(struct Message *msg) {
	struct Message ack_message;
	ack_message.session_id = msg->session_id;
	ack_message.session_type = msg->session_type;
	ack_message.channel = msg->channel;
	ack_message.data_len = 2;
	ack_message.data[0] = 'O';
	ack_message.data[1] = 'K';

	uint8_t data[64];
	uint8_t data_len;
	MessageHandler::message_to_bytes(&ack_message, data, &data_len);

	_data_link->send_to_gs(data, data_len);
}

static void command_waypoints_upload(struct Message *msg) {
	uint16_t num_of_waypoints = msg->data[0] | (msg->data[1] << 8);

	struct Message ack_message;
	ack_message.session_id = msg->session_id;
	ack_message.session_type = msg->session_type;
	ack_message.channel = msg->channel;
	ack_message.data_len = 2;
	ack_message.data[0] = 'O';
	ack_message.data[1] = 'K';

	uint8_t data[64];
	uint8_t data_len;
	MessageHandler::message_to_bytes(&ack_message, data, &data_len);
	_data_link->send_to_gs(data, data_len);


	uint8_t rx_data[64];
	for (uint16_t i = 0; i < num_of_waypoints; ++i) {
		_data_link->listen_for_msg(rx_data);
		_data_link->send_to_gs(data, data_len);
	}
}

void MessageHandler::message_to_bytes(struct Message *msg, uint8_t *data, uint8_t *data_len) {
	memcpy(data, &msg->session_id, 4);
	memcpy(data + 4, &msg->session_type, 1);
	memcpy(data + 5, &msg->channel, 1);
	memcpy(data + 6, &msg->data_len, 1);
	memcpy(data + 7, msg->data, msg->data_len);

	*data_len = 7 + msg->data_len;
}

void MessageHandler::bytes_to_message(uint8_t *data, uint8_t data_len, struct Message *msg) {
	memcpy(&msg->session_id, data, 4);
	memcpy(&msg->session_type, data + 4, 1);
	memcpy(&msg->channel, data + 5, 1);
	memcpy(&msg->data_len, data + 6, 1);
	memcpy(msg->data, data + 7, msg->data_len);
}
