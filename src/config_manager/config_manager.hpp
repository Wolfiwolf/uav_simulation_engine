#pragma once

#include <string>
#include <unordered_map>

class ConfigManager {
	private:
		static std::unordered_map<std::string, std::string> _params;

	public:
		static void init();

		static bool get_param_val(const std::string &key, std::string &val);
};

