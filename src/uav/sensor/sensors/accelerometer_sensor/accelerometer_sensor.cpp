#include "accelerometer_sensor.hpp"

AccelerometerSensor::AccelerometerSensor(UAV *uav, float noise_level): Sensor(uav, noise_level) {
    _x = 0.0f;
    _y = 0.0f;
    _z = 0.0f;
}

void AccelerometerSensor::update(uint64_t t) {
    struct Matrix forces = _uav->get_forces();
    struct Matrix euler = _uav->get_orientation_euler_angles_ZYX();

    struct Matrix forces_temp;
    uav_matrix_init(&forces_temp, 3, 1);
    uav_matrix_copy(&forces, &forces_temp);


    uav_rotation_body_to_inertial(&forces_temp, &euler);

    uav_matrix_scalar_multiply(&forces_temp, 1.0f / (_uav->get_mass() * 9.8f));

    forces_temp.rows[2][0] -= 1.0f;

    uav_rotation_inertial_to_body(&forces_temp, &euler);


    _x = forces_temp.rows[0][0];
    _y = forces_temp.rows[1][0];
    _z = forces_temp.rows[2][0];

    uav_matrix_destroy(&forces_temp);
}

std::vector<float> AccelerometerSensor::get_data() {
    return {_x, _y, _z};
}
