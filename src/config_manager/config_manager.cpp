#include "config_manager.hpp"
#include <cstdio>
#include <fstream>
#include <iostream>

std::unordered_map<std::string, std::string> ConfigManager::_params;

void ConfigManager::init() {
	std::ifstream config_file("config.txt");

	std::string line;
	while(std::getline(config_file, line)) {
		if (line.size() < 3) break;
		uint32_t pos = line.find_first_of(':');
		std::string key = line.substr(0, pos);
		std::string val = line.substr(pos + 2, line.size() - pos - 2);

		_params[key] = val;
	}

	config_file.close();
}

std::string ConfigManager::get_param_val(const std::string &key) {
	return _params[key];
}

