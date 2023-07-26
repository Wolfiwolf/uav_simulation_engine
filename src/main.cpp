#include <cmath>
#include <iostream>
#include "config_manager/config_manager.hpp"
#include "streamer/streamer.hpp"
#include "uav/uavs/quad_copter/quad_copter.hpp"
#include <chrono> 
#include <string>
#include <thread>

void run_data_link(UAV *uav);
void run_data_link_non_blocking(UAV *uav);
void run_simulation(UAV *uav);

int main(int argc, char *argv[]) {
	ConfigManager::init();

	UAV *uav;
	uav = new QuadCopter();

	std::thread simulation_thread(run_simulation, uav);
	std::thread data_link_thread(run_data_link, uav);
	std::thread data_link_non_blocking_thread(run_data_link_non_blocking, uav);

	simulation_thread.join();
	data_link_thread.join();
	data_link_non_blocking_thread.join();

	delete uav;

	return 0;
}

void run_simulation(UAV *uav) {
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

		uav->update(t, delta_time);

        streamer.stream_data(t, uav);
	}
}

void run_data_link(UAV *uav) {
	uav->communication_thread();
}

void run_data_link_non_blocking(UAV *uav) {
	uav->communication_non_blocking_thread();
}
