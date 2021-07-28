#pragma once

namespace PapyrusActor
{
	using VM = RE::BSScript::IVirtualMachine;

	constexpr const char* CLASS_MCM = "qdx_gw_QuestMCM";

	void Update(RE::StaticFunctionTag*)
	{
		auto& meta = Meta::GetSingleton();
		meta.Update();
	}

	void UpdateActor(RE::StaticFunctionTag*, RE::Actor* a_actor)
	{
		if (!a_actor)
			return;

		auto& meta = Meta::GetSingleton();
		meta.UpdateActor(a_actor);
	}

	void GetActorList(RE::StaticFunctionTag*)
	{
		auto& meta = Meta::GetSingleton();
		auto obj = stl::get(meta.m_QuestMCM, CLASS_MCM);
		if (!obj)
			return;

		std::vector<RE::Actor*> actors;
		std::vector<std::string> names;

		for (auto& [formID, data] : meta.m_MetaData) {
			auto actor = RE::TESObjectREFR::LookupByID<RE::Actor>(formID);
			if (actor) {
				auto name = actor->GetName();
				if (actor->IsPlayerRef()) {
					actors.insert(actors.begin(), actor);
					names.insert(names.begin(), name);
				} else {
					actors.push_back(actor);
					names.push_back(name);
				}
			}
		}

		stl::set(obj, "ActorList", actors);
		stl::set(obj, "ActorNameList", names);
	}

	void GetActorMeta(RE::StaticFunctionTag*, RE::Actor* a_actor)
	{
		if (!a_actor)
			return;

		auto& meta = Meta::GetSingleton();
		auto obj = stl::get(meta.m_QuestMCM, CLASS_MCM);
		if (!obj)
			return;

		auto metaData = meta.GetMetaData(a_actor);
		if (metaData) {
			stl::set<float>(obj, "ActorWetness", metaData->m_Wetness);
			stl::set<float>(obj, "ActorGlossiness", metaData->m_Glossiness);
			stl::set<float>(obj, "ActorSpecular", metaData->m_Specular);
		}
	}

	void AddActorPreset(RE::StaticFunctionTag*, RE::Actor* a_actor)
	{
		if (!a_actor)
			return;

		auto& prs = Presets::GetSingleton();
		prs.AddPreset(a_actor);
	}

	void RemoveActorPreset(RE::StaticFunctionTag*, RE::Actor* a_actor)
	{
		if (!a_actor)
			return;

		auto& prs = Presets::GetSingleton();
		prs.RemovePreset(a_actor);
	}

	bool GetActorPreset(RE::StaticFunctionTag*, RE::Actor* a_actor)
	{
		if (!a_actor)
			return false;

		auto& meta = Meta::GetSingleton();
		auto obj = stl::get(meta.m_QuestMCM, CLASS_MCM);
		if (!obj)
			return false;

		auto& prs = Presets::GetSingleton();
		auto it = prs.presets.find(a_actor->GetFormID());
		if (it != prs.presets.end()) {
			auto& preset = it->second;
			stl::set<float>(obj, "ActorWetnessMin", preset.WetnessMin);
			stl::set<float>(obj, "ActorWetnessMax", preset.WetnessMax);
			stl::set<float>(obj, "ActorGlossinessMin", preset.GlossinessMin);
			stl::set<float>(obj, "ActorGlossinessMax", preset.GlossinessMax);
			stl::set<float>(obj, "ActorSpecularMin", preset.SpecularMin);
			stl::set<float>(obj, "ActorSpecularMax", preset.SpecularMax);
			stl::set<bool>(obj, "ActorForceWetness", preset.ForceWetness);
			stl::set<bool>(obj, "ActorForceGlossiness", preset.ForceGlossiness);
			stl::set<bool>(obj, "ActorForceSpecular", preset.ForceSpecular);
			return true;
		}

		return false;
	}

	void SetActorPreset(RE::StaticFunctionTag*, RE::Actor* a_actor)
	{
		if (!a_actor)
			return;

		auto& meta = Meta::GetSingleton();
		auto obj = stl::get(meta.m_QuestMCM, CLASS_MCM);
		if (!obj)
			return;

		auto& prs = Presets::GetSingleton();
		auto it = prs.presets.find(a_actor->GetFormID());
		if (it != prs.presets.end()) {
			auto& preset = it->second;
			preset.WetnessMin = stl::get<float>(obj, "ActorWetnessMin");
			preset.WetnessMax = stl::get<float>(obj, "ActorWetnessMax");
			preset.GlossinessMin = stl::get<float>(obj, "ActorGlossinessMin");
			preset.GlossinessMax = stl::get<float>(obj, "ActorGlossinessMax");
			preset.SpecularMin = stl::get<float>(obj, "ActorSpecularMin");
			preset.SpecularMax = stl::get<float>(obj, "ActorSpecularMax");
			preset.ForceWetness = stl::get<bool>(obj, "ActorForceWetness");
			preset.ForceGlossiness = stl::get<bool>(obj, "ActorForceGlossiness");
			preset.ForceSpecular = stl::get<bool>(obj, "ActorForceSpecular");
		}

		// TODO: Allow user to decide to save?
		prs.Save();
	}

	bool Bind(VM* a_vm)
	{
		const auto obj = "qdx_gw"sv;

		BIND(Update);
		BIND(UpdateActor);
		BIND(GetActorList);
		BIND(GetActorMeta);
		BIND(AddActorPreset);
		BIND(RemoveActorPreset);
		BIND(GetActorPreset);
		BIND(SetActorPreset);

		return true;
	}
}
