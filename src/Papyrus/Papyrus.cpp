#include "Papyrus/Papyrus.h"
#include "Papyrus/Script.h"

#include "Game/MetaData.h"
#include "Game/Meta.h"

#include "Store/PresetStore.h"
#include "Store/SettingStore.h"

#include "Util.h"

#define BIND(a_method, ...) a_vm->RegisterFunction(#a_method##sv, CLASS, a_method __VA_OPT__(, ) __VA_ARGS__)

namespace Papyrus
{
	void Update(RE::StaticFunctionTag*)
	{
		auto& meta = Meta::GetSingleton();
		meta.Update();
	}

	void UpdateActor(
		VM* a_vm,
		StackID a_stackID,
		RE::StaticFunctionTag*,
		RE::Actor* a_actor)
	{
		if (!a_actor) {
			a_vm->TraceStack("Actor is None", a_stackID);
			return;
		}

		auto& meta = Meta::GetSingleton();
		meta.UpdateActor(a_actor);
	}

	void GetActorList(RE::StaticFunctionTag*)
	{
		auto& meta = Meta::GetSingleton();
		auto obj = Script::GetObject(meta.m_QuestMCM, CLASS_MCM);
		if (!obj)
			return;

		std::vector<RE::Actor*> actors;
		std::vector<std::string> names;

		auto player = RE::PlayerCharacter::GetSingleton();
		if (player) {
			actors.push_back(player);
			names.push_back(player->GetName());
		}

		auto pl = RE::ProcessLists::GetSingleton();
		for (auto& handle : pl->highActorHandles) {
			if (actors.size() >= 128)
				break;

			auto actor = handle.get().get();
			if (actor) {
				if (!meta.IsValidActor(actor))
					continue;

				if (!meta.IsValidEffect(actor))
					continue;

				actors.push_back(actor);
				names.push_back(actor->GetName());
			}
		}

		for (auto& actor : actors) {
			logger::info("Actor: {}", actor->GetName());
		}

		Script::SetProperty(obj, "ActorList", actors);
		Script::SetProperty(obj, "ActorNameList", names);
	}

	bool GetActorMeta(
		VM* a_vm,
		StackID a_stackID,
		RE::StaticFunctionTag*,
		RE::Actor* a_actor)
	{
		if (!a_actor) {
			a_vm->TraceStack("Actor is None", a_stackID);
			return false;
		}

		auto& meta = Meta::GetSingleton();
		auto obj = Script::GetObject(meta.m_QuestMCM, CLASS_MCM);
		if (!obj) {
			logger::info("MCM Script Object not found!");
			return false;
		}

		auto metaData = meta.GetMetaData(a_actor);
		if (metaData) {
			Script::SetProperty(obj, "TargetWetness", metaData->m_Wetness);
			Script::SetProperty(obj, "TargetGlossiness", metaData->m_Glossiness);
			Script::SetProperty(obj, "TargetSpecular", metaData->m_Specular);
			return true;
		}

		return false;
	}

	void AddActorPreset(
		VM* a_vm,
		StackID a_stackID,
		RE::StaticFunctionTag*,
		RE::Actor* a_actor)
	{
		if (!a_actor) {
			a_vm->TraceStack("Actor is None", a_stackID);
			return;
		}

		auto& prs = PresetStore::GetSingleton();
		prs.Add(a_actor);
	}

	void RemoveActorPreset(
		VM* a_vm,
		StackID a_stackID,
		RE::StaticFunctionTag*,
		RE::Actor* a_actor)
	{
		if (!a_actor) {
			a_vm->TraceStack("Actor is None", a_stackID);
			return;
		}

		auto& prs = PresetStore::GetSingleton();
		prs.Remove(a_actor);
	}

