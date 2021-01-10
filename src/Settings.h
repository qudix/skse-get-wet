#pragma once

static constexpr char DEFAULT_FILE[] = "Data/Configs/qdx-get-wet/Settings/Default.json";
static constexpr char CUSTOM_FILE[] = "Data/Configs/qdx-get-wet/Settings/Custom.json";

class Settings
{
public:
	static Settings* GetSingleton()
	{
		static Settings singleton;
		return std::addressof(singleton);
	}

	template<typename T>
	static T Get(std::string key)
	{
		auto settings = GetSingleton();
		return settings->data[key].get<T>();
	}
	
	static json& Get(std::string key)
	{
		auto settings = GetSingleton();
		return settings->data[key];
	}

	static void Set(std::string key, json value)
	{
		auto settings = GetSingleton();
		settings->data[key] = value;
	}

	static bool Load()
	{
		auto settings = GetSingleton();
		auto& data = settings->data;
		auto& data_default = settings->data_default;

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
			data_default = j;

			if (has_custom) {
				json c;
				in_custom >> c;
				in_custom.close();
				j.update(c);
			}

			data = j;
		} catch (const std::exception& e) {
			logger::error("Failed to parse .json\n{}", e.what());
			return false;
		}

		return true;
	}

	static void Save()
	{
		auto settings = GetSingleton();
		auto& data = settings->data;
		auto& data_default = settings->data_default;

		json j = json::object();
		json::iterator it;
		for (auto& item : data.items()) {
			auto& key = item.key();
			it = data_default.find(key);
			if (it != data_default.end()) {
				auto& value = item.value();
				if (value != it.value())
					j[key] = value;
			}
		}

		if (!j.empty()) {
			std::ofstream out_custom(CUSTOM_FILE);
			if (out_custom.is_open()) {
				out_custom << std::setw(2) << j;
				out_custom.close();
			}
		}
	}

public:
	json data;
	json data_default;

private:
	Settings() = default;
	Settings(const Settings&) = delete;
	Settings(Settings&&) = delete;

	inline ~Settings() { Save(); };

	Settings& operator=(const Settings&) = delete;
	Settings& operator=(Settings&&) = delete;
};
