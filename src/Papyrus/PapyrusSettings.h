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
			settings->Get<float>("UpdateLoopFreq"),
			settings->Get<float>("UpdateLoopRange"),
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

	static constexpr char CLASS_NAME[] = "qdx_gw";

	bool Register(VM* a_vm)
	{
		if (!a_vm) {
			logger::critical("Papyrus Settings: Couldn't get VM");
			return false;
		}

		a_vm->RegisterFunction("GetIntSettings", CLASS_NAME, GetIntSettings);
		a_vm->RegisterFunction("GetFloatSettings", CLASS_NAME, GetFloatSettings);
		a_vm->RegisterFunction("GetBoolSettings", CLASS_NAME, GetBoolSettings);

		a_vm->RegisterFunction("SetIntSetting", CLASS_NAME, SetIntSetting);
		a_vm->RegisterFunction("SetFloatSetting", CLASS_NAME, SetFloatSetting);
		a_vm->RegisterFunction("SetBoolSetting", CLASS_NAME, SetBoolSetting);

		//a_vm->RegisterFunction("GetActorIntSettings", CLASS_NAME, GetActorIntSettings);
		return true;
	}
}
