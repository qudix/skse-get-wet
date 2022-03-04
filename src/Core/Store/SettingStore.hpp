#pragma once

struct Setting
{
	bool ApplyGlobal{ false };
	bool ApplyPlayer{ false };
	bool ApplyNPC{ false };
	bool ApplyMale{ false };
	bool ApplyFemale{ false };
	bool ApplyBeast{ false };

	float WetnessMin{ 0 };
	float WetnessMax{ 0 };
	float WetnessRateMax{ 0 };

	float GlossinessMin{ 0 };
	float GlossinessMax{ 0 };
	bool GlossinessHead{ false };
	bool GlossinessBody{ false };
	bool GlossinessHands{ false };
	bool GlossinessFeet{ false };
	bool GlossinessOther{ false };

	float SpecularMin{ 0 };
	float SpecularMax{ 0 };
	bool SpecularHead{ false };
	bool SpecularBody{ false };
	bool SpecularHands{ false };
	bool SpecularFeet{ false };
	bool SpecularOther{ false };

	float ActivityStamina{ 0 };
	float ActivityMagicka{ 0 };
	float ActivitySprinting{ 0 };
	float ActivityRunning{ 0 };
	float ActivitySneaking{ 0 };
	float ActivityGalloping{ 0 };
	float ActivityWorking{ 0 };
};

class SettingStore final
{
public:
	static SettingStore& GetSingleton();

	void Load();
	void LoadDefault();
	void LoadSettings();
	bool Read(const fs::path& a_ini, Setting& a_store);

	auto Get() -> Setting&;

private:
	SettingStore() = default;
	SettingStore(const SettingStore&) = delete;
	SettingStore(SettingStore&&) = delete;

	~SettingStore() = default;

	SettingStore& operator=(const SettingStore&) = delete;
	SettingStore& operator=(SettingStore&&) = delete;

	Setting _default;
	Setting _setting;
};
