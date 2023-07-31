#include "accelerometer_sensor.hpp"

AccelerometerSensor::AccelerometerSensor(UAV *uav, float noise_level): Sensor(uav, noise_level) {
    _x = 0.0f;
    _y = 0.0f;
    _z = 0.0f;
}

void AccelerometerSensor::update(uint64_t t) {
    struct Matrix forces = _uav->get_forces();

    struct Matrix forces_in_inertial;
    uav_matrix_init(&forces_in_inertial, 3, 1);

    uav_rotation_body_to_inertial(&forces, &forces_in_inertial);

    uav_matrix_scalar_multiply(&forces_in_inertial, 1.0f / (_uav->get_mass() * 9.8f));

    forces_in_inertial.rows[2][0] -= 1.0f;

    uav_rotation_inertial_to_body(&forces_in_inertial, &forces);

    uav_matrix_destroy(&forces_in_inertial);

    _x = forces.rows[0][0];
    _y = forces.rows[1][0];
    _z = forces.rows[2][0];
}

std::vector<float> AccelerometerSensor::get_data() {
    return {_x, _y, _z};
}
