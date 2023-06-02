#include <iostream>
#include "uav/uavs/quad_copter/quad_copter.hpp"

int main(int argc, char *argv[]) {
	UAV *uav = new QuadCopter();

	delete uav;
	return 0;
}

