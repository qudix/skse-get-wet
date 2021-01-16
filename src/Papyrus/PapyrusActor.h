#pragma once

namespace PapyrusActor
{
	using VM = RE::BSScript::IVirtualMachine;

	void Update(RE::StaticFunctionTag*)
	{
		Actor::Update();
	}

	void UpdateActor(RE::StaticFunctionTag*, RE::Actor* a_actor)
	{
		Actor::UpdateActor(a_actor);
	}
	
	std::vector<RE::Actor*> GetActorList(RE::StaticFunctionTag*)
	{
		std::vector<RE::Actor*> a_vec;

		for (auto& props : Actor::m_actorProps) {
			auto actor = RE::TESObjectREFR::LookupByID<RE::Actor>(props->m_ID);
			if (actor)
				a_vec.push_back(actor);
		}
		return a_vec;
	}

	std::vector<RE::BSFixedString> GetActorNameList(RE::StaticFunctionTag*)
	{
		std::vector<RE::BSFixedString> n_vec;

		for (auto& props : Actor::m_actorProps) {
			auto actor = RE::TESObjectREFR::LookupByID<RE::Actor>(props->m_ID);
			if (actor)
				n_vec.push_back(actor->GetName());
		}
		return n_vec;
	}

	std::vector<float> GetActorFloatSettings(RE::StaticFunctionTag*, RE::Actor* a_actor)
	{
		if (!a_actor)
			return {};

		std::vector<float> vec;

		for (auto& props : Actor::m_actorProps) {
			auto actor = RE::TESObjectREFR::LookupByID<RE::Actor>(props->m_ID);
			if (actor == a_actor) {
				if (props->m_HasSettings) {
					vec.push_back(props->m_GlossinessMin);
					vec.push_back(props->m_GlossinessMax);
					vec.push_back(props->m_SpecularMin);
					vec.push_back(props->m_SpecularMax);
				}
				break;
			}
		}

		return vec;
	}

	void SetActorFloatSetting(RE::StaticFunctionTag*, RE::Actor* a_actor, std::string a_key, float a_value)
	{
		SettingsActor::SetActorSetting(a_actor, a_key, a_value);
	}

	void AddActorSettings(RE::StaticFunctionTag*, RE::Actor* a_actor)
	{
		SettingsActor::AddActorSettings(a_actor);
	}

	void RemoveActorSettings(RE::StaticFunctionTag*, RE::Actor* a_actor)
	{
		SettingsActor::RemoveActorSettings(a_actor);
	}

	void Bind(VM& a_vm)
	{
		const auto obj = "qdx_gw"sv;

		BIND(Update);
		BIND(UpdateActor);
		BIND(GetActorList);
		BIND(GetActorNameList);
		BIND(GetActorFloatSettings);
		BIND(SetActorFloatSetting);
		BIND(AddActorSettings);
		BIND(RemoveActorSettings);
	}
}