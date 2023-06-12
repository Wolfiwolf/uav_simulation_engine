#include <iostream>
#include "config_manager/config_manager.hpp"
#include "streamer/streamer.hpp"
#include "uav/uavs/quad_copter/quad_copter.hpp"
#include "data_link/data_link.hpp"
#include <chrono>
#include <string>
#include <thread>


void run_data_link(UAV *uav, DataLink *data_link);
void run_simulation(UAV *uav, DataLink *data_link);

int main(int argc, char *argv[]) {
	ConfigManager::init();

	std::string port_str;
	ConfigManager::get_param_val("data_link_port", port_str);
	int port = std::stoi(port_str);

	UAV *uav;
	DataLink data_link(uav, port);
	uav = new QuadCopter();

	std::thread simulation_thread(run_simulation, uav, &data_link);
	std::thread data_link_thread(run_data_link, uav, &data_link);

	simulation_thread.join();
	data_link_thread.join();

	delete uav;

	return 0;
}

void run_simulation(UAV *uav, DataLink *data_link) {
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
}

void run_data_link(UAV *uav, DataLink *data_link) {

	while (true) {
		data_link->wait_for_link();

		uint8_t rx_buffer[1024];
		while (true) {
			uint32_t rx_buf_len = data_link->listen_for_msg(rx_buffer);
		}
	}
}
