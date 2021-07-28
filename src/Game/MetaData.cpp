#include "MetaData.h"

#include "Game/Config.h"
#include "Game/Presets.h"
#include "Util.h"

bool MetaData::Save(SKSE::SerializationInterface* a_intfc)
{
	stl::write(a_intfc, &m_Wetness);
	stl::write(a_intfc, &m_WetnessRate);
	stl::write(a_intfc, &m_Glossiness);
	stl::write(a_intfc, &m_Specular);
	return true;
}

void MetaData::Load(SKSE::SerializationInterface* a_intfc, uint32_t& a_length)
{
	m_Wetness = stl::read<float>(a_intfc, a_length);
	m_WetnessRate = stl::read<float>(a_intfc, a_length);
	m_Glossiness = stl::read<float>(a_intfc, a_length);
	m_Specular = stl::read<float>(a_intfc, a_length);
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
	auto& data = Config::GetSingleton().data;
	auto kUnnamed52 = static_cast<BipedSlot>(1 << 22); // SOS?
	ApplyHead(a_actor, data.GlossinessHead, data.SpecularHead);
	ApplySlot(a_actor, BipedSlot::kBody, data.SpecularBody, data.SpecularBody);
	ApplySlot(a_actor, BipedSlot::kHands, data.GlossinessHands, data.SpecularHands);
	ApplySlot(a_actor, BipedSlot::kFeet, data.GlossinessFeet, data.SpecularFeet);
	ApplySlot(a_actor, kUnnamed52, data.GlossinessOther, data.SpecularOther);
}

bool MetaData::UpdatePreset(RE::Actor* a_actor)
{
	auto& data = Config::GetSingleton().data;
	auto& presets = Presets::GetSingleton().presets;
	auto it = presets.find(a_actor->GetFormID());
	if (it != presets.end()) {
		auto& preset = it->second;
		m_ForceWetness = preset.ForceWetness;
		m_ForceGlossiness = preset.ForceGlossiness;
		m_ForceSpecular = preset.ForceSpecular;
		m_WetnessMin = m_ForceWetness ? preset.WetnessMin : data.WetnessMin;
		m_WetnessMax = m_ForceWetness ? preset.WetnessMax : data.WetnessMax;
		m_GlossinessMin = m_ForceGlossiness ? preset.GlossinessMin : data.GlossinessMin;
		m_GlossinessMax = m_ForceGlossiness ? preset.GlossinessMax : data.GlossinessMax;
		m_SpecularMin = m_ForceSpecular ? preset.SpecularMin : data.SpecularMin;
		m_SpecularMax = m_ForceSpecular ? preset.SpecularMax : data.SpecularMax;
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
		auto& data = Config::GetSingleton().data;
		m_ForceWetness = false;
		m_ForceGlossiness = false;
		m_ForceSpecular = false;
		m_WetnessMin = data.WetnessMin;
		m_WetnessMax = data.WetnessMax;
		m_GlossinessMin = data.GlossinessMin;
		m_GlossinessMax = data.GlossinessMax;
		m_SpecularMin = data.SpecularMin;
		m_SpecularMax = data.SpecularMax;
	}

	// TODO: Calculate this dynamically
	m_Wetness = m_WetnessMin;
	m_Glossiness = m_GlossinessMin;
	m_Specular = m_SpecularMin;

	Apply(actor);
}
