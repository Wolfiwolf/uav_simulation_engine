#include "sensor.hpp"

Sensor::Sensor(UAV *uav, float noise_level) {
    _uav = uav;
    _noise_level = noise_level;
}

