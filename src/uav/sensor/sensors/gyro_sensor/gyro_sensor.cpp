#include "gyro_sensor.hpp"
#include <math.h>

#define PI 3.141592653f

GyroSensor::GyroSensor(UAV *uav, float noise_level): Sensor(uav, noise_level) {}

void GyroSensor::update(uint64_t t) {
	struct Matrix mat = _uav->get_angular_velocity();

    /* ADDING NOISE
    float t_in_s = t / 1000000.0f;

    float p_noise = 2 * sinf(t_in_s * 80.0f *  2.0f * PI);
    float q_noise = 2 * sinf(t_in_s * 65.0f * 2.0f * PI);
    float r_noise = 2 * sinf(t_in_s * 75.0f *  2.0f * PI);
    */

    _p = mat.rows[0][0];
    _q = mat.rows[1][0];
    _r = mat.rows[2][0];
}

std::vector<float> GyroSensor::get_data() {
	return { _p, _q, _r };
}
