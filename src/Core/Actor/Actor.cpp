#include "Game/Meta.h"
#include "Game/MetaData.h"
#include "Serial/Serial.h"
#include "Store/SettingStore.h"

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
	a_intfc->WriteRecordData(count);
	for (auto& [formID, data] : m_MetaData) {
		a_intfc->WriteRecordData(formID);
		data.Save(a_intfc);
	}

	return true;
}

bool Meta::Load(SKSE::SerializationInterface* a_intfc, uint32_t& a_length)
{
	auto count = Serial::Read<uint32_t>(a_intfc, a_length);
	for (uint32_t i = 0; i < count; ++i) {
		auto formID = Serial::Read<uint32_t>(a_intfc, a_length);
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
	auto mod = "GetWet.esp"sv;
	m_QuestMCM = handler->LookupForm(RE::FormID(0x801), mod)->As<RE::TESQuest>();
	m_ModEnabled = handler->LookupForm(RE::FormID(0x810), mod)->As<RE::TESGlobal>();
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

bool Meta::IsValidActor(RE::Actor* a_actor)
{
	if (!a_actor->Is3DLoaded() || a_actor->IsDead())
		return false;

	if (!a_actor->formType.all(RE::FormType::ActorCharacter))
		return false;

	if (a_actor->IsChild())
		return false;

	auto dobj = RE::BGSDefaultObjectManager::GetSingleton();
	auto keywordNPC = dobj->GetObject<RE::BGSKeyword>(RE::DEFAULT_OBJECT::kKeywordNPC);
	if (!a_actor->HasKeyword(keywordNPC))
		return false;

	return true;
}

bool Meta::IsValidEffect(RE::Actor* a_actor)
{
	auto& setting = SettingStore::GetSingleton().Get();
	auto female = a_actor->GetActorBase()->IsFemale();
	if (female && !setting.ApplyFemale)
		return false;

	if (!female && !setting.ApplyMale)
		return false;

	bool player = a_actor->IsPlayerRef();
	if (player && !setting.ApplyPlayer)
		return false;

	if (!player && !setting.ApplyNPC)
		return false;

	auto dobj = RE::BGSDefaultObjectManager::GetSingleton();
	auto keywordBeast = dobj->GetObject<RE::BGSKeyword>(RE::DEFAULT_OBJECT::kKeywordBeastRace);
	bool beast = a_actor->HasKeyword(keywordBeast);
	if (beast && !setting.ApplyBeast)
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
	if (!m_ModEnabled || !m_ModEnabled->value)
		return;

	bool valid = IsValidActor(a_actor);
	if (!valid)
		return;

	bool clean = CleanActor(a_actor);
	if (!clean)
		return;

	auto& setting = SettingStore::GetSingleton().Get();
	if (a_force || setting.ApplyGlobal) {
		auto metaData = GetMetaData(a_actor, true);
		if (metaData) {
			metaData->Update();
		}
	}
}

void Meta::Update()
{
	if (!m_ModEnabled || !m_ModEnabled->value)
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