	bool GetActorPreset(
		VM* a_vm,
		StackID a_stackID,
		RE::StaticFunctionTag*,
		RE::Actor* a_actor)
	{
		if (!a_actor) {
			a_vm->TraceStack("Actor is None", a_stackID);
			return false;
		}

		auto& meta = Meta::GetSingleton();
		auto obj = Script::GetObject(meta.m_QuestMCM, CLASS_MCM);
		if (!obj)
			return false;

		auto& presetStore = PresetStore::GetSingleton();
		auto preset = presetStore.Find(a_actor->GetFormID());
		if (preset) {
			Script::SetProperty(obj, "TargetWetnessMin", preset->WetnessMin);
			Script::SetProperty(obj, "TargetWetnessMax", preset->WetnessMax);
			Script::SetProperty(obj, "TargetWetnessForced", preset->WetnessForced);
			Script::SetProperty(obj, "TargetGlossinessMin", preset->GlossinessMin);
			Script::SetProperty(obj, "TargetGlossinessMax", preset->GlossinessMax);
			Script::SetProperty(obj, "TargetGlossinessForced", preset->GlossinessForced);
			Script::SetProperty(obj, "TargetSpecularMin", preset->SpecularMin);
			Script::SetProperty(obj, "TargetSpecularMax", preset->SpecularMax);
			Script::SetProperty(obj, "TargetSpecularForced", preset->SpecularForced);
			return true;
		}

		return false;
	}

	void SetActorPreset(
		VM* a_vm,
		StackID a_stackID,
		RE::StaticFunctionTag*,
		RE::Actor* a_actor)
	{
		if (!a_actor) {
			a_vm->TraceStack("Actor is None", a_stackID);
			return;
		}

		auto& meta = Meta::GetSingleton();
		auto obj = Script::GetObject(meta.m_QuestMCM, CLASS_MCM);
		if (!obj)
			return;

		auto& presetStore = PresetStore::GetSingleton();
		auto preset = presetStore.Find(a_actor->GetFormID());
		if (preset) {
			preset->WetnessMin = Script::GetProperty<float>(obj, "TargetWetnessMin");
			preset->WetnessMax = Script::GetProperty<float>(obj, "TargetWetnessMax");
			preset->WetnessForced = Script::GetProperty<bool>(obj, "TargetWetnessForced");
			preset->GlossinessMin = Script::GetProperty<float>(obj, "TargetGlossinessMin");
			preset->GlossinessMax = Script::GetProperty<float>(obj, "TargetGlossinessMax");
			preset->GlossinessForced = Script::GetProperty<bool>(obj, "TargetGlossinessForced");
			preset->SpecularMin = Script::GetProperty<float>(obj, "TargetSpecularMin");
			preset->SpecularMax = Script::GetProperty<float>(obj, "TargetSpecularMax");
			preset->SpecularForced = Script::GetProperty<bool>(obj, "TargetSpecularForced");
		}

		presetStore.Save();
	}

	void LoadSettings(RE::StaticFunctionTag*)
	{
		auto& setting = SettingStore::GetSingleton();
		setting.LoadSettings();
	}

	void PrintConsole(RE::StaticFunctionTag*, std::string a_msg)
	{
		const auto log = RE::ConsoleLog::GetSingleton();
		if (log) {
			log->Print(a_msg.c_str());
		}
	}

	auto FormatFloat(
		RE::StaticFunctionTag*,
		std::string a_fmt,
		float a_val) -> std::string
	{
		return fmt::format(a_fmt, a_val);
	}

	bool SKSEAPI Bind(VM* a_vm)
	{
		if (!a_vm) {
			logger::info("No Virtual Machine!");
			return false;
		}

		// Actor
		BIND(Update);
		BIND(UpdateActor);
		BIND(GetActorList);
		BIND(GetActorMeta);
		BIND(AddActorPreset);
		BIND(RemoveActorPreset);
		BIND(GetActorPreset);
		BIND(SetActorPreset);

		// Settings
		BIND(LoadSettings);

		// Util
		BIND(PrintConsole);
		BIND(FormatFloat);

		return true;
	}
}
