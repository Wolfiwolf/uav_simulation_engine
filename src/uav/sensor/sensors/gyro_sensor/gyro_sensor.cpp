#include "gyro_sensor.hpp"
#include <math.h>

#define PI 3.141592653f

GyroSensor::GyroSensor(UAV *uav, float noise_level): Sensor(uav, noise_level) {}

void GyroSensor::update(uint64_t t) {
	struct Matrix w = _uav->get_angular_velocity();

    /* ADDING NOISE
    */
    float t_in_s = t / 1000000.0f;

    float p_noise = sinf(t_in_s * 80.0f *  2.0f * PI);
    float q_noise = sinf(t_in_s * 65.0f * 2.0f * PI);
    float r_noise = sinf(t_in_s * 75.0f *  2.0f * PI);

    _p = w.rows[0][0] + p_noise * _noise_level;
    _q = w.rows[1][0] + q_noise * _noise_level;
    _r = w.rows[2][0] + r_noise * _noise_level;
}

std::vector<float> GyroSensor::get_data() {
	return { _p, _q, _r };
}
