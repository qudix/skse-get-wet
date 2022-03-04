#pragma once

class MetaData;

class Meta final
{
public:
	[[nodiscard]] static Meta& GetSingleton() noexcept;

	bool Save(SKSE::SerializationInterface* a_intfc, uint32_t a_type, uint32_t a_version);
	bool Save(SKSE::SerializationInterface* a_intfc);
	bool Load(SKSE::SerializationInterface* a_intfc, uint32_t& a_length);
	void Revert(SKSE::SerializationInterface*);

	void Setup();

	MetaData* GetMetaData(RE::FormID a_formID, bool a_create = false);
	MetaData* GetMetaData(RE::Actor* a_actor, bool a_create = false);

	bool IsValidActor(RE::Actor* a_actor);
	bool IsValidEffect(RE::Actor* a_actor);

	bool CleanActor(RE::Actor* a_actor);
	void UpdateActor(RE::Actor* a_actor, bool a_force = false);
	void Update();

public:
	// Quest
	RE::TESQuest* m_QuestMCM{ nullptr };
	RE::TESGlobal* m_ModEnabled { nullptr };

	// Actor
	RE::FormID m_LastFormID{ 0 };
	MetaData* m_LastMetaData{ nullptr };
	std::unordered_map<uint32_t, MetaData> m_MetaData;

private:
	Meta() = default;
	Meta(const Meta&) = delete;
	Meta(Meta&&) = delete;
	~Meta() = default;

	Meta& operator=(const Meta&) = delete;
	Meta& operator=(Meta&&) = delete;
};
