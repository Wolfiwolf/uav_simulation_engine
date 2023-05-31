#include "uav.hpp"
#include <iostream>


void UAV::init() {
	uav_matrix_init(&_forces, 3, 1);
	uav_matrix_init(&_moments, 3, 1);

	specific_init();
}

void UAV::update(float delta_t) {
	forces_update(delta_t);

	moments_update(delta_t);

	physics_update(delta_t);

	control_update(delta_t);
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
