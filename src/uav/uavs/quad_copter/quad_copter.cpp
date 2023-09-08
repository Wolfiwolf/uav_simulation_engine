#include "quad_copter.hpp"
#include "message_handler/message_handler.hpp"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <chrono>
#include <cmath>
#include "../../../uav_math/uav_math.hpp"

QuadCopter::QuadCopter() {
	_power = 0.5f;

	_target_altitude = 10.0f;
    uav_matrix_init(&_target_orientation, 3, 1);
    for (uint8_t i = 0; i < 3; ++i) 
        _target_orientation.rows[i][0] = 0.0f;
    

	_actuators["m1"] = 1.0f;
	_actuators["m2"] = 1.0f;
	_actuators["m3"] = 1.0f;
	_actuators["m4"] = 1.0f;

	_gyro_sensor = new GyroSensor(this, 1.0f);
    _accelerometer_sensor = new AccelerometerSensor(this, 1.0f);

	_sensors.push_back(_gyro_sensor);
    _sensors.push_back(_accelerometer_sensor);
}

QuadCopter::~QuadCopter() {
	delete _gyro_sensor;
    delete _accelerometer_sensor;
}

void QuadCopter::state_estimation_update(float delta_t) {
	const auto p1 = std::chrono::system_clock::now();
    static uint32_t prev_send_t = std::chrono::duration_cast<std::chrono::milliseconds>(p1.time_since_epoch()).count() - 26;
    static bool toggle = false;

    uint32_t current_t = std::chrono::duration_cast<std::chrono::milliseconds>(p1.time_since_epoch()).count();

    if (current_t - prev_send_t > 25) {
        if (toggle) {
            struct Message msg;

            msg.timestamp = current_t;
            msg.session_id = (rand() % 0xFFFFFFFF) & 0xFFFFFFFF;
            msg.session_type = SESSION_TYPE_UNSOLICITED;
            msg.channel = UNSOLICITED_ORIENTATION;
            msg.msg_index = 0;
            msg.msg_type = MESSAGE_TYPE_DATA;
            msg.data_len = 3 * 4;

            struct Matrix euler_angles = get_orientation_euler_angles_ZYX();

            memcpy(msg.data, &euler_angles.rows[0][0], 4);
            memcpy(msg.data + 4, &euler_angles.rows[1][0], 4);
            memcpy(msg.data + 8, &euler_angles.rows[2][0], 4);

            msg.crc = MessageHandler_calculate_crc(&msg);

            MessageHandler_send_msg(&msg);

            toggle = !toggle;
        } else {
            struct Message msg;

            msg.timestamp = current_t;
            msg.session_id = (rand() % 0xFFFFFFFF) & 0xFFFFFFFF;
            msg.session_type = SESSION_TYPE_UNSOLICITED;
            msg.channel = UNSOLICITED_POSITION;
            msg.msg_index = 0;
            msg.msg_type = MESSAGE_TYPE_DATA;
            msg.data_len = 3 * 4;

            struct Matrix position = get_position_geodetic();

            memcpy(msg.data, &position.rows[0][0], 4);
            memcpy(msg.data + 4, &position.rows[1][0], 4);
            memcpy(msg.data + 8, &position.rows[2][0], 4);

            msg.crc = MessageHandler_calculate_crc(&msg);

            MessageHandler_send_msg(&msg);

            toggle = !toggle;
        }

        prev_send_t = current_t;
    }
}

void QuadCopter::control_update(float delta_t) {
    static float prev_x = 0.0f;
    static float prev_y = 0.0f;
    static float prev_z = 0.0f;

    static float prev_pow = 0.0f;

    struct Matrix euler_angles = get_orientation_euler_angles_ZYX();
    struct Matrix pos = get_position();

    float target_x = _target_orientation.rows[0][0];
    float target_y = _target_orientation.rows[1][0];

    float Px = 0.5f * (target_x - euler_angles.rows[0][0]);
    float Dx = 1000.0f * (prev_x - euler_angles.rows[0][0]);
    prev_x = euler_angles.rows[0][0];

    float Py = 0.5f * (target_y - euler_angles.rows[1][0]);
    float Dy = 1000.0f * (prev_y - euler_angles.rows[1][0]);
    prev_y = euler_angles.rows[1][0];

    float Pz = 0.4f * -euler_angles.rows[2][0];
    float Dz = 8000.0f * (prev_z - euler_angles.rows[2][0]);
    prev_z = euler_angles.rows[2][0];

    float Ppower = 4.0f * (_target_altitude - pos.rows[2][0]);
    float Dpower = 200000.0f * (prev_pow - pos.rows[2][0]);
    prev_pow = pos.rows[2][0];

    float power_gain = Ppower + Dpower;
    if (power_gain > 0.4) power_gain = 0.4f;
    else if (power_gain < -0.4) power_gain = -0.4f;

    float m1 = _power + power_gain + (Px + Dx) + (Py + Dy) + (Pz + Dz);
    float m2 = _power + power_gain - (Px + Dx) + (Py + Dy) - (Pz + Dz);
    float m3 = _power + power_gain - (Px + Dx) - (Py + Dy) + (Pz + Dz);
    float m4 = _power + power_gain + (Px + Dx) - (Py + Dy) - (Pz + Dz);

    if (m1 < 0.0f) m1 = 0.0f;
    if (m1 > 1.0f) m1 = 1.0f;

    if (m2 < 0.0f) m2 = 0.0f;
    if (m2 > 1.0f) m2 = 1.0f;

    if (m3 < 0.0f) m3 = 0.0f;
    if (m3 > 1.0f) m3 = 1.0f;

    if (m4 < 0.0f) m4 = 0.0f;
    if (m4 > 1.0f) m4 = 1.0f;

    _actuators["m1"] = m1;
    _actuators["m2"] = m2;
    _actuators["m3"] = m3;
    _actuators["m4"] = m4;
}

