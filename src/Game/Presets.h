#pragma once

class Presets final
{
public:
	struct Preset
	{
		std::string Form;
		float WetnessMin{ 0.0f };
		float WetnessMax{ 0.0f };
		float GlossinessMin{ 0.0f };
		float GlossinessMax{ 0.0f };
		float SpecularMin{ 0.0f };
		float SpecularMax{ 0.0f };
		bool ForceWetness{ false };
		bool ForceGlossiness{ false };
		bool ForceSpecular{ false };
		JSX_ALL(Preset, Form, WetnessMin, WetnessMax, GlossinessMin, GlossinessMax,
			SpecularMin, SpecularMax, ForceWetness, ForceGlossiness, ForceSpecular)
	};

	[[nodiscard]]
	static Presets& GetSingleton() noexcept;

	bool Load();
	void Save();

	void AddPreset(RE::Actor* a_actor);
	void RemovePreset(RE::Actor* a_actor);

public:
	std::unordered_map<RE::FormID, Preset> presets;

private:
	Presets() = default;
	Presets(const Presets&) = delete;
	Presets(Presets&&) = delete;

	inline ~Presets() { Save(); };

	Presets& operator=(const Presets&) = delete;
	Presets& operator=(Presets&&) = delete;
};
