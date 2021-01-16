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
		std::vector<RE::Actor*> vec;

		for (auto& props : Actor::m_actorProps) {
			auto actor = RE::TESObjectREFR::LookupByID<RE::Actor>(props->m_ID);
			if (actor) {
				if (actor->IsPlayerRef())
					vec.insert(vec.begin(), actor);
				else
					vec.push_back(actor);
			}
		}

		return vec;
	}

	std::vector<RE::BSFixedString> GetActorNameList(RE::StaticFunctionTag*)
	{
		std::vector<RE::BSFixedString> vec;

		for (auto& props : Actor::m_actorProps) {
			auto actor = RE::TESObjectREFR::LookupByID<RE::Actor>(props->m_ID);
			if (actor) {
				auto name = actor->GetName();
				if (actor->IsPlayerRef())
					vec.insert(vec.begin(), name);
				else
					vec.push_back(name);
			}
		}

		return vec;
	}

	std::vector<float> GetActorFloatSettings(RE::StaticFunctionTag*, RE::Actor* a_actor)
	{
		std::vector<float> vec;
		if (!a_actor)
			return vec;

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