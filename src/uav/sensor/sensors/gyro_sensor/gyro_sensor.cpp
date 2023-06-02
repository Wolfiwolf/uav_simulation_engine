#include "gyro_sensor.hpp"

GyroSensor::GyroSensor(UAV *uav) {
	set_uav(uav);
}

void GyroSensor::init() {

}

void GyroSensor::update() {

}

std::vector<float> GyroSensor::get_data() {
	struct Matrix mat = _uav->get_angular_velocity();

	return { mat.rows[0][0], mat.rows[1][0], mat.rows[2][0]};
}
