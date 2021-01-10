#pragma once

#include "Actor.h"

namespace PapyrusActor
{
	using VM = RE::BSScript::IVirtualMachine;
	using StackID = RE::VMStackID;
	using Severity = RE::BSScript::ErrorLogger::Severity;

	void Update(RE::StaticFunctionTag*, bool a_running)
	{
		Actor::Update(a_running);
	}
	
	std::vector<RE::Actor*> GetActorList(RE::StaticFunctionTag*)
	{
		std::vector<RE::Actor*> a_vec;

		for (auto& props : Actor::m_actorProps) {
			if (props.m_Actor)
				a_vec.push_back(props.m_Actor);
		}
		return a_vec;
	}

	std::vector<RE::BSFixedString> GetActorNameList(RE::StaticFunctionTag*)
	{
		std::vector<RE::BSFixedString> n_vec;

		for (auto& props : Actor::m_actorProps) {
			if (props.m_Actor)
				n_vec.push_back(props.m_Actor->GetName());
		}
		return n_vec;
	}

	std::vector<float> GetActorFloatSettings(RE::StaticFunctionTag*, RE::Actor* a_actor)
	{
		std::vector<float> vec;

		for (auto& props : Actor::m_actorProps) {
			if (props.m_Actor == a_actor) {
				if (props.m_HasSettings) {
					vec.push_back(props.m_GlossinessMin);
					vec.push_back(props.m_GlossinessMax);
					vec.push_back(props.m_SpecularMin);
					vec.push_back(props.m_SpecularMax);
				}
				break;
			}
		}

		return vec;
	}

	void SetActorFloatSetting(RE::StaticFunctionTag*, RE::Actor* a_actor, std::string a_key, float a_value)
	{
		Actor::SetActorSetting(a_actor, a_key, a_value);
	}

	void AddActorSettings(RE::StaticFunctionTag*, RE::Actor* a_actor)
	{
		Actor::AddActorSettings(a_actor);
	}

	void RemoveActorSettings(RE::StaticFunctionTag*, RE::Actor* a_actor)
	{
		Actor::RemoveActorSettings(a_actor);
	}

	static constexpr char CLASS_NAME[] = "qdx_gw";

	bool Register(VM* a_vm)
	{
		if (!a_vm) {
			logger::critical("Papyrus Actor: Couldn't get VM");
			return false;
		}

		a_vm->RegisterFunction("Update", CLASS_NAME, Update);
		a_vm->RegisterFunction("GetActorList", CLASS_NAME, GetActorList);
		a_vm->RegisterFunction("GetActorNameList", CLASS_NAME, GetActorNameList);
		a_vm->RegisterFunction("GetActorFloatSettings", CLASS_NAME, GetActorFloatSettings);
		a_vm->RegisterFunction("SetActorFloatSetting", CLASS_NAME, SetActorFloatSetting);
		a_vm->RegisterFunction("AddActorSettings", CLASS_NAME, AddActorSettings);
		a_vm->RegisterFunction("RemoveActorSettings", CLASS_NAME, RemoveActorSettings);
		
		return true;
	}
}