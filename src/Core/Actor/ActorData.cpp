#include "Game/MetaData.h"

#include "Serial/Serial.h"
#include "Store/PresetStore.h"
#include "Store/SettingStore.h"
#include "Util.h"

bool MetaData::Save(SKSE::SerializationInterface* a_intfc)
{
	Serial::Write(a_intfc, &m_Wetness);
	Serial::Write(a_intfc, &m_WetnessRate);
	Serial::Write(a_intfc, &m_Glossiness);
	Serial::Write(a_intfc, &m_Specular);
	return true;
}

void MetaData::Load(SKSE::SerializationInterface* a_intfc, uint32_t& a_length)
{
	m_Wetness = Serial::Read<float>(a_intfc, a_length);
	m_WetnessRate = Serial::Read<float>(a_intfc, a_length);
	m_Glossiness = Serial::Read<float>(a_intfc, a_length);
	m_Specular = Serial::Read<float>(a_intfc, a_length);
}

void MetaData::ApplyProperties(RE::NiAVObject* a_obj, bool a_glossiness, bool a_specular)
{
	using State = RE::BSGeometry::States;
	using Feature = RE::BSShaderMaterial::Feature;
	using Visit = RE::BSVisit::BSVisitControl;
	RE::BSVisit::TraverseScenegraphGeometries(a_obj, [&](RE::BSGeometry* a_geometry) {
		auto effect = a_geometry->properties[State::kEffect].get();
		if (!effect)
			return Visit::kContinue;

		auto lightingShader = netimmerse_cast<RE::BSLightingShaderProperty*>(effect);
		if (!lightingShader)
			return Visit::kContinue;

		auto material = static_cast<RE::BSLightingShaderMaterialBase*>(lightingShader->material);
		if (!material)
			return Visit::kContinue;

		auto type = material->GetFeature();
		if (type == Feature::kFaceGenRGBTint || type == Feature::kFaceGen) {
			if (a_glossiness)
				material->specularPower = m_Glossiness;
			if (a_specular)
				material->specularColorScale = m_Specular;
		}

		return Visit::kContinue;
	});
}

void MetaData::ApplyHead(RE::Actor* a_actor, bool a_glossiness, bool a_specular)
{
	auto obj = a_actor->GetHeadPartObject(RE::BGSHeadPart::HeadPartType::kFace);
	if (obj) {
		ApplyProperties(obj, a_glossiness, a_specular);
	}
}

void MetaData::ApplySlot(RE::Actor* a_actor, BipedSlot a_slot, bool a_glossiness, bool a_specular)
{
	auto skin = a_actor->GetSkin(a_slot);
	if (!skin)
		return;

	auto addon = Util::GetArmorAddonByMask(skin, a_actor->race, a_slot);
	if (!addon)
		return;

	bool isPlayer = a_actor->IsPlayerRef();
	Util::VisitArmorAddon(a_actor, skin, addon, [&](bool a_firstPerson, RE::NiAVObject& a_obj) {
		if (!a_firstPerson || (a_firstPerson && isPlayer)) {
			ApplyProperties(&a_obj, a_glossiness, a_specular);
		}
	});
}

void MetaData::Apply(RE::Actor* a_actor)
{
	auto& setting = SettingStore::GetSingleton().Get();
	auto kUnnamed52 = static_cast<BipedSlot>(1 << 22); // SOS?
	ApplyHead(a_actor, setting.GlossinessHead, setting.SpecularHead);
	ApplySlot(a_actor, BipedSlot::kBody, setting.GlossinessBody, setting.SpecularBody);
	ApplySlot(a_actor, BipedSlot::kHands, setting.GlossinessHands, setting.SpecularHands);
	ApplySlot(a_actor, BipedSlot::kFeet, setting.GlossinessFeet, setting.SpecularFeet);
	ApplySlot(a_actor, kUnnamed52, setting.GlossinessOther, setting.SpecularOther);
}

bool MetaData::UpdatePreset(RE::Actor* a_actor)
{
	auto& setting = SettingStore::GetSingleton().Get();
	auto& presets = PresetStore::GetSingleton();
	auto preset = presets.Find(a_actor->GetFormID());
	if (preset) {
		m_WetnessForced = preset->WetnessForced;
		m_WetnessMin = m_WetnessForced ? preset->WetnessMin : setting.WetnessMin;
		m_WetnessMax = m_WetnessForced ? preset->WetnessMax : setting.WetnessMax;
		m_GlossinessForced = preset->GlossinessForced;
		m_GlossinessMin = m_GlossinessForced ? preset->GlossinessMin : setting.GlossinessMin;
		m_GlossinessMax = m_GlossinessForced ? preset->GlossinessMax : setting.GlossinessMax;
		m_SpecularForced = preset->SpecularForced;
		m_SpecularMin = m_SpecularForced ? preset->SpecularMin : setting.SpecularMin;
		m_SpecularMax = m_SpecularForced ? preset->SpecularMax : setting.SpecularMax;
		return true;
	}

	return false;
}

void MetaData::Update(bool a_reset)
{
	auto actor = RE::TESObjectREFR::LookupByID<RE::Actor>(m_FormID);
	if (!actor)
		return;

	if (a_reset || !UpdatePreset(actor)) {
		auto& setting = SettingStore::GetSingleton().Get();
		m_WetnessMin = setting.WetnessMin;
		m_WetnessMax = setting.WetnessMax;
		m_WetnessForced = false;
		m_GlossinessMin = setting.GlossinessMin;
		m_GlossinessMax = setting.GlossinessMax;
		m_GlossinessForced = false;
		m_SpecularMin = setting.SpecularMin;
		m_SpecularMax = setting.SpecularMax;
		m_SpecularForced = false;
	}

	// TODO: Calculate this dynamically
	m_Wetness = m_WetnessMin;
	m_Glossiness = m_GlossinessMin;
	m_Specular = m_SpecularMin;

	Apply(actor);
}
