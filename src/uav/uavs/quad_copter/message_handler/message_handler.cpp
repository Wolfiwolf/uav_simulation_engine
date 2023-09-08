#include "message_handler.hpp"

#include <stdbool.h>
#include <stdint.h>
#include <chrono>
#include <string.h>

struct SingleSession {
	bool active;
	uint32_t last_msg_t;
	uint32_t session_id;
	uint8_t session_type;
	uint8_t channel;
	uint8_t retry_count;
	struct Message last_msg;
};

struct StreamSession {
	bool active;
	uint32_t last_msg_t;
	uint32_t session_id;
	uint8_t session_type;
	uint8_t channel;
	uint32_t prev_index;
	uint32_t end_index;
	bool waiting_for_ack;
	uint8_t retry_count;
	struct Message last_msg;
};

#define SESSION_MANAGEMENT_TIME 		100
#define SESSION_DEAD_TIMEOUT 			10000
#define MESSAGE_RETRY_TIME_PERIOD 		250
#define MAX_MESSAGE_RETRIES  			3

static uint32_t get_timestamp();

static void send_ack(struct Message *msg);
static void send_nack(struct Message *msg);

static void manage_existing_sessions();

static uint8_t get_session_index(uint32_t session_id, uint8_t session_type);
static uint8_t get_free_session_index(uint8_t session_type);

static void handle_msg(struct Message *msg);

static void handle_single(struct Message *msg);
static void handle_stream(struct Message *msg);

static void handle_request(struct Message *msg);
static void handle_command(struct Message *msg);

static void handle_download(uint8_t channel, uint32_t address, uint8_t *buffer, uint8_t data_len);
static void handle_upload(uint8_t channel, uint32_t address, uint8_t *buffer, uint8_t data_len);

static void handle_request_marco(uint8_t *buffer, uint8_t *data_len);

static void handle_command_calibrate_gyro(uint8_t *buffer, uint8_t data_len);
static void handle_command_calibrate_accelerometer_xy(uint8_t *buffer, uint8_t data_len);
static void handle_command_calibrate_accelerometer_z(uint8_t *buffer, uint8_t data_len);
static void handle_command_control_pan(uint8_t *buffer, uint8_t data_len);
static void handle_command_control_elevation(uint8_t *buffer, uint8_t data_len);

static void handle_download_test(uint32_t address, uint8_t *buffer, uint8_t data_len);

static void handle_upload_test(uint32_t address, uint8_t *buffer, uint8_t data_len);

static DataLink *_data_link;
static QuadCopter *_quad_copter;

#define NUM_OF_SESSIONS 6

static struct SingleSession _single_sessions[NUM_OF_SESSIONS];
static struct StreamSession _stream_sessions[NUM_OF_SESSIONS];

void MessageHandler_handle(struct Message *msg)
{
	handle_msg(msg);
}

static uint32_t _prev_t;

void MessageHandler_update()
{
	uint32_t current_t = get_timestamp();

	current_t = get_timestamp();
	if (current_t - _prev_t > SESSION_MANAGEMENT_TIME)
	{
		manage_existing_sessions();
		_prev_t = current_t;
	}
}

void MessageHandler_init(DataLink *data_link, QuadCopter *quad_copter)
{
	_data_link = data_link;
    _quad_copter = quad_copter;

	for (uint8_t i = 0; i < NUM_OF_SESSIONS; i++) {
		_single_sessions[i].active = false;
		_stream_sessions[i].active = false;
	}

	_prev_t = get_timestamp();
}

static void handle_msg(struct Message *msg)
{
	uint8_t crc = MessageHandler_calculate_crc(msg);

	if (crc != msg->crc)
	{
		send_nack(msg);
		return;
	}

	if (msg->session_type == SESSION_TYPE_REQUEST || msg->session_type == SESSION_TYPE_COMMAND) {
		handle_single(msg);
	} else if (msg->session_type == SESSION_TYPE_DOWNLOAD || msg->session_type == SESSION_TYPE_UPLOAD) {
		handle_stream(msg);
	} else {
		send_nack(msg);
	}
}

static void handle_single(struct Message *msg)
{
	switch (msg->session_type)
	{
	case SESSION_TYPE_REQUEST:
		handle_request(msg);
		break;
	case SESSION_TYPE_COMMAND:
		handle_command(msg);
		break;
	default:
	 	send_nack(msg);
		break;
	}
}

