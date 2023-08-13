#include "airplane.hpp"

Airplane::Airplane() {
	_gyro_sensor = new GyroSensor(this, 1.0f);
    _accelerometer_sensor = new AccelerometerSensor(this, 1.0f);

	_sensors.push_back(_gyro_sensor);
    _sensors.push_back(_accelerometer_sensor);
}

Airplane::~Airplane() {
    delete _gyro_sensor;
    delete _accelerometer_sensor;
}

void Airplane::state_estimation_update(float delta_t) {

}

void Airplane::control_update(float delta_t) {

}

void Airplane::forces_update(float delta_t) {

}

void Airplane::moments_update(float delta_t) {

}

void Airplane::communication_thread() {

}

void Airplane::communication_non_blocking_thread() {

}
