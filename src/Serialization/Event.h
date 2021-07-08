#pragma once

namespace Event
{
	using EventResult = RE::BSEventNotifyControl;

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

	class ObjectLoadedEventHandler : public RE::BSTEventSink<RE::TESObjectLoadedEvent>
	{
	public:
		static ObjectLoadedEventHandler* GetSingleton()
		{
			static ObjectLoadedEventHandler singleton;
			return &singleton;
		}

		virtual EventResult ProcessEvent(const RE::TESObjectLoadedEvent* a_event, RE::BSTEventSource<RE::TESObjectLoadedEvent>*)
		{
			if (!a_event || !a_event->loaded)
				return EventResult::kContinue;

			Update(a_event->formID);

			return EventResult::kContinue;
		}

	private:
		ObjectLoadedEventHandler() = default;
		ObjectLoadedEventHandler(const ObjectLoadedEventHandler&) = delete;
		ObjectLoadedEventHandler(ObjectLoadedEventHandler&&) = delete;
		virtual ~ObjectLoadedEventHandler() = default;

		ObjectLoadedEventHandler& operator=(const ObjectLoadedEventHandler&) = delete;
		ObjectLoadedEventHandler& operator=(ObjectLoadedEventHandler&&) = delete;
	};

	class EquipEventHandler : public RE::BSTEventSink<RE::TESEquipEvent>
	{
	public:
		static EquipEventHandler* GetSingleton()
		{
			static EquipEventHandler singleton;
			return &singleton;
		}

		virtual EventResult ProcessEvent(const RE::TESEquipEvent* a_event, RE::BSTEventSource<RE::TESEquipEvent>*)
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

	private:
		EquipEventHandler() = default;
		EquipEventHandler(const EquipEventHandler&) = delete;
		EquipEventHandler(EquipEventHandler&&) = delete;
		virtual ~EquipEventHandler() = default;

		EquipEventHandler& operator=(const EquipEventHandler&) = delete;
		EquipEventHandler& operator=(EquipEventHandler&&) = delete;
	};

	class MenuOpenCloseEventHandler : public RE::BSTEventSink<RE::MenuOpenCloseEvent>
	{
	public:
		static MenuOpenCloseEventHandler* GetSingleton()
		{
			static MenuOpenCloseEventHandler singleton;
			return std::addressof(singleton);
		}

		virtual EventResult ProcessEvent(const RE::MenuOpenCloseEvent* a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>*)
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

	private:
		MenuOpenCloseEventHandler() = default;
		MenuOpenCloseEventHandler(const MenuOpenCloseEventHandler&) = delete;
		MenuOpenCloseEventHandler(MenuOpenCloseEventHandler&&) = delete;
		inline ~MenuOpenCloseEventHandler() = default;

		MenuOpenCloseEventHandler& operator=(const MenuOpenCloseEventHandler&) = delete;
		MenuOpenCloseEventHandler& operator=(MenuOpenCloseEventHandler&&) = delete;
	};
	/*
	class AnimationEventHandler : public RE::BSTEventSink<RE::BGSFootstepEvent>
	{
	public:
		static AnimationEventHandler* GetSingleton()
		{
			static AnimationEventHandler singleton;
			return std::addressof(singleton);
		}

		virtual EventResult ProcessEvent(const RE::BGSFootstepEvent* a_event, RE::BSTEventSource<RE::BGSFootstepEvent>*)
		{
			if (!a_event)
				return EventResult::kContinue;

			auto actor = a_event->actor.get().get();
			if (actor) {
				logger::info("Footstep: {}", actor->GetName());
			}

			return EventResult::kContinue;
		}
	};*/

	void Register()
	{
		auto events = RE::ScriptEventSourceHolder::GetSingleton();
		if (events) {
			events->AddEventSink(ObjectLoadedEventHandler::GetSingleton());
			events->AddEventSink(EquipEventHandler::GetSingleton());
			//events->AddEventSink(AnimationEventHandler::GetSingleton());
		}

		auto ui = RE::UI::GetSingleton();
		if (ui) {
			ui->AddEventSink(MenuOpenCloseEventHandler::GetSingleton());
		}
	}
}

