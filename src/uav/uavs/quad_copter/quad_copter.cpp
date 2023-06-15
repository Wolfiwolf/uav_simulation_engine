
#include "quad_copter.hpp"
#include <iostream>

QuadCopter::QuadCopter() {
	_power = 0.0f;

	_actuators["m1"] = 1.0f;
	_actuators["m2"] = 1.0f;
	_actuators["m3"] = 0.0f;
	_actuators["m4"] = 0.0f;

	_gyro_sensor = new GyroSensor(this);
	_sensors.push_back(_gyro_sensor);
}

QuadCopter::~QuadCopter() {
	delete _gyro_sensor;
}

void QuadCopter::control_update(float delta_t) {
	struct Matrix euler_angles = get_orientation_euler_angles_ZYX();

	float Px = 0.1f * -euler_angles.rows[0][0];
	
	float Py = 0.1f * -euler_angles.rows[1][0];

	float Pz = 0.1f * -euler_angles.rows[2][0];

	/*
	_actuators["m1"] = _power + Px + Py + Pz;
	_actuators["m2"] = _power - Px + Py - Pz;
	_actuators["m3"] = _power - Px - Py + Pz;
	_actuators["m4"] = _power + Px - Py - Pz;
	*/
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

	moment.rows[0][0] = 0.0f;
	moment.rows[1][0] = 0.0f;
	moment.rows[2][0] = 0.0f;

	struct Matrix m_moment;
	uav_matrix_init(&m_moment, 3, 1);

	float mass = get_mass();

	// M1 MOMENT
	m_moment.rows[0][0] = 0.3f * (mass * _actuators["m1"]);
	m_moment.rows[1][0] = 0.3f * (mass * _actuators["m1"]);
	m_moment.rows[2][0] = 0.0f;

	uav_matrix_add_to(&moment, &m_moment);

	// M2 MOMENT
	m_moment.rows[0][0] = -0.3f * (mass * _actuators["m2"]);
	m_moment.rows[1][0] = 0.3f * (mass * _actuators["m2"]);
	m_moment.rows[2][0] = 0.0f;

	uav_matrix_add_to(&moment, &m_moment);

	// M3 MOMENT
	m_moment.rows[0][0] = -0.3f * (mass * _actuators["m3"]);
	m_moment.rows[1][0] = -0.3f * (mass * _actuators["m3"]);
	m_moment.rows[2][0] = 0.0f;

	uav_matrix_add_to(&moment, &m_moment);

	// M4 MOMENT
	m_moment.rows[0][0] = -0.3f * (mass * _actuators["m4"]);
	m_moment.rows[1][0] = 0.3f * (mass * _actuators["m4"]);
	m_moment.rows[2][0] = 0.0f;

	uav_matrix_add_to(&moment, &m_moment);
	
	add_moment(&moment);

	uav_matrix_destroy(&m_moment);
	uav_matrix_destroy(&moment);
}

void QuadCopter::handle_gs_bytes(uint8_t *bytes, uint32_t len) {

}
