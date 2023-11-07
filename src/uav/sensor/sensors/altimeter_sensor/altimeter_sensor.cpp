#include "altimeter_sensor.hpp"
#include <math.h>

#define PI 3.141592653f

AltimeterSensor::AltimeterSensor(UAV *uav, float noise_level): Sensor(uav, noise_level) {}

void AltimeterSensor::update(uint64_t t) {
	struct Matrix pos = _uav->get_position();

    float t_in_s = t / 1000000.0f;

    float noise = sinf(t_in_s * 80.0f *  2.0f * PI);

    _altitude = pos.rows[2][0] + noise * _noise_level;
}

std::vector<float> AltimeterSensor::get_data() {
	return { _altitude + 270 };
}

