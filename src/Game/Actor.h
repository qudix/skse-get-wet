#pragma once

namespace Actor 
{
	// Quest
	RE::TESQuest* m_QuestMain = nullptr;
	RE::TESQuest* m_QuestConfig = nullptr;

	// Actor
	RE::FormID m_LastFormID = 0;
	ActorData* m_LastActorData = nullptr;
	std::unordered_map<uint32_t, ActorData> m_ActorData;

	ActorData& _GetActorData(RE::FormID a_formID)
	{
		if (m_LastFormID == a_formID && m_LastActorData)
			return *m_LastActorData;

		// Will add actor data if it doesn't exist
		auto& result = m_ActorData[a_formID];
		m_LastFormID = a_formID;
		m_LastActorData = &result;
		result.SetFormID(a_formID);
		return result;
	}

	template <class T>
	using optional_ref = std::optional<std::reference_wrapper<T>>;

	optional_ref<ActorData> GetActorData(RE::Actor& a_actor)
	{
		return _GetActorData(a_actor.GetFormID());
	}

	bool IsValidActor(RE::Actor& a_actor)
	{
		if (!a_actor.Is3DLoaded() || a_actor.IsDead())
			return false;

		if (!a_actor.formType.all(RE::FormType::ActorCharacter))
			return false;

		auto race = a_actor.GetRace();
		if (race->data.flags.all(RE::RACE_DATA::Flag::kChild))
			return false;

		auto dobj = RE::BGSDefaultObjectManager::GetSingleton();
		auto keywordNPC = dobj->GetObject<RE::BGSKeyword>(RE::DEFAULT_OBJECT::kKeywordNPC);
		if (!a_actor.HasKeyword(keywordNPC))
			return false;

		return true;
	}

	bool IsValidEffect(RE::Actor& a_actor)
	{
		auto set = Settings::GetSingleton();
		auto& apply = set->data.Apply;

		auto female = a_actor.GetActorBase()->IsFemale();
		if (female && apply.Female)
			return false;

		if (!female && !apply.Male)
			return false;

		bool player = a_actor.IsPlayerRef();
		if (player && !apply.Player)
			return false;

		if (!player && !apply.NPC)
			return false;

		auto dobj = RE::BGSDefaultObjectManager::GetSingleton();
		auto keywordBeast = dobj->GetObject<RE::BGSKeyword>(RE::DEFAULT_OBJECT::kKeywordBeastRace);
		bool beast = a_actor.HasKeyword(keywordBeast);
		if (beast && !apply.Beast)
			return false;

		return true;
	}

	bool CleanActor(RE::Actor& a_actor)
	{
		auto valid = IsValidEffect(a_actor);
		if (!valid) {
			auto formID = a_actor.GetFormID();
			auto data = m_ActorData.find(formID);
			if (data != m_ActorData.end()) {
				data->second.Reset();
				m_ActorData.erase(data);
				return false;
			}
		}

		return valid;
	}

	void UpdateActor(RE::Actor& a_actor, bool a_force = false)
	{
		if (!m_QuestMain || !m_QuestMain->IsRunning())
			return;

		bool valid = IsValidActor(a_actor);
		if (!valid)
			return;

		bool clean = CleanActor(a_actor);
		if (!clean)
			return;

		auto set = Settings::GetSingleton();
		auto global = set->data.Apply.Global;
		if (global || a_force) {
			auto opt = GetActorData(a_actor);
			if (opt) {
				auto& data = opt.value().get();
				data.Update();
			}
		}
	}

	void Update()
	{
		if (!m_QuestMain || !m_QuestMain->IsRunning())
			return;

		auto player = RE::PlayerCharacter::GetSingleton();
		if (player)
			UpdateActor(*player);

		auto pl = RE::ProcessLists::GetSingleton();
		for (auto& handle : pl->highActorHandles) {
			auto actor = handle.get().get();
			if (actor)
				UpdateActor(*actor);
		}
	}

	constexpr uint32_t DataVersion = 1;

	void SerialRevert(SKSE::SerializationInterface*)
	{
		m_LastFormID = 0;
		m_LastActorData = nullptr;
		m_ActorData.clear();

		// Do Locking?
	}

	void SerialLoad(SKSE::SerializationInterface* intfc)
	{
		uint32_t type;
		uint32_t version;
		uint32_t length;
		bool error = false;

		while (!error && intfc->GetNextRecordInfo(type, version, length)) {
			if (type != 'DATA') {
				logger::critical("Unknown type!");
				error = true;
				continue;
			}

			if (version != DataVersion)
				continue;

			auto count = Data::ReadHelper<uint32_t>(intfc, length);
			for (uint32_t i = 0; i < count; ++i) {
				auto formID = Data::ReadHelper<uint32_t>(intfc, length);
				ActorData data(intfc, length);

				uint32_t newFormID;
				if (!intfc->ResolveFormID(formID, newFormID))
					continue;

				data.SetFormID(newFormID);
				m_ActorData[newFormID] = std::move(data);
			}
		}

		if (error)
			logger::critical("Error while loading data");
	}

	void SerialSave(SKSE::SerializationInterface* intfc)
	{
		auto record = intfc->OpenRecord('DATA', DataVersion);
		if (record) {
			auto count = static_cast<uint32_t>(m_ActorData.size());
			Data::Write(intfc, &count);
			for (auto const& entry : m_ActorData) {
				uint32_t formID = entry.first;
				auto const& data = entry.second;
				Data::Write(intfc, &formID);
				data.Serialize(intfc);
			}
		}
	}

	void Setup()
	{
		auto handler = RE::TESDataHandler::GetSingleton();
		m_QuestMain = handler->LookupForm(RE::FormID(0x800), "qdx-get-wet.esp")->As<RE::TESQuest>();
		m_QuestConfig = handler->LookupForm(RE::FormID(0x801), "qdx-get-wet.esp")->As<RE::TESQuest>();
	}

	void RegisterSerial()
	{
		auto serial = SKSE::GetSerializationInterface();
		if (!serial) {
			logger::critical("Could not get serialization interface!");
			return;
		}

		serial->SetUniqueID('QXGW');
		serial->SetRevertCallback(SerialRevert);
		serial->SetSaveCallback(SerialSave);
		serial->SetLoadCallback(SerialLoad);
	}
}