static void handle_stream(struct Message *msg)
{
	uint32_t session_index = get_session_index(msg->session_id, msg->session_type);

	if (session_index == 0xFF)
	{
		if (msg->msg_index != 0) {
			send_nack(msg);
			return;
		}

		session_index = get_free_session_index(msg->session_type);

		_stream_sessions[session_index].active = true;
		_stream_sessions[session_index].last_msg_t = get_timestamp();
		_stream_sessions[session_index].session_id = msg->session_id;
		_stream_sessions[session_index].session_type = msg->session_type;
		_stream_sessions[session_index].channel = msg->channel;
		_stream_sessions[session_index].prev_index = *((uint32_t*)msg->data);
		_stream_sessions[session_index].end_index = _stream_sessions[session_index].prev_index + (*((uint32_t*)(msg->data + 4)));
		_stream_sessions[session_index].waiting_for_ack = false;
		_stream_sessions[session_index].retry_count = 0;
		_stream_sessions[session_index].last_msg = *msg;

		uint32_t address = *((uint32_t*)msg->data);
		uint32_t len = *((uint32_t*)(msg->data + 4));

		uint32_t test = 3;
	}

	struct StreamSession *session = &_stream_sessions[session_index];

	if (session->session_type == SESSION_TYPE_DOWNLOAD) {
		if (session->waiting_for_ack) {
			if (msg->msg_type != MESSAGE_TYPE_ACK || msg->msg_index != session->last_msg.msg_index) {
				send_nack(msg);
				session->active = false;
				return;
			}

			session->retry_count = 0;
			if (session->prev_index >= session->end_index)
			{
				session->active = false;
				return;
			}

		}

		struct Message data_msg;
		data_msg.timestamp = get_timestamp();
		data_msg.session_id = msg->session_id;
		data_msg.session_type = msg->session_type;
		data_msg.channel = msg->channel;
		data_msg.msg_type = MESSAGE_TYPE_DATA;
		data_msg.msg_index = msg->msg_index + 1;

		uint8_t bytes_to_read = session->end_index - session->prev_index;
		data_msg.data_len = bytes_to_read < 16 ? bytes_to_read : 16;

		handle_download(session->channel, session->prev_index, data_msg.data, data_msg.data_len);

		data_msg.crc = MessageHandler_calculate_crc(&data_msg);

		MessageHandler_send_msg(&data_msg);

		session->prev_index += data_msg.data_len;
		session->last_msg = data_msg;
		session->waiting_for_ack = true;

	} else if (session->session_type == SESSION_TYPE_UPLOAD) {

		if (msg->msg_index == 0 || msg->msg_index == session->last_msg.msg_index)
		{
			send_ack(msg);
			return;
		}

		handle_upload(session->channel, session->prev_index, msg->data, msg->data_len);

		session->prev_index += msg->data_len;

		send_ack(msg);

		if (session->prev_index >= session->end_index)
			session->active = false;
	}

}

static void handle_command(struct Message *msg)
{
    bool res = false;
    switch (msg->channel)
    {
        case COMMAND_GYRO_TOGGLE_CALIBRATION:
            res = true;
            break;
        case COMMAND_GYRO_SET_OFFSETS:
            res = true;
            break;
        case COMMAND_ACCELEROMETER_TOGGLE_CALIBRATION:
            res = true;
            break;
        case COMMAND_ACCELEROMETER_SET_OFFSETS:
            res = true;
            break;
        case COMMAND_ACCELEROMETER_SET_SCALERS:
            res = true;
            break;
        case COMMAND_MAGNETOMETER_TOGGLE_CALIBRATION:
            res = true;
            break;
        case COMMAND_MAGNETOMETER_SET_HARD_IRON_BIAS:
            res = true;
            break;
        case COMMAND_MAGNETOMETER_SET_X_SOFT_IRON_BIAS:
            res = true;
            break;
        case COMMAND_MAGNETOMETER_SET_Y_SOFT_IRON_BIAS:
            res = true;
            break;
        case COMMAND_MAGNETOMETER_SET_Z_SOFT_IRON_BIAS:
            res = true;
            break;
        case COMMAND_SET_STREAMING_POSITION:
            res = true;
            break;
        case COMMAND_SET_STREAMING_ORIENTATION:
            res = true;
            break;
        case COMMAND_SET_STREAMING_GYRO:
            res = true;
            break;
        case COMMAND_SET_STREAMING_ACCELEROMETER:
            res = true;
            break;
        case COMMAND_SET_STREAMING_MAGNETOMETER:
            res = true;
            break;
        case COMMAND_SET_STREAMING_BAROMETER:
            res = true;
            break;
        case COMMAND_CONTROL_PAN:
            handle_command_control_pan(msg->data, msg->data_len);
            res = true;
            break;
        case COMMAND_CONTROL_ELEVATION:
            handle_command_control_elevation(msg->data, msg->data_len);
            res = true;
            break;
	}

    if (res) send_ack(msg);
    else send_nack(msg);
}

