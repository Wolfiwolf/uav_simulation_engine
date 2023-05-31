
#include "quad_copter.hpp"
#include <iostream>

QuadCopter::QuadCopter() {
}

QuadCopter::~QuadCopter() {
}

void QuadCopter::specific_init() {
	std::cout << "Hello, I am quad!\n";
}

void QuadCopter::control_update(float delta_t) {

}

void QuadCopter::forces_update(float delta_t) {

}

void QuadCopter::moments_update(float delta_t) {

}
