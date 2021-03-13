#pragma once

static constexpr char DEFAULT_FILE[] = "Data/Configs/qdx-get-wet/Settings/Default.json";
static constexpr char CUSTOM_FILE[] = "Data/Configs/qdx-get-wet/Settings/Custom.json";

#define QDX_ALL(Type, ...)\
	friend void from_json(const json& nlohmann_json_j, Type& nlohmann_json_t) { NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_FROM, __VA_ARGS__)) }\
	friend void to_json(json& nlohmann_json_j, const Type& nlohmann_json_t) { NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_TO, __VA_ARGS__)) }

struct Apply_t
{
	bool Global, Player, NPC, Female, Male, Beast;
	QDX_ALL(Apply_t, Global, Player, NPC, Female, Male, Beast)
};

struct Activity_t
{
	float Stamina, Magicka, Sprinting, Running, Sneaking, Galloping, Working;
	QDX_ALL(Activity_t, Stamina, Magicka, Sprinting, Running, Sneaking, Galloping, Working)
};

struct Wetness_t
{
	float Min, Start, Soaked, Max, RateMax;
	QDX_ALL(Wetness_t, Min, Start, Soaked, Max, RateMax)
};

struct Visual_t
{
	float Min, Max;
	bool Head, Body, Hands, Feet, Other;
	QDX_ALL(Visual_t, Min, Max, Head, Body, Hands, Feet, Other)
};

struct Actor_t
{
	std::string Actor;
	float Wetness, Glossiness, Specular;
	QDX_ALL(Actor_t, Actor, Wetness, Glossiness, Specular)
};

struct Base_t
{
	Apply_t Apply;
	Activity_t Activity;
	Wetness_t Wetness;
	Visual_t Glossiness;
	Visual_t Specular;
	std::vector<Actor_t> Actors;
	QDX_ALL(Base_t, Apply, Activity, Wetness, Glossiness, Specular, Actors)
};

class Settings
{
public:
	static Settings* GetSingleton()
	{
		static Settings singleton;
		return std::addressof(singleton);
	}

	static bool Load()
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

			auto set = GetSingleton();
			set->defu = j;

			if (has_custom) {
				json c;
				in_custom >> c;
				in_custom.close();
				for (auto& it : j.items()) {
					auto& key = it.key();
					if (c.find(key) != c.end())
						j[key].update(c[key]);
				}
			}

			set->data = j;
			set->Save();
		} catch (const std::exception& e) {
			logger::error("Failed to parse .json\n{}", e.what());
			return false;
		}

		return true;
	}

	static void Save()
	{
		auto set = GetSingleton();
		json defu = set->defu;
		json data = set->data;

		auto j = json::object();
		for (auto& itemA : data.items()) {
			auto& keyA = itemA.key();
			if (defu.find(keyA) == defu.end())
				continue;

			for (auto& itemB : itemA.value().items()) {
				auto& keyB = itemB.key();
				auto itB = defu[keyA].find(keyB);
				if (itB != defu[keyA].end()) {
					auto& value = itemB.value();
					if (value != itB.value())
						j[keyA][keyB] = value;
				}
			}
		}

		auto dump = j.dump(4);
		logger::info("result: \n{}"sv, dump);

		if (!j.empty()) {
			std::ofstream out_custom(CUSTOM_FILE);
			if (out_custom.is_open()) {
				out_custom << std::setw(2) << j;
				out_custom.close();
			}
		}
	}

public:
	Base_t defu;
	Base_t data;

private:
	Settings() = default;
	Settings(const Settings&) = delete;
	Settings(Settings&&) = delete;

	inline ~Settings() { Save(); };

	Settings& operator=(const Settings&) = delete;
	Settings& operator=(Settings&&) = delete;
};
