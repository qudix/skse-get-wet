#pragma once

namespace PapyrusSettings
{
	using VM = RE::BSScript::IVirtualMachine;

	constexpr const char* CLASS_CONFIG = "qdx_gw_QuestConfig";

	void GetConfig(RE::StaticFunctionTag*)
	{
		auto& meta = Meta::GetSingleton();
		auto obj = stl::get(meta.m_QuestConfig, CLASS_CONFIG);
		if (!obj)
			return;

		auto& cfg = Config::GetSingleton();
		json data = cfg.data;
		for (auto& item : data.items()) {
			auto value = item.value();
			if (value.is_boolean()) {
				stl::set(obj, item.key(), value.get<bool>());
			} else if (value.is_number_float()) {
				stl::set(obj, item.key(), value.get<float>());
			}
		}
	}

	void SetConfig(RE::StaticFunctionTag*)
	{
		auto& meta = Meta::GetSingleton();
		auto obj = stl::get(meta.m_QuestConfig, CLASS_CONFIG);
		if (!obj)
			return;

		auto& cfg = Config::GetSingleton();
		json data = cfg.data;
		for (auto& item : data.items()) {
			auto& value = item.value();
			if (value.is_boolean()) {
				value = stl::get<bool>(obj, item.key());
			} else if (value.is_number_float()) {
				value = stl::get<float>(obj, item.key());
			}
		}

		cfg.data = data;
		cfg.Save();
	}

	bool Bind(VM* a_vm)
	{
		const auto obj = "qdx_gw"sv;

		BIND(GetConfig);
		BIND(SetConfig);

		return true;
	}
}
