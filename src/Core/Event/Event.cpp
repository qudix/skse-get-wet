#include "Game/Event.h"
#include "Game/Meta.h"

namespace Event
{
	void Update(RE::FormID a_id)
	{
		auto task = SKSE::GetTaskInterface();
		task->AddTask([a_id] {
			auto actor = RE::TESObjectREFR::LookupByID<RE::Actor>(a_id);
			if (actor) {
				auto& meta = Meta::GetSingleton();
				meta.UpdateActor(actor);
			}
		});
	}

	EventResult ScriptEventHandler::ProcessEvent(const RE::TESObjectLoadedEvent* a_event, RE::BSTEventSource<RE::TESObjectLoadedEvent>*)
	{
		if (!a_event || !a_event->loaded)
			return EventResult::kContinue;

		Update(a_event->formID);

		return EventResult::kContinue;
	}

	EventResult ScriptEventHandler::ProcessEvent(const RE::TESEquipEvent* a_event, RE::BSTEventSource<RE::TESEquipEvent>*)
	{
		auto actor = a_event->actor->As<RE::Actor>();
		if (!a_event || !actor)
			return EventResult::kContinue;

		auto form = RE::TESForm::LookupByID(a_event->baseObject);
		if (!form)
			return EventResult::kContinue;

		if (form->Is(RE::FormType::Armor) || form->Is(RE::FormType::Armature)) {
			Update(actor->GetFormID());
		}

		return EventResult::kContinue;
	}

	EventResult UIEventHandler::ProcessEvent(const RE::MenuOpenCloseEvent* a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>*)
	{
		auto uiStr = RE::InterfaceStrings::GetSingleton();
		if (uiStr) {
			auto& name = a_event->menuName;
			if (name == uiStr->faderMenu) {
				if (!a_event->opening) {
					auto player = RE::PlayerCharacter::GetSingleton();
					if (player) {
						auto& meta = Meta::GetSingleton();
						meta.UpdateActor(player);
					}
				}
			}
		}
		return EventResult::kContinue;
	}
}
