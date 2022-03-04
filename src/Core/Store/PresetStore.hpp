#pragma once

struct Preset
{
	RE::TESForm* Form;

	float WetnessMin{ 0.0f };
	float WetnessMax{ 0.0f };
	bool WetnessForced{ false };

	float GlossinessMin{ 0.0f };
	float GlossinessMax{ 0.0f };
	bool GlossinessForced{ false };

	float SpecularMin{ 0.0f };
	float SpecularMax{ 0.0f };
	bool SpecularForced{ false };
};

class PresetStore final
{
	using Storage = std::unordered_map<RE::FormID, Preset>;

public:
	static PresetStore& GetSingleton();

	bool Load();
	void Save();

	auto Find(RE::FormID a_formID) -> Preset*;

	void Add(RE::Actor* a_actor);
	void Remove(RE::Actor* a_actor);

private:
	PresetStore() = default;
	PresetStore(const PresetStore&) = delete;
	PresetStore(PresetStore&&) = delete;

	~PresetStore() { Save(); };

	PresetStore& operator=(const PresetStore&) = delete;
	PresetStore& operator=(PresetStore&&) = delete;

	Storage _presets;
};
