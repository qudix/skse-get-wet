#include "Store/SettingStore.h"

#include <SimpleIni.h>

auto SettingStore::GetSingleton() -> SettingStore&
{
	static SettingStore singleton;
	return singleton;
}

void SettingStore::Load()
{
	logger::info("Loading settings"sv);
	LoadDefault();
	LoadSettings();
}

void SettingStore::LoadDefault()
{
	fs::path configPath{ "Data/MCM/Config/GetWet"sv };
	fs::directory_entry configDir{ configPath };
	if (!configDir.exists() || !configDir.is_directory())
		return;

	auto iniPath = configPath / "settings.ini"sv;
	fs::directory_entry ini{ iniPath };
	if (ini.exists() && ini.is_regular_file()) {
		Read(iniPath, _default);
		_setting = _default;
	}
}

void SettingStore::LoadSettings()
{
	fs::path settingsPath{ "Data/MCM/Settings"sv };
	fs::directory_entry settingsDir{ settingsPath };
	if (!settingsDir.exists() || !settingsDir.is_directory())
		return;

	auto iniPath = settingsPath / "GetWet.ini"sv;
	fs::directory_entry ini{ iniPath };
	if (ini.exists() && ini.is_regular_file()) {
		Read(iniPath, _setting);
	}
}

bool SettingStore::Read(const fs::path& a_ini, Setting& a_store)
{
	CSimpleIniA ini;
	ini.SetUnicode();

	SI_Error rc = ini.LoadFile(a_ini.c_str());
	if (rc < 0) {
		logger::warn("Failed to parse ini: {}"sv, a_ini.string());
		return false;
	}

	a_store.ApplyGlobal = ini.GetBoolValue("Apply", "bGlobal", _default.ApplyGlobal);
	a_store.ApplyPlayer = ini.GetBoolValue("Apply", "bPlayer", _default.ApplyPlayer);
	a_store.ApplyNPC = ini.GetBoolValue("Apply", "bNPC", _default.ApplyNPC);
	a_store.ApplyMale = ini.GetBoolValue("Apply", "bMale", _default.ApplyMale);
	a_store.ApplyFemale = ini.GetBoolValue("Apply", "bFemale", _default.ApplyFemale);
	a_store.ApplyBeast = ini.GetBoolValue("Apply", "bBeast", _default.ApplyBeast);

	a_store.WetnessMin = static_cast<float>(ini.GetDoubleValue("Wetness", "fMin", _default.WetnessMin));
	a_store.WetnessMax = static_cast<float>(ini.GetDoubleValue("Wetness", "fMax", _default.WetnessMax));
	a_store.WetnessRateMax = static_cast<float>(ini.GetDoubleValue("Wetness", "fRateMax", _default.WetnessRateMax));

	a_store.GlossinessMin = static_cast<float>(ini.GetDoubleValue("Glossiness", "fMin", _default.GlossinessMin));
	a_store.GlossinessMax = static_cast<float>(ini.GetDoubleValue("Glossiness", "fMax", _default.GlossinessMax));
	a_store.GlossinessHead = ini.GetBoolValue("Glossiness", "bHead", _default.GlossinessHead);
	a_store.GlossinessBody = ini.GetBoolValue("Glossiness", "bBody", _default.GlossinessBody);
	a_store.GlossinessHands = ini.GetBoolValue("Glossiness", "bHands", _default.GlossinessHands);
	a_store.GlossinessFeet = ini.GetBoolValue("Glossiness", "bFeet", _default.GlossinessFeet);
	a_store.GlossinessOther = ini.GetBoolValue("Glossiness", "bOther", _default.GlossinessOther);

	a_store.SpecularMin = static_cast<float>(ini.GetDoubleValue("Specular", "fMin", _default.SpecularMin));
	a_store.SpecularMax = static_cast<float>(ini.GetDoubleValue("Specular", "fMax", _default.SpecularMax));
	a_store.SpecularHead = ini.GetBoolValue("Specular", "bHead", _default.SpecularHead);
	a_store.SpecularBody = ini.GetBoolValue("Specular", "bBody", _default.SpecularBody);
	a_store.SpecularHands = ini.GetBoolValue("Specular", "bHands", _default.SpecularHands);
	a_store.SpecularFeet = ini.GetBoolValue("Specular", "bFeet", _default.SpecularFeet);
	a_store.SpecularOther = ini.GetBoolValue("Specular", "bOther", _default.SpecularOther);

	a_store.ActivityStamina = static_cast<float>(ini.GetDoubleValue("Activity", "fStamina", _default.ActivityStamina));
	a_store.ActivityMagicka = static_cast<float>(ini.GetDoubleValue("Activity", "fMagicka", _default.ActivityMagicka));
	a_store.ActivitySprinting = static_cast<float>(ini.GetDoubleValue("Activity", "fSprinting", _default.ActivitySprinting));
	a_store.ActivityRunning = static_cast<float>(ini.GetDoubleValue("Activity", "fRunning", _default.ActivityRunning));
	a_store.ActivitySneaking = static_cast<float>(ini.GetDoubleValue("Activity", "fSneaking", _default.ActivitySneaking));
	a_store.ActivityGalloping = static_cast<float>(ini.GetDoubleValue("Activity", "fGalloping", _default.ActivityGalloping));
	a_store.ActivityWorking = static_cast<float>(ini.GetDoubleValue("Activity", "fWorking", _default.ActivityWorking));

	return true;
}

auto SettingStore::Get() -> Setting&
{
	return _setting;
}
