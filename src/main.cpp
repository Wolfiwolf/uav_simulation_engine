#include <iostream>
#include "config_manager/config_manager.hpp"
#include "streamer/streamer.hpp"
#include "uav/uavs/quad_copter/quad_copter.hpp"
#include <chrono>

int main(int argc, char *argv[]) {
	ConfigManager::init();

	UAV *uav = new QuadCopter();

	Streamer streamer;

	auto current_time = std::chrono::high_resolution_clock::now();
	auto prev_time = current_time;

	uint64_t t = 0;

	while (true) {
		current_time = std::chrono::high_resolution_clock::now();
		uint64_t delta_time_us = std::chrono::duration_cast<std::chrono::microseconds>(current_time - prev_time).count();
		prev_time = current_time;
		float delta_time = (float)(delta_time_us) / 1000000.0f;

		t += delta_time_us;
		std::cout << delta_time_us << "\n";

		uav->update(t, delta_time);

		streamer.stream_data(t, uav);
	}

	delete uav;
	return 0;
}

