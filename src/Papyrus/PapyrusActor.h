#pragma once

namespace Papyrus::PapyrusActor
{
	using VM = RE::BSScript::IVirtualMachine;

	inline void Update(RE::StaticFunctionTag*)
	{
		Actor::Update();
	}

	inline void UpdateActor(RE::StaticFunctionTag*, RE::Actor* a_actor)
	{
		if (!a_actor)
			return;

		Actor::UpdateActor(*a_actor);
	}

	inline std::vector<RE::Actor*> GetActorList(RE::StaticFunctionTag*)
	{
		std::vector<RE::Actor*> vec;

		for (auto& data : Actor::m_ActorData) {
			auto actor = RE::TESObjectREFR::LookupByID<RE::Actor>(data.first);
			if (actor) {
				if (actor->IsPlayerRef())
					vec.insert(vec.begin(), actor);
				else
					vec.push_back(actor);
			}
		}

		return vec;
	}

	inline std::vector<RE::BSFixedString> GetActorNameList(RE::StaticFunctionTag*)
	{
		std::vector<RE::BSFixedString> vec;
		for (auto& data : Actor::m_ActorData) {
			auto actor = RE::TESObjectREFR::LookupByID<RE::Actor>(data.first);
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

	inline std::vector<float> GetActorFloatSettings(RE::StaticFunctionTag*, RE::Actor* a_actor)
	{
		if (!a_actor)
			return {};

		std::vector<float> vec;
		for (auto& entry : Actor::m_ActorData) {
			auto actor = RE::TESObjectREFR::LookupByID<RE::Actor>(entry.first);
			if (actor == a_actor) {
				auto& data = entry.second;
				if (data.IsCustom()) {
					vec = data.GetVisual();
				}
				break;
			}
		}

		return vec;
	}

	inline void SetActorFloatSetting(RE::StaticFunctionTag*, RE::Actor* a_actor, std::string a_key, float a_value)
	{
		if (!a_actor)
			return;

		auto actor_id = Util::FormToString(a_actor);
		if (!actor_id)
			return;

		auto set = Settings::GetSingleton();
		auto& actors = set->data.Actors;
		auto it = std::find_if(actors.begin(), actors.end(), [&](const auto& a_obj) {
			return !a_obj.Actor.empty() && (a_obj.Actor == actor_id);
		});

		if (it != actors.end()) {
			json obj = *it;
			obj[a_key] = a_value;
			*it = obj;
			set->Save();
		}
	}

	inline void AddActorSettings(RE::StaticFunctionTag*, RE::Actor* a_actor)
	{
		if (!a_actor)
			return;

		auto actor_id = Util::FormToString(a_actor);
		if (!actor_id)
			return;

		auto set = Settings::GetSingleton();
		auto& data = set->data;

		Actor_t obj;
		obj.Actor = *actor_id;
		obj.Wetness = data.Wetness.Min;
		obj.Glossiness = data.Glossiness.Min;
		obj.Specular = data.Specular.Min;

		auto& actors = data.Actors;
		if (a_actor->IsPlayerRef())
			actors.insert(actors.begin(), obj);
		else
			actors.push_back(obj);

		set->Save();
	}

	inline void RemoveActorSettings(RE::StaticFunctionTag*, RE::Actor* a_actor)
	{
		if (!a_actor)
			return;

		auto actor_id = Util::FormToString(a_actor);
		if (!actor_id)
			return;

		auto set = ::Settings::GetSingleton();
		auto& actors = set->data.Actors;
		auto it = std::find_if(actors.begin(), actors.end(), [&](const auto& a_obj) {
			return !a_obj.Actor.empty() && (a_obj.Actor == actor_id);
		});

		if (it != actors.end()) {
			actors.erase(it);
			set->Save();
		}
	}

	inline void Bind(VM& a_vm)
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