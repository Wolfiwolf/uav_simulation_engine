#include <iostream>
#include "config_manager/config_manager.hpp"
#include "streamer/streamer.hpp"
#include "uav/uavs/quad_copter/quad_copter.hpp"
#include <chrono>

int main(int argc, char *argv[]) {
	ConfigManager::init();

	UAV *uav = new QuadCopter();

	Streamer streamer;

	auto start_time = std::chrono::high_resolution_clock::now();
	auto current_time = start_time;
	auto prev_time = current_time;
	
	while (true) {
		float delta_time = (float)(std::chrono::duration_cast<std::chrono::microseconds>(current_time - prev_time).count()) / 1000000.0f;

		uav->update(delta_time);

		streamer.stream_data(uav);
	}

	delete uav;
	return 0;
}

