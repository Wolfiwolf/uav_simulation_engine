#include "uav.hpp"
#include <iostream>
#include <string>
#include "../config_manager/config_manager.hpp"

UAV::UAV() {
	{
		std::string pos_str;
		ConfigManager::get_param_val("uav_initial_position", pos_str);
		std::string num("");

		uint8_t i = 0;
		for (; pos_str[i] != ' '; ++i) num += pos_str[i];
		_lat = std::stof(num);
		num = "";
		++i;

		for (; pos_str[i] != ' '; ++i) num += pos_str[i];
		_lon = std::stof(num);
		num = "";

		for (; i < pos_str.size(); ++i) num += pos_str[i];
		_alt = std::stof(num);
	}

	_mass = 1.0f;

	uav_matrix_init(&_inertia_matrix, 3, 3);

	for (uint8_t i = 0; i < 3; ++i) {
		for (uint8_t j = 0; j < 3; ++j) {
			if (i == j) 
				_inertia_matrix.rows[i][j] = 0.1f;
			else 
				_inertia_matrix.rows[i][j] = 0.0f;
		}
	}

	uav_matrix_init(&_inverse_inertia_matrix, 3, 3);

	for (uint8_t i = 0; i < 3; ++i) {
		for (uint8_t j = 0; j < 3; ++j) {
			if (i == j) 
				_inverse_inertia_matrix.rows[i][j] = 1 / 0.1f;
			else 
				_inverse_inertia_matrix.rows[i][j] = 0.0f;
		}
	}

	uav_matrix_init(&_position, 3, 1);
	for (uint8_t i = 0; i < 3; ++i) _position.rows[i][0] = 0.0f;

	uav_matrix_init(&_velocity, 3, 1);
	for (uint8_t i = 0; i < 3; ++i) _velocity.rows[i][0] = 0.0f;

	uav_matrix_init(&_angular_velocity, 3, 1);
	for (uint8_t i = 0; i < 3; ++i) _angular_velocity.rows[i][0] = 0.0f;

	uav_matrix_init(&_orientation_euler_angles, 3, 1);
	_orientation_euler_angles.rows[0][0] = 45.0f;
	_orientation_euler_angles.rows[1][0] = 45.0f;
	_orientation_euler_angles.rows[2][0] = 45.0f;

	uav_matrix_init(&_orientation, 4, 1);
	uav_orient_euler_to_q(&_orientation_euler_angles, &_orientation);

	uav_matrix_init(&_forces, 3, 1);
	for (uint8_t i = 0; i < 3; ++i) _forces.rows[i][0] = 0.0f;

	uav_matrix_init(&_moments, 3, 1);
	for (uint8_t i = 0; i < 3; ++i) _moments.rows[i][0] = 0.0f;

	std::string url;
	ConfigManager::get_param_val("data_link_url", url);

	std::string port_str;
	ConfigManager::get_param_val("data_link_port", port_str);
	int port = std::stoi(port_str);

	_data_link = new DataLink(port);
}

UAV::~UAV() {
	delete _data_link;
}

void UAV::update(uint64_t t, float delta_t) {
	sensors_update(delta_t);
	forces_update(delta_t);
	moments_update(delta_t);
	physics_update(delta_t);
	control_update(delta_t);
}

float UAV::get_mass() {
	return _mass;
}

struct Matrix UAV::get_position() {
	return _position;
}

struct Matrix UAV::get_velocity() {
	return _velocity;
}

struct Matrix UAV::get_angular_velocity() {
	return _angular_velocity;
}

struct Matrix UAV::get_orientation_q() {
	return _orientation;
}

struct Matrix UAV::get_orientation_euler_angles_ZYX() {
	return _orientation_euler_angles;
}

void UAV::add_force(struct Matrix *force) {
	uav_matrix_add_to(&_forces, force);
}

void UAV::add_moment(struct Matrix *moment) {
	uav_matrix_add_to(&_moments, moment);
}

void UAV::physics_update(float delta_t) {

	uav_rotation_body_to_inertial(&_forces, &_orientation_euler_angles);

	_forces.rows[2][0] = _forces.rows[2][0] - 9.8f * _mass;

	update_velocity(delta_t);

	update_position(delta_t);

	update_angular_velocity(delta_t);

	update_orientation(delta_t);

	// CLEAR FORCES AND MOMENTS
	_forces.rows[0][0] = 0.0f;
	_forces.rows[1][0] = 0.0f;
	_forces.rows[2][0] = 0.0f;

	_moments.rows[0][0] = 0.0f;
	_moments.rows[1][0] = 0.0f;
	_moments.rows[2][0] = 0.0f;
}

void UAV::sensors_update(float delta_t) {
	for (Sensor *sensor : _sensors)
		sensor->update();
}

void UAV::update_position(float delta_t) {
	struct Matrix pos_dot;
	uav_matrix_init(&pos_dot, 3, 1);

	uav_matrix_copy(&_velocity, &pos_dot);

	uav_matrix_scalar_multiply(&pos_dot, delta_t);

	uav_matrix_add_to(&_position, &pos_dot);

	uav_matrix_destroy(&pos_dot);
}

void UAV::update_velocity(float delta_t) {
	struct Matrix velocity_dot;
	uav_matrix_init(&velocity_dot, 3, 1);

	uav_matrix_copy(&_forces, &velocity_dot);

	uav_matrix_scalar_multiply(&velocity_dot, delta_t / _mass);

	uav_matrix_add_to(&_velocity, &velocity_dot);

	uav_matrix_destroy(&velocity_dot);
}

void UAV::update_orientation(float delta_t) {
	struct Matrix q_dot;
	uav_matrix_init(&q_dot, 4, 1);

	uav_orient_q_dot(&_orientation, &_angular_velocity, &q_dot, delta_t);

	uav_matrix_add_to(&_orientation, &q_dot);

	float mag = uav_vec_magnitude(&_orientation);

	uav_matrix_scalar_multiply(&_orientation, 1 / mag);

	uav_orient_q_to_euler(&_orientation, &_orientation_euler_angles);

	uav_matrix_destroy(&q_dot);
}

void UAV::update_angular_velocity(float delta_t) {
	struct Matrix temp1; 
	uav_matrix_init(&temp1, 3, 1);

	uav_matrix_multiply(&_inertia_matrix, &_angular_velocity, &temp1);

	struct Matrix temp2; 
	uav_matrix_init(&temp2, 3, 1);

	uav_vec_cross(&_angular_velocity, &temp1, &temp2);

	uav_matrix_scalar_multiply(&temp2, -1);
	uav_matrix_add_to(&temp2, &_moments);

	struct Matrix temp3; 
	uav_matrix_init(&temp3, 3, 1);

	uav_matrix_multiply(&_inverse_inertia_matrix, &temp2, &temp3);

	uav_matrix_scalar_multiply(&temp3, delta_t);

	uav_matrix_add_to(&_angular_velocity, &temp3);

	uav_matrix_destroy(&temp1);
	uav_matrix_destroy(&temp2);
	uav_matrix_destroy(&temp3);

}
