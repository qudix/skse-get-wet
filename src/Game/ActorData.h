#pragma once

namespace Actor
{
	class ActorData
	{
	public:
		using BipedSlot = RE::BIPED_MODEL::BipedObjectSlot;

		ActorData() = default;
		~ActorData() = default;

		ActorData(SKSE::SerializationInterface* intfc, uint32_t& length) :
			ActorData()
		{
			m_Wetness = Data::ReadHelper<float>(intfc, length);
			m_WetnessRate = Data::ReadHelper<float>(intfc, length);
			m_WetnessForced = Data::ReadHelper<float>(intfc, length);
		}
		ActorData& operator=(ActorData&& other) = default;

		void Serialize(SKSE::SerializationInterface* intfc) const
		{
			Data::Write(intfc, &m_Wetness);
			Data::Write(intfc, &m_WetnessRate);
			Data::Write(intfc, &m_WetnessForced);
		}

		void ApplyObjProperties(RE::NiAVObject* a_obj, bool a_glossiness, bool a_specular)
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
						material->specularPower = data.Glossiness;
					if (a_specular)
						material->specularColorScale = data.Specular;
				}

				return Visit::kContinue;
			});
		}

		void ApplyHeadVisual(RE::Actor& a_actor, bool a_glossiness, bool a_specular)
		{
			auto obj = a_actor.GetHeadPartObject(RE::BGSHeadPart::HeadPartType::kFace);
			if (obj) {
				ApplyObjProperties(obj, a_glossiness, a_specular);
			}
		}

		void ApplySlotVisual(RE::Actor& a_actor, BipedSlot a_slot, bool a_glossiness, bool a_specular)
		{
			auto skin = a_actor.GetSkin(a_slot);
			if (!skin)
				return;

			auto addon = skin->GetArmorAddonByMask(a_actor.race, a_slot);
			if (!addon)
				return;

			bool isPlayer = a_actor.IsPlayerRef();
			a_actor.VisitArmorAddon(skin, addon, [&](bool a_firstPerson, RE::NiAVObject& a_obj) {
				if (!a_firstPerson || (a_firstPerson && isPlayer)) {
					ApplyObjProperties(&a_obj, a_glossiness, a_specular);
				}
			});
		}

		void ApplyVisual(RE::Actor& a_actor)
		{
			auto set = Settings::GetSingleton();
			auto& glossiness = set->data.Glossiness;
			auto& specular = set->data.Specular;
			ApplyHeadVisual(a_actor, glossiness.Head, specular.Head);
			ApplySlotVisual(a_actor, BipedSlot::kBody, glossiness.Body, specular.Body);
			ApplySlotVisual(a_actor, BipedSlot::kHands, glossiness.Hands, specular.Hands);
			ApplySlotVisual(a_actor, BipedSlot::kFeet, glossiness.Feet, specular.Feet);
			ApplySlotVisual(a_actor, BipedSlot::kUnnamed52, glossiness.Other, specular.Other);
		}

		std::vector<float> GetVisual()
		{
			return {
				data.Wetness,
				data.Glossiness,
				data.Specular
			};
		}

		void Reset()
		{
			auto actor = RE::TESObjectREFR::LookupByID<RE::Actor>(m_FormID);
			if (!actor)
				return;

			data.Wetness = 0;
			data.Glossiness = 30;
			data.Specular = 3;

			//ApplyVisual(*actor);
		}

		std::optional<Actor_t> ActorSettings(RE::Actor* a_actor)
		{
			auto actor_id = Util::FormToString(a_actor);
			if (!actor_id)
				return {};

			auto set = Settings::GetSingleton();
			auto& actors = set->data.Actors;
			for (auto& obj : actors) {
				auto& actor = obj.Actor;
				if (!actor.empty() && actor == *actor_id) {
					return obj;
				}
			}

			return {};
		}

		void Update()
		{
			auto actor = RE::TESObjectREFR::LookupByID<RE::Actor>(m_FormID);
			if (!actor)
				return;

			auto act = ActorSettings(actor);
			if (!act) {
				data = *act;
				m_Custom = true;
			} else {
				auto set = Settings::GetSingleton();
				data.Wetness = set->data.Wetness.Min;
				data.Glossiness = set->data.Glossiness.Min;
				data.Specular = set->data.Specular.Min;
				m_Custom = false;
			}

			//ApplyVisual(*actor);
		}

		void SetFormID(RE::FormID a_formID) { m_FormID = a_formID; }

		bool IsCustom() { return m_Custom; }

	private:
		RE::FormID m_FormID;

		// Default/Custom
		Actor_t data;

		// Active
		float m_Wetness = 0;
		float m_WetnessRate = 0;
		float m_WetnessForced = 0;

		bool m_Custom = false;

	private:
		ActorData& operator=(const ActorData&) = delete;
		ActorData(const ActorData&) = delete;
	};
}
