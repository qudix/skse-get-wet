#include "Game/Meta.h"
#include "Game/MetaData.h"
#include "Game/Config.h"

Meta& Meta::GetSingleton() noexcept
{
	static Meta singleton;
	return singleton;
}

bool Meta::Save(SKSE::SerializationInterface* a_intfc, uint32_t a_type, uint32_t a_version)
{
	if (!a_intfc->OpenRecord(a_type, a_version)) {
		logger::info("Failed to open serializtion record!"sv);
		return false;
	}

	return Save(a_intfc);
}

bool Meta::Save(SKSE::SerializationInterface* a_intfc)
{
	auto count = static_cast<uint32_t>(m_MetaData.size());
	stl::write(a_intfc, &count);
	for (auto& [formID, data] : m_MetaData) {
		stl::write(a_intfc, &formID);
		data.Save(a_intfc);
	}

	return true;
}

bool Meta::Load(SKSE::SerializationInterface* a_intfc, uint32_t& a_length)
{
	auto count = stl::read<uint32_t>(a_intfc, a_length);
	for (uint32_t i = 0; i < count; ++i) {
		auto formID = stl::read<uint32_t>(a_intfc, a_length);
		uint32_t newFormID;
		if (!a_intfc->ResolveFormID(formID, newFormID))
			continue;

		MetaData data;
		data.Load(a_intfc, a_length);
		data.SetFormID(newFormID);
		m_MetaData[newFormID] = std::move(data);
	}

	return true;
}

void Meta::Revert(SKSE::SerializationInterface*)
{
	m_LastFormID = 0;
	m_LastMetaData = nullptr;
	m_MetaData.clear();
}

void Meta::Setup()
{
	auto handler = RE::TESDataHandler::GetSingleton();
	auto mod = "qdx-get-wet.esp"sv;
	m_QuestMain = handler->LookupForm(RE::FormID(0x800), mod)->As<RE::TESQuest>();
	m_QuestConfig = handler->LookupForm(RE::FormID(0x801), mod)->As<RE::TESQuest>();
	m_QuestMCM = handler->LookupForm(RE::FormID(0x802), mod)->As<RE::TESQuest>();
}

MetaData* Meta::GetMetaData(RE::FormID a_formID, bool a_create)
{
	if (m_LastFormID == a_formID && m_LastMetaData)
		return m_LastMetaData;

	auto it = m_MetaData.find(a_formID);
	if (it != m_MetaData.end()) {
		m_LastFormID = a_formID;
		m_LastMetaData = &it->second;
		return &it->second;
	}

	if (a_create) {
		auto& data = m_MetaData[a_formID];
		data.SetFormID(a_formID);
		m_LastFormID = a_formID;
		m_LastMetaData = &data;
		return &data;
	}

	return nullptr;
}

MetaData* Meta::GetMetaData(RE::Actor* a_actor, bool a_create)
{
	return a_actor ? GetMetaData(a_actor->GetFormID(), a_create) : nullptr;
}

bool IsValidActor(RE::Actor* a_actor)
{
	if (!a_actor->Is3DLoaded() || a_actor->IsDead())
		return false;

	if (!a_actor->formType.all(RE::FormType::ActorCharacter))
		return false;

	auto race = a_actor->GetRace();
	if (race->data.flags.all(RE::RACE_DATA::Flag::kChild))
		return false;

	auto dobj = RE::BGSDefaultObjectManager::GetSingleton();
	auto keywordNPC = dobj->GetObject<RE::BGSKeyword>(RE::DEFAULT_OBJECT::kKeywordNPC);
	if (!a_actor->HasKeyword(keywordNPC))
		return false;

	return true;
}

bool IsValidEffect(RE::Actor* a_actor)
{
	auto& data = Config::GetSingleton().data;
	auto female = a_actor->GetActorBase()->IsFemale();
	if (female && !data.ApplyFemale)
		return false;

	if (!female && !data.ApplyMale)
		return false;

	bool player = a_actor->IsPlayerRef();
	if (player && !data.ApplyPlayer)
		return false;

	if (!player && !data.ApplyNPC)
		return false;

	auto dobj = RE::BGSDefaultObjectManager::GetSingleton();
	auto keywordBeast = dobj->GetObject<RE::BGSKeyword>(RE::DEFAULT_OBJECT::kKeywordBeastRace);
	bool beast = a_actor->HasKeyword(keywordBeast);
	if (beast && !data.ApplyBeast)
		return false;

	return true;
}

bool Meta::CleanActor(RE::Actor* a_actor)
{
	auto valid = IsValidEffect(a_actor);
	if (!valid) {
		auto it = m_MetaData.find(a_actor->GetFormID());
		if (it != m_MetaData.end()) {
			it->second.Update(true);
			m_MetaData.erase(it);
			return false;
		}
	}

	return valid;
}

void Meta::UpdateActor(RE::Actor* a_actor, bool a_force)
{
	if (!m_QuestMain || !m_QuestMain->IsEnabled())
		return;

	bool valid = IsValidActor(a_actor);
	if (!valid)
		return;

	bool clean = CleanActor(a_actor);
	if (!clean)
		return;

	auto& data = Config::GetSingleton().data;
	if (a_force || data.ApplyGlobal) {
		auto metaData = GetMetaData(a_actor, true);
		if (metaData) {
			metaData->Update();
		}
	}
}

void Meta::Update()
{
	if (!m_QuestMain || !m_QuestMain->IsEnabled())
		return;

	auto player = RE::PlayerCharacter::GetSingleton();
	if (player)
		UpdateActor(player);

	auto pl = RE::ProcessLists::GetSingleton();
	for (auto& handle : pl->highActorHandles) {
		auto actor = handle.get().get();
		if (actor)
			UpdateActor(actor);
	}
}
