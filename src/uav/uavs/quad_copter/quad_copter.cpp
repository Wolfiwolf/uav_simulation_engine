
#include "quad_copter.hpp"
#include <iostream>

QuadCopter::QuadCopter() {
	_power = 0.0f;

	_actuators["m1"] = 0.0f;
	_actuators["m2"] = 0.0f;
	_actuators["m3"] = 0.0f;
	_actuators["m4"] = 0.0f;
}

QuadCopter::~QuadCopter() {

}

void QuadCopter::control_update(float delta_t) {
	struct Matrix euler_angles = get_orientation_euler_angles_ZYX();

	float Px = 0.1f * -euler_angles.rows[0][0];
	
	float Py = 0.1f * -euler_angles.rows[1][0];

	float Pz = 0.1f * -euler_angles.rows[2][0];

	_actuators["m1"] = _power + Px + Py + Pz;
	_actuators["m2"] = _power - Px + Py - Pz;
	_actuators["m3"] = _power - Px - Py + Pz;
	_actuators["m4"] = _power + Px - Py - Pz;
}

void QuadCopter::forces_update(float delta_t) {
	struct Matrix force;
	uav_matrix_init(&force, 3, 1);

	force.rows[0][0] = 0.0f;
	force.rows[1][0] = 0.0f;
	force.rows[2][0] = get_mass() * (_actuators["m1"] + _actuators["m2"] + _actuators["m3"] + _actuators["m4"]);

	add_force(&force);

	uav_matrix_destroy(&force);
}

void QuadCopter::moments_update(float delta_t) {
	struct Matrix moment;
	uav_matrix_init(&moment, 3, 1);

	moment.rows[0][0] = (_actuators["m1"] + _actuators["m4"]) - (_actuators["m2"] + _actuators["m3"]);
	moment.rows[1][0] = (_actuators["m1"] + _actuators["m2"]) - (_actuators["m3"] + _actuators["m4"]);
	moment.rows[2][0] = (_actuators["m1"] + _actuators["m3"]) - (_actuators["m2"] + _actuators["m4"]);

	add_moment(&moment);

	uav_matrix_destroy(&moment);
}
