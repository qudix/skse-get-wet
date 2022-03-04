#pragma once

namespace Event
{
	using EventResult = RE::BSEventNotifyControl;

	class ScriptEventHandler final:
		public RE::BSTEventSink<RE::TESObjectLoadedEvent>,
		public RE::BSTEventSink<RE::TESEquipEvent>
	{
	public:
		static ScriptEventHandler* GetSingleton()
		{
			static ScriptEventHandler singleton;
			return &singleton;
		}

		static void Register()
		{
			auto scripts = RE::ScriptEventSourceHolder::GetSingleton();
			if (scripts) {
				scripts->AddEventSink<RE::TESObjectLoadedEvent>(GetSingleton());
				scripts->AddEventSink<RE::TESEquipEvent>(GetSingleton());
			}
		}

		EventResult ProcessEvent(const RE::TESObjectLoadedEvent* a_event, RE::BSTEventSource<RE::TESObjectLoadedEvent>*) override;
		EventResult ProcessEvent(const RE::TESEquipEvent* a_event, RE::BSTEventSource<RE::TESEquipEvent>*) override;

	private:
		ScriptEventHandler() = default;
		ScriptEventHandler(const ScriptEventHandler&) = delete;
		ScriptEventHandler(ScriptEventHandler&&) = delete;
		~ScriptEventHandler() = default;

		ScriptEventHandler& operator=(const ScriptEventHandler&) = delete;
		ScriptEventHandler& operator=(ScriptEventHandler&&) = delete;
	};

	class UIEventHandler final :
		public RE::BSTEventSink<RE::MenuOpenCloseEvent>
	{
	public:
		static UIEventHandler* GetSingleton()
		{
			static UIEventHandler singleton;
			return std::addressof(singleton);
		}

		static void Register()
		{
			auto ui = RE::UI::GetSingleton();
			if (ui) {
				ui->AddEventSink<RE::MenuOpenCloseEvent>(GetSingleton());
			}
		}

		EventResult ProcessEvent(const RE::MenuOpenCloseEvent* a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>*) override;

	private:
		UIEventHandler() = default;
		UIEventHandler(const UIEventHandler&) = delete;
		UIEventHandler(UIEventHandler&&) = delete;
		~UIEventHandler() = default;

		UIEventHandler& operator=(const UIEventHandler&) = delete;
		UIEventHandler& operator=(UIEventHandler&&) = delete;
	};
}