static void handle_request(struct Message *msg)
{
	uint8_t session_index = get_session_index(msg->session_id, msg->session_type);

	if (session_index != 0xFF)
	{
		struct SingleSession *session = &_single_sessions[session_index];

		if (msg->msg_type == MESSAGE_TYPE_ACK)
		{
			session->active = false;
			return;
		}
	}


	struct Message response_msg;
	response_msg.timestamp = get_timestamp();
	response_msg.session_id = msg->session_id;
	response_msg.session_type = msg->session_type;
	response_msg.channel = msg->channel;
	response_msg.msg_type = MESSAGE_TYPE_DATA;
	response_msg.msg_index = msg->msg_index + 1;

    bool res = false;
	switch (msg->channel)
	{
	case REQUEST_MARCO:
	 	handle_request_marco(response_msg.data, &response_msg.data_len);
        res = true;
		break;
	case REQUEST_GET_TIMESTAMP:
        res = true;
		break;
	case REQUEST_GET_ALIVE_TIME:
        res = true;
		break;
	}

    if (!res) {
        send_nack(msg);
    } else {
        response_msg.crc = MessageHandler_calculate_crc(&response_msg);

        MessageHandler_send_msg(&response_msg);

        session_index = get_free_session_index(msg->session_type);
        if (session_index == 0xFF) {
            send_nack(msg);
            return;
        }

        struct SingleSession *session = &_single_sessions[session_index];
        session->active = true;
        session->last_msg_t = get_timestamp();
        session->session_id = msg->session_id;
        session->session_type = SESSION_TYPE_REQUEST;
        session->channel = msg->channel;
        session->retry_count = 0;
        session->last_msg = response_msg;
    }
}

static void handle_download(uint8_t channel, uint32_t address, uint8_t *buffer, uint8_t data_len)
{
	switch (channel)
	{
	case DOWNLOAD_TEST:
	 	handle_download_test(address, buffer, data_len);
		break;
	default:
		break;
	}
}

static void handle_upload(uint8_t channel, uint32_t address, uint8_t *buffer, uint8_t data_len)
{
	switch (channel)
	{
	case UPLOAD_TEST:
	 	handle_upload_test(address, buffer, data_len);
		break;
	default:
		break;
	}
}

static void handle_command_calibrate_gyro(uint8_t *buffer, uint8_t data_len) {
}

static void handle_command_calibrate_accelerometer_xy(uint8_t *buffer, uint8_t data_len) {

}

static void handle_command_calibrate_accelerometer_z(uint8_t *buffer, uint8_t data_len) {

}

static void handle_command_control_pan(uint8_t *buffer, uint8_t data_len) {
    _quad_copter->control_pan(*(float *)buffer, *(float *)(buffer + 4));
}

static void handle_command_control_elevation(uint8_t *buffer, uint8_t data_len) {
    _quad_copter->control_elevation(*(float *)buffer);
}

static void handle_request_marco(uint8_t *buffer, uint8_t *data_len)
{
	buffer[0] = 'P';
	buffer[1] = 'O';
	buffer[2] = 'L';
	buffer[3] = 'O';

	*data_len = 4;
}

static void handle_download_test(uint32_t address, uint8_t *buffer, uint8_t data_len)
{
	for (size_t i = 0; i < data_len; i++)
	{
		buffer[i] = (address + i) % 256;
	}
}

static void handle_upload_test(uint32_t address, uint8_t *buffer, uint8_t data_len) 
{

}

static uint8_t get_session_index(uint32_t session_id, uint8_t session_type)
{

	if (session_type == SESSION_TYPE_COMMAND || session_type == SESSION_TYPE_REQUEST)
	{
		for (uint8_t i = 0; i < NUM_OF_SESSIONS; i++)
		{
			if (_single_sessions[i].active && _single_sessions->active)
			{
				return i;
			}
		}
	}
	else if (session_type == SESSION_TYPE_DOWNLOAD || session_type == SESSION_TYPE_UPLOAD)
	{
		for (uint8_t i = 0; i < NUM_OF_SESSIONS; i++)
		{
			if (_stream_sessions[i].active && _stream_sessions->active)
			{
				return i;
			}
		}
	}

	return 0xFF;
}

static uint8_t get_free_session_index(uint8_t session_type) {

	if (session_type == SESSION_TYPE_COMMAND || session_type == SESSION_TYPE_REQUEST)
	{
		for (uint8_t i = 0; i < NUM_OF_SESSIONS; i++)
		{
			if (!_single_sessions[i].active)
				return i;
		}
	}
	else if (session_type == SESSION_TYPE_DOWNLOAD || session_type == SESSION_TYPE_UPLOAD)
	{
		for (uint8_t i = 0; i < NUM_OF_SESSIONS; i++)
		{
			if (!_stream_sessions[i].active)
				return i;
		}
	}

	return 0xFF;
}

static void send_nack(struct Message *msg)
{
	struct Message nack_msg = *msg;

	nack_msg.data_len = 0;
	nack_msg.timestamp = get_timestamp();
	nack_msg.msg_type = MESSAGE_TYPE_NACK;
	nack_msg.crc = MessageHandler_calculate_crc(&nack_msg);

	MessageHandler_send_msg(&nack_msg);
}

