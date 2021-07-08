#include "Game/Presets.h"
#include "Game/Config.h"

#include "Util.h"

static constexpr char PRESETS_FILE[] = "Data/Configs/qdx-get-wet/Settings/Presets.json";

Presets& Presets::GetSingleton() noexcept
{
	static Presets singleton;
	return singleton;
}

bool Presets::Load()
{
	std::ifstream in_presets(PRESETS_FILE);
	if (in_presets.is_open()) {
		try {
			json j;
			in_presets >> j;
			in_presets.close();

			for (auto& obj : j) {
				auto formStr = obj["Form"].get<std::string>();
				auto form = Util::AsForm(formStr);
				if (form) {
					auto formID = form->GetFormID();
					Preset preset = obj;
					presets[formID] = std::move(preset);
				} else {
					logger::info("Bad Form: {}", formStr);
				}
			}
		} catch (const std::exception& e) {
			logger::error("Failed to parse .json\n{}", e.what());
			return false;
		}
	}

	return true;
}

void Presets::Save()
{
	auto j = json::array();
	for (auto& [formID, preset] : presets) {
		j.push_back(preset);
	}

	if (!j.empty()) {
		std::ofstream out_custom(PRESETS_FILE);
		if (out_custom.is_open()) {
			out_custom << std::setw(2) << j;
			out_custom.close();
		}
	}
}

void Presets::AddPreset(RE::Actor* a_actor)
{
	auto formID = a_actor->GetFormID();
	auto it = presets.find(formID);
	if (it != presets.end())
		return;

	auto& cfg = Config::GetSingleton();
	Preset preset;
	preset.Form = Util::AsString(a_actor);
	preset.WetnessMin = cfg.data.WetnessMin;
	preset.WetnessMax = cfg.data.WetnessMax;
	preset.GlossinessMin = cfg.data.GlossinessMin;
	preset.GlossinessMax = cfg.data.GlossinessMax;
	preset.SpecularMin = cfg.data.SpecularMin;
	preset.SpecularMax = cfg.data.SpecularMax;
	presets[formID] = std::move(preset);
	Save();
}

void Presets::RemovePreset(RE::Actor* a_actor)
{
	auto formID = a_actor->GetFormID();
	auto it = presets.find(formID);
	if (it != presets.end()) {
		presets.erase(it);
		Save();
	}
}
