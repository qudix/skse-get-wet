#pragma once

namespace Papyrus
{
	constexpr auto CLASS = "qdx_gw";
	constexpr auto CLASS_MCM = "qdx_gw_MCM";

	void Update(RE::StaticFunctionTag*);

	void UpdateActor(
		VM* a_vm,
		StackID a_stackID,
		RE::StaticFunctionTag*,
		RE::Actor* a_actor);

	void GetActorList(RE::StaticFunctionTag*);

	bool GetActorMeta(
		VM* a_vm,
		StackID a_stackID,
		RE::StaticFunctionTag*,
		RE::Actor* a_actor);

	void AddActorPreset(
		VM* a_vm,
		StackID a_stackID,
		RE::StaticFunctionTag*,
		RE::Actor* a_actor);

	void RemoveActorPreset(
		VM* a_vm,
		StackID a_stackID,
		RE::StaticFunctionTag*,
		RE::Actor* a_actor);

	bool GetActorPreset(
		VM* a_vm,
		StackID a_stackID,
		RE::StaticFunctionTag*,
		RE::Actor* a_actor);

	void SetActorPreset(
		VM* a_vm,
		StackID a_stackID,
		RE::StaticFunctionTag*,
		RE::Actor* a_actor);

	void LoadSettings(RE::StaticFunctionTag*);

	void PrintConsole(RE::StaticFunctionTag*, std::string a_msg);

	auto FormatFloat(
		RE::StaticFunctionTag*,
		std::string a_fmt,
		float a_val) -> std::string;

	bool SKSEAPI Bind(VM* a_vm);
}