static void send_ack(struct Message *msg)
{
	struct Message ack_msg = *msg;

	ack_msg.data_len = 0;
	ack_msg.timestamp = get_timestamp();
	ack_msg.msg_type = MESSAGE_TYPE_ACK;
	ack_msg.crc = MessageHandler_calculate_crc(&ack_msg);

	MessageHandler_send_msg(&ack_msg);
}

static void manage_existing_sessions() {
	uint32_t current_t = get_timestamp();

	for (uint8_t i = 0; i < NUM_OF_SESSIONS; i++)
	{
		struct StreamSession *session = &_stream_sessions[i];
		if (!session->active) continue;

		if (current_t - session->last_msg_t > SESSION_DEAD_TIMEOUT) {
			session->active = false;
			continue;
		}

		if (session->session_type == SESSION_TYPE_DOWNLOAD) {
			// CHECK IF ENOUGH TIME HAS PASSED
			if (session->waiting_for_ack) {
				if (session->retry_count >= MAX_MESSAGE_RETRIES) {
					session->active = false;
					continue;
				}
				session->last_msg_t = get_timestamp();
				session->last_msg.timestamp = session->last_msg_t;
				session->last_msg.crc = MessageHandler_calculate_crc(&session->last_msg);

				MessageHandler_send_msg(&session->last_msg);

				++session->retry_count;
			}
		}
	}

	for (uint8_t i = 0; i < NUM_OF_SESSIONS; i++)
	{
		struct SingleSession *session = &_single_sessions[i];
		if (!session->active)
			continue;

		if (session->session_type == SESSION_TYPE_REQUEST)
		{
			if (current_t - session->last_msg_t >= MESSAGE_RETRY_TIME_PERIOD)
			{
				if (session->retry_count < MAX_MESSAGE_RETRIES)
				{
					session->last_msg_t = get_timestamp();
					session->last_msg.timestamp = session->last_msg_t;
					session->last_msg.crc = MessageHandler_calculate_crc(&session->last_msg);

					MessageHandler_send_msg(&session->last_msg);

					++session->retry_count;
				}
				else
				{
					session->active = false;
				}
			}
		}
	}
}

void MessageHandler_send_msg(struct Message *msg)
{
	uint8_t data[32];
	uint8_t len;
	MessageHandler_message_to_bytes(msg, data, &len, true);

	_data_link->send_to_gs(data, len);
}

uint32_t MessageHandler_calculate_crc(struct Message *msg)
{
	uint8_t data[32];
	uint8_t data_len;
	MessageHandler_message_to_bytes(msg, data, &data_len, false);

	uint8_t crc = 0;

	for (uint8_t i = 0; i < data_len; ++i)
	{
		crc = (crc + data[i]) % 256;
	}

	return crc;
}

static uint32_t get_timestamp()
{
	const auto p1 = std::chrono::system_clock::now();
	
	return std::chrono::duration_cast<std::chrono::milliseconds>(p1.time_since_epoch()).count();
}

void MessageHandler_bytes_to_message(uint8_t *data, struct Message *msg)
{
	uint8_t i = 0;
	memcpy(&msg->crc, data + i, 4);
	i += 1;
	memcpy(&msg->timestamp, data + i, 4);
	i += 4;
	memcpy(&msg->session_id, data + i, 4);
	i += 4;
	memcpy(&msg->session_type, data + i, 1);
	i += 1;
	memcpy(&msg->channel, data + i, 1);
	i += 1;
	memcpy(&msg->msg_type, data + i, 1);
	i += 1;
	memcpy(&msg->msg_index, data + i, 2);
	i += 2;
	memcpy(&msg->data_len, data + i, 1);
	i += 1;
	memcpy(msg->data, data + i, msg->data_len);
}

void MessageHandler_message_to_bytes(struct Message *msg, uint8_t *buffer, uint8_t *len, bool include_crc)
{
	*len = 0;

	if (include_crc) {
		memcpy(buffer + *len, &msg->crc, 1);
		*len += 1;
	}

	memcpy(buffer + *len, &msg->timestamp, 4);
	*len += 4;
	memcpy(buffer + *len, &msg->session_id, 4);
	*len += 4;
	memcpy(buffer + *len, &msg->session_type, 1);
	*len += 1;
	memcpy(buffer + *len, &msg->channel, 1);
	*len += 1;
	memcpy(buffer + *len, &msg->msg_type, 1);
	*len += 1;
	memcpy(buffer + *len, &msg->msg_index, 2);
	*len += 2;
	memcpy(buffer + *len, &msg->data_len, 1);
	*len += 1;
	memcpy(buffer + *len, msg->data, msg->data_len);
	*len += msg->data_len;
}
