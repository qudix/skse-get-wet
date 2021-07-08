#include "Game/Config.h"

static constexpr char DEFAULT_FILE[] = "Data/Configs/qdx-get-wet/Settings/Default.json";
static constexpr char CUSTOM_FILE[] = "Data/Configs/qdx-get-wet/Settings/Custom.json";

Config& Config::GetSingleton() noexcept
{
	static Config singleton;
	return singleton;
}

bool Config::Load()
{
	std::ifstream in_default(DEFAULT_FILE);
	if (!in_default.is_open()) {
		logger::error("Failed to open default .json");
		return false;
	}

	bool has_custom = false;
	std::ifstream in_custom(CUSTOM_FILE);
	if (in_custom.is_open())
		has_custom = true;

	try {
		json j;
		in_default >> j;
		in_default.close();

		defu = j;
		if (has_custom) {
			json c;
			in_custom >> c;
			in_custom.close();
			j.update(c);
		}

		data = j;
		Save();
	} catch (const std::exception& e) {
		logger::error("Failed to parse .json\n{}", e.what());
		return false;
	}

	return true;
}

void Config::Save()
{
	json jdefu = defu;
	json jdata = data;

	auto j = json::object();
	for (auto& itemA : jdata.items()) {
		auto& keyA = itemA.key();
		auto it = jdefu.find(keyA);
		if (it != jdefu.end()) {
			auto& value = itemA.value();
			if (value != it.value())
				j[keyA] = value;
		}
	}

	//auto dump = j.dump(4);
	//logger::info("result: \n{}"sv, dump);

	if (!j.empty()) {
		std::ofstream out_custom(CUSTOM_FILE);
		if (out_custom.is_open()) {
			out_custom << std::setw(2) << j;
			out_custom.close();
		}
	}
}
