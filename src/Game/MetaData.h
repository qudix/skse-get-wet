#pragma once

class MetaData final
{
public:
	using BipedSlot = RE::BIPED_MODEL::BipedObjectSlot;

	MetaData() = default;
	~MetaData() = default;

	bool Save(SKSE::SerializationInterface* a_intfc);
	void Load(SKSE::SerializationInterface* a_intfc, uint32_t& a_length);

	void ApplyProperties(RE::NiAVObject* a_obj, bool a_glossiness, bool a_specular);
	void ApplyHead(RE::Actor* a_actor, bool a_glossiness, bool a_specular);
	void ApplySlot(RE::Actor* a_actor, BipedSlot a_slot, bool a_glossiness, bool a_specular);
	void Apply(RE::Actor* a_actor);

	bool UpdatePreset(RE::Actor* a_actor);
	void Update(bool a_reset = false);

	void SetFormID(RE::FormID& a_formID) { m_FormID = a_formID; };
	RE::FormID GetFormID() { return m_FormID; };

public:
	RE::FormID m_FormID;

	// Active
	float m_Wetness = 0;
	float m_WetnessRate = 0;
	float m_Glossiness = 0;
	float m_Specular = 0;

	// Default/Custom
	float m_WetnessMin{ 0 };
	float m_WetnessMax{ 0 };
	float m_GlossinessMin{ 0 };
	float m_GlossinessMax{ 0 };
	float m_SpecularMin{ 0 };
	float m_SpecularMax{ 0 };
	bool m_ForceWetness{ false };
	bool m_ForceGlossiness{ false };
	bool m_ForceSpecular{ false };
};