void QuadCopter::forces_update(float delta_t) {
    struct Matrix force;
    uav_matrix_init(&force, 3, 1);

    force.rows[0][0] = 0.0f;
    force.rows[1][0] = 0.0f;
    force.rows[2][0] = -0.7f * (get_mass() * 9.8f) * (_actuators["m1"] + _actuators["m2"] + _actuators["m3"] + _actuators["m4"]);

    add_force(&force);


    uav_matrix_destroy(&force);
}

void QuadCopter::moments_update(float delta_t) {
    struct Matrix moment;
    uav_matrix_init(&moment, 3, 1);

    moment.rows[0][0] = 0.0f;
    moment.rows[1][0] = 0.0f;
    moment.rows[2][0] = 0.0f;

    struct Matrix m_moment;
    uav_matrix_init(&m_moment, 3, 1);

    float mass = get_mass();

    // M1 MOMENT
    m_moment.rows[0][0] = 0.3f * ((mass * 9.8f) * _actuators["m1"]);
    m_moment.rows[1][0] = 0.3f * ((mass * 9.8f) * _actuators["m1"]);
    m_moment.rows[2][0] = 0.5f * _actuators["m1"];

    uav_matrix_add_to(&moment, &m_moment);

    // M2 MOMENT
    m_moment.rows[0][0] = -0.3f * ((mass * 9.8f) * _actuators["m2"]);
    m_moment.rows[1][0] = 0.3f * ((mass * 9.8f) * _actuators["m2"]);
    m_moment.rows[2][0] = -0.5f * _actuators["m2"];

    uav_matrix_add_to(&moment, &m_moment);

    // M3 MOMENT
    m_moment.rows[0][0] = -0.3f * ((mass * 9.8f) * _actuators["m3"]);
    m_moment.rows[1][0] = -0.3f * ((mass * 9.8f) * _actuators["m3"]);
    m_moment.rows[2][0] = 0.5f * _actuators["m3"];

    uav_matrix_add_to(&moment, &m_moment);

    // M4 MOMENT
    m_moment.rows[0][0] = 0.3f * ((mass * 9.8f) * _actuators["m4"]);
    m_moment.rows[1][0] = -0.3f * ((mass * 9.8f) * _actuators["m4"]);
    m_moment.rows[2][0] = -0.5f * _actuators["m4"];

    uav_matrix_add_to(&moment, &m_moment);

    // NOISE
    float amplitude = 0.0f;
    m_moment.rows[0][0] = ((rand() % 100) / 100.0f) * amplitude + -(amplitude / 2.0f);
    m_moment.rows[1][0] = ((rand() % 100) / 100.0f) * amplitude + -(amplitude / 2.0f);
    m_moment.rows[2][0] = 0.0f;

    uav_matrix_add_to(&moment, &m_moment);

    add_moment(&moment);

    uav_matrix_destroy(&m_moment);
    uav_matrix_destroy(&moment);
}


void QuadCopter::communication_thread() {
    MessageHandler_init(_data_link, this);
    while(true) {
        std::cout << "Waiting for link...\n";
        _data_link->wait_for_link();
        std::cout << "Link established!\n";

        uint8_t rx_buffer[1024];
        while(true) {
            uint32_t data_len = _data_link->listen_for_msg(rx_buffer);
            if (data_len == 0) break;

            struct Message msg;
            MessageHandler_bytes_to_message(rx_buffer, &msg);

            MessageHandler_handle(&msg);
        }
    }
}

void QuadCopter::communication_non_blocking_thread()
{
    while(true)
    {
        MessageHandler_update();
    }
}

void QuadCopter::control_pan(float x, float y) {
    _target_orientation.rows[0][0] = 30.0f * y * 0.01745329f;
    _target_orientation.rows[1][0] = 30.0f * -x * 0.01745329f;
}

void QuadCopter::control_elevation(float val) {
}
