#include "uav.hpp"
#include <iostream>

UAV::UAV() {
	_mass = 1.0f;

	uav_matrix_init(&_inertia_matrix, 3, 3);

	for (uint8_t i = 0; i < 3; ++i) {
		for (uint8_t j = 0; j < 3; ++j) {
			if (i == j) 
				_inertia_matrix.rows[i][j] = 1.0f;
			else 
				_inertia_matrix.rows[i][j] = 0.0f;
		}
	}

	uav_matrix_init(&_position, 3, 1);
	for (uint8_t i = 0; i < 3; ++i) _position.rows[i][0] = 0.0f;

	uav_matrix_init(&_velocity, 3, 1);
	for (uint8_t i = 0; i < 3; ++i) _velocity.rows[i][0] = 0.0f;

	uav_matrix_init(&_angular_velocity, 3, 1);
	for (uint8_t i = 0; i < 3; ++i) _angular_velocity.rows[i][0] = 0.0f;

	uav_matrix_init(&_orientation, 4, 1);
	_orientation.rows[0][0] = 1.0f;
	for (uint8_t i = 1; i < 4; ++i) _orientation.rows[i][0] = 0.0f;

	uav_matrix_init(&_orientation_euler_angles, 3, 1);
	for (uint8_t i = 0; i < 3; ++i) _orientation_euler_angles.rows[i][0] = 0.0f;

	uav_matrix_init(&_forces, 3, 1);
	for (uint8_t i = 0; i < 3; ++i) _forces.rows[i][0] = 0.0f;

	uav_matrix_init(&_moments, 3, 1);
	for (uint8_t i = 0; i < 3; ++i) _moments.rows[i][0] = 0.0f;
}

UAV::~UAV() {
}

void UAV::update(float delta_t) {
	forces_update(delta_t);

	moments_update(delta_t);

	physics_update(delta_t);

	control_update(delta_t);
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

void UAV::physics_update(float delta_t) {


	// CLEAR FORCES AND MOMENTS
	_forces.rows[0][0] = 0.0f;
	_forces.rows[1][0] = 0.0f;
	_forces.rows[2][0] = 0.0f;

	_moments.rows[0][0] = 0.0f;
	_moments.rows[1][0] = 0.0f;
	_moments.rows[2][0] = 0.0f;
}

void UAV::add_force(struct Matrix *force) {
	uav_matrix_add_to(&_forces, force);
}

void UAV::add_moment(struct Matrix *moment) {
	uav_matrix_add_to(&_moments, moment);
}
