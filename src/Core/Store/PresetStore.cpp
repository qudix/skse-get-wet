#include "Store/PresetStore.h"
#include "Store/SettingStore.h"

#include "Util.h"
#include <nlohmann/json.hpp>

static constexpr char PRESETS_FILE[] = "Data/Configs/qdx-get-wet/Settings/Presets.json";

using json = nlohmann::basic_json<nlohmann::ordered_map, std::vector, std::string, bool, std::int64_t, std::uint64_t, float>;

auto PresetStore::GetSingleton() -> PresetStore&
{
	static PresetStore singleton;
	return singleton;
}

bool PresetStore::Load()
{
	std::ifstream in_presets(PRESETS_FILE);
	if (!in_presets.is_open())
		return false;

	auto& setting = SettingStore::GetSingleton().Get();
	try {
		json j;
		in_presets >> j;
		in_presets.close();

		for (auto& obj : j) {
			auto formStr = obj.value("Form", "");
			auto form = Util::AsForm(formStr);
			if (form) {
				auto formID = form->GetFormID();
				Preset preset{ form };
				preset.WetnessMin = obj.value("WetnessMin", setting.WetnessMin);
				preset.WetnessMax = obj.value("WetnessMax", setting.WetnessMax);
				preset.WetnessForced = obj.value("WetnessForced", false);

				preset.GlossinessMin = obj.value("GlossinessMin", setting.GlossinessMin);
				preset.GlossinessMax = obj.value("GlossinessMax", setting.GlossinessMax);
				preset.GlossinessForced = obj.value("GlossinessForced", false);

				preset.SpecularMin = obj.value("SpecularMin", setting.SpecularMin);
				preset.SpecularMax = obj.value("SpecularMax", setting.SpecularMax);
				preset.SpecularForced = obj.value("SpecularForced", false);

				_presets[formID] = std::move(preset);
			} else {
				logger::info("Bad Form: {}", formStr);
			}
		}

	} catch (const std::exception& e) {
		logger::error("Failed to parse .json\n{}", e.what());
		return false;
	}

	return true;
}

void PresetStore::Save()
{
	auto j = json::array();
	for (auto& [formID, preset] : _presets) {
		auto obj = json::object();
		obj["Form"] = Util::AsString(preset.Form);

		obj["WetnessMin"] = preset.WetnessMin;
		obj["WetnessMax"] = preset.WetnessMax;
		obj["WetnessForced"] = preset.WetnessForced;

		obj["GlossinessMin"] = preset.GlossinessMin;
		obj["GlossinessMax"] = preset.GlossinessMax;
		obj["GlossinessForced"] = preset.GlossinessForced;

		obj["SpecularMin"] = preset.SpecularMin;
		obj["SpecularMax"] = preset.SpecularMax;
		obj["SpecularForced"] = preset.SpecularForced;

		j.push_back(obj);
	}

	if (!j.empty()) {
		std::ofstream out_custom(PRESETS_FILE);
		if (out_custom.is_open()) {
			out_custom << std::setw(4) << j;
			out_custom.close();
		}
	}
}

auto PresetStore::Find(RE::FormID a_formID) -> Preset*
{
	auto it = _presets.find(a_formID);
	if (it != _presets.end()) {
		return &it->second;
	}

	return nullptr;
}

void PresetStore::Add(RE::Actor* a_actor)
{
	auto formID = a_actor->GetFormID();
	auto it = _presets.find(formID);
	if (it != _presets.end())
		return;

	auto& setting = SettingStore::GetSingleton().Get();
	Preset preset;
	preset.Form = a_actor;
	preset.WetnessMin = setting.WetnessMin;
	preset.WetnessMax = setting.WetnessMax;
	preset.GlossinessMin = setting.GlossinessMin;
	preset.GlossinessMax = setting.GlossinessMax;
	preset.SpecularMin = setting.SpecularMin;
	preset.SpecularMax = setting.SpecularMax;
	_presets[formID] = std::move(preset);
	Save();
}

void PresetStore::Remove(RE::Actor* a_actor)
{
	auto formID = a_actor->GetFormID();
	auto it = _presets.find(formID);
	if (it != _presets.end()) {
		_presets.erase(it);
		Save();
	}
}
