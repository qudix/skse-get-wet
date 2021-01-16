#pragma once

namespace PapyrusSettings
{
	using VM = RE::BSScript::IVirtualMachine;

	std::vector<int> GetIntSettings(RE::StaticFunctionTag*)
	{
		return {
			// Something...
		};
	}

	std::vector<float> GetFloatSettings(RE::StaticFunctionTag*)
	{
		auto settings = Settings::GetSingleton();
		return {
			settings->Get<float>("VisualGlossinessMin"),
			settings->Get<float>("VisualGlossinessMax"),
			settings->Get<float>("VisualSpecularMin"),
			settings->Get<float>("VisualSpecularMax")
		};
	}

	std::vector<bool> GetBoolSettings(RE::StaticFunctionTag*)
	{
		auto settings = Settings::GetSingleton();
		return {
			settings->Get<bool>("ApplyGlobal"),
			settings->Get<bool>("ApplyPlayer"),
			settings->Get<bool>("ApplyNPC"),
			settings->Get<bool>("ApplyFemale"),
			settings->Get<bool>("ApplyMale"),
			settings->Get<bool>("ApplyBeast"),
			settings->Get<bool>("VisualGlossinessHead"),
			settings->Get<bool>("VisualSpecularHead"),
			settings->Get<bool>("VisualGlossinessBody"),
			settings->Get<bool>("VisualSpecularBody"),
			settings->Get<bool>("VisualGlossinessHands"),
			settings->Get<bool>("VisualSpecularHands"),
			settings->Get<bool>("VisualGlossinessFeet"),
			settings->Get<bool>("VisualSpecularFeet"),
			settings->Get<bool>("VisualGlossinessOther"),
			settings->Get<bool>("VisualSpecularOther")
		};
	}

	void SetIntSetting(RE::StaticFunctionTag*, std::string key, int a_val)
	{
		auto settings = Settings::GetSingleton();
		settings->Set(key, a_val);
		settings->Save();
	}

	void SetFloatSetting(RE::StaticFunctionTag*, std::string key, float a_val)
	{
		auto settings = Settings::GetSingleton();
		settings->Set(key, a_val);
		settings->Save();
	}

	void SetBoolSetting(RE::StaticFunctionTag*, std::string key, bool a_val)
	{
		auto settings = Settings::GetSingleton();
		settings->Set(key, a_val);
		settings->Save();
	}

	void Bind(VM& a_vm)
	{
		const auto obj = "qdx_gw"sv;

		BIND(GetIntSettings);
		BIND(GetFloatSettings);
		BIND(GetBoolSettings);

		BIND(SetIntSetting);
		BIND(SetFloatSetting);
		BIND(SetBoolSetting);
	}
}
