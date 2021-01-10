#pragma once

#include "Util/Util.h"

namespace Actor
{
	using BipedSlot = RE::BIPED_MODEL::BipedObjectSlot;

	struct ActorProps
	{
		RE::Actor* m_Actor = nullptr;
		float m_GlossinessMin = 30;
		float m_GlossinessMax = 30;
		float m_SpecularMin = 3;
		float m_SpecularMax = 3;
		bool m_HasSettings = false;
	};

	std::vector<ActorProps> m_actorProps;

	bool IsWithinRadius(RE::TESObjectREFR* a_centerObj, RE::TESObjectREFR* a_objRef, float a_radius)
	{
		if (!(a_radius > 0.0f))
			return false;

		RE::NiPoint3 a = a_centerObj->GetPosition();
		RE::NiPoint3 b = a_objRef->GetPosition();

		float tempx = std::abs(a.x - b.x);
		float tempy = std::abs(a.y - b.y);
		float tempz = std::abs(a.z - b.z);

		if (tempx + tempy + tempz < a_radius)
			return true; // very small distances
		if (tempx + tempy + tempz > a_radius / 2)
			return false; // very large distances

		tempx = tempx * tempx;
		tempy = tempy * tempy;
		tempz = tempz * tempz;

		float tempd = a_radius * a_radius;
		if (tempx + tempy + tempz < tempd)
			return true; // near but within distance

		return false;
	}

	bool IsValid(RE::Actor* a_actor)
	{
		auto settings = Settings::GetSingleton();

		if (!a_actor || !a_actor->Is3DLoaded() || a_actor->IsDead())
			return false;

		auto female = a_actor->GetActorBase()->IsFemale();
		if (female && !settings->Get<bool>("ApplyFemale"))
			return false;

		if (!female && !settings->Get<bool>("ApplyMale"))
			return false;

		bool player = a_actor->IsPlayerRef();
		if (player && !settings->Get<bool>("ApplyPlayer"))
			return false;

		if (!player && !settings->Get<bool>("ApplyNPC"))
			return false;

		auto dobj = RE::BGSDefaultObjectManager::GetSingleton();
		auto keywordNPC = dobj->GetObject<RE::BGSKeyword>(RE::DEFAULT_OBJECT::kKeywordNPC);
		if (!a_actor->HasKeyword(keywordNPC))
			return false;

		auto keywordBeast = dobj->GetObject<RE::BGSKeyword>(RE::DEFAULT_OBJECT::kKeywordBeastRace);
		bool beast = a_actor->HasKeyword(keywordBeast);
		if (beast && !settings->Get<bool>("ApplyBeast"))
			return false;

		return true;
	}

	void SetObjProperties(RE::NiAVObject* a_obj, ActorProps a_props, std::string a_glossiness, std::string a_specular)
	{
		using State = RE::BSGeometry::States;
		using Feature = RE::BSShaderMaterial::Feature;
		RE::BSVisit::TraverseScenegraphGeometries(a_obj, [&](RE::BSGeometry* a_geometry) -> RE::BSVisit::BSVisitControl {
			auto effect = a_geometry->properties[State::kEffect].get();
			if (effect) {
				auto lightingShader = netimmerse_cast<RE::BSLightingShaderProperty*>(effect);
				if (lightingShader) {
					auto material = static_cast<RE::BSLightingShaderMaterialBase*>(lightingShader->material);
					if (material) {
						auto type = material->GetFeature();
						if (type == Feature::kFaceGenRGBTint || type == Feature::kFaceGen) {
							auto settings = Settings::GetSingleton();
							if (settings->Get<bool>(a_glossiness))
								material->specularPower = a_props.m_GlossinessMin;
							if (settings->Get<bool>(a_specular))
								material->specularColorScale = a_props.m_SpecularMin;
						}
					}
				}
			}
			return RE::BSVisit::BSVisitControl::kContinue;
		});
	}

	void SetHeadVisual(ActorProps a_props)
	{
		auto obj = a_props.m_Actor->GetHeadPartObject(RE::BGSHeadPart::HeadPartType::kFace);
		if (obj) {
			auto task = SKSE::GetTaskInterface();
			task->AddTask([obj, a_props]() {
				SetObjProperties(obj, a_props, "VisualGlossinessHead", "VisualSpecularHead");
			});
		}
	}

	void SetSlotVisual(ActorProps a_props, BipedSlot a_slot, std::string a_glossiness, std::string a_specular)
	{
		auto actor = a_props.m_Actor;
		auto skin = actor->GetSkin(a_slot);
		if (!skin)
			return;

		auto addon = skin->GetArmorAddonByMask(actor->race, a_slot);
		if (!addon)
			return;

		auto obj = actor->VisitArmorAddon(skin, addon);
		if (obj) {
			auto task = SKSE::GetTaskInterface();
			task->AddTask([obj, a_props, a_glossiness, a_specular]() {
				SetObjProperties(obj, a_props, a_glossiness, a_specular);
			});
		}
	}

	json GetActorSettings(RE::Actor* a_actor)
	{
		auto actor_id = Util::GetFormString(a_actor);
		auto settings = Settings::GetSingleton();
		auto& actors = settings->Get("Actors");

		for (auto& obj : actors) {
			auto actor = obj.value("Actor", "");
			if (!actor.empty() && actor == actor_id) {
				return obj;
			}
		}

		return {};
	}

	void SetActorSetting(RE::Actor* a_actor, std::string a_key, float a_value)
	{
		auto actor_id = Util::GetFormString(a_actor);
		auto settings = Settings::GetSingleton();
		auto& actors = settings->Get("Actors");

		for (auto& [index, obj] : actors.items()) {
			auto actor = obj.value("Actor", "");
			if (!actor.empty() && actor == actor_id) {
				obj[a_key] = a_value;
				break;
			}
		}

		settings->Save();
	}

	void AddActorSettings(RE::Actor* a_actor)
	{
		auto actor_id = Util::GetFormString(a_actor);
		auto settings = Settings::GetSingleton();
		auto& actors = settings->Get("Actors");
		auto obj = json::object();

		obj["Actor"] = actor_id;
		obj["VisualGlossinessMin"] = settings->Get<float>("VisualGlossinessMin");
		obj["VisualGlossinessMax"] = settings->Get<float>("VisualGlossinessMax");
		obj["VisualSpecularMin"] = settings->Get<float>("VisualSpecularMin");
		obj["VisualSpecularMax"] = settings->Get<float>("VisualSpecularMax");

		actors.push_back(obj);
		settings->Save();
	}

	void RemoveActorSettings(RE::Actor* a_actor)
	{
		auto actor_id = Util::GetFormString(a_actor);
		auto settings = Settings::GetSingleton();
		auto& actors = settings->Get("Actors");

		int index = -1;
		for (auto& it : actors.items()) {
			auto actor = it.value().at("Actor").get<std::string>();
			if (!actor.empty() && actor == actor_id) {
				index = std::stoi(it.key());
				break;
			}
		}

		if (index != -1) {
			actors.erase(index);
			settings->Save();
		}
	}

	void SetActor(RE::Actor* a_actor, bool a_running)
	{
		if (!IsValid(a_actor))
			return;

		auto settings = Settings::GetSingleton();
		auto set = GetActorSettings(a_actor);

		ActorProps props;
		props.m_Actor = a_actor;

		if (!set.empty()) {
			props.m_GlossinessMin = set["VisualGlossinessMin"].get<float>();
			props.m_GlossinessMax = set["VisualGlossinessMax"].get<float>();
			props.m_SpecularMin = set["VisualSpecularMin"].get<float>();
			props.m_SpecularMax = set["VisualSpecularMax"].get<float>();
			props.m_HasSettings = true;
		} else {
			props.m_GlossinessMin = settings->Get<float>("VisualGlossinessMin");
			props.m_GlossinessMax = settings->Get<float>("VisualGlossinessMax");
			props.m_SpecularMin = settings->Get<float>("VisualSpecularMin");
			props.m_SpecularMax = settings->Get<float>("VisualSpecularMax");
		}

		m_actorProps.push_back(props);

		if (a_running) {
			SetHeadVisual(props);
			SetSlotVisual(props, BipedSlot::kBody, "VisualGlossinessBody", "VisualSpecularBody");
			SetSlotVisual(props, BipedSlot::kHands, "VisualGlossinessHands", "VisualSpecularHands");
			SetSlotVisual(props, BipedSlot::kFeet, "VisualGlossinessFeet", "VisualSpecularFeet");
			SetSlotVisual(props, BipedSlot::kUnnamed52, "VisualGlossinessOther", "VisualSpecularOther");
		}
	}

	void Update(bool a_running)
	{
		auto pl = RE::ProcessLists::GetSingleton();
		auto player = RE::PlayerCharacter::GetSingleton();
		if (!pl || !player)
			return;

		m_actorProps.clear();

		SetActor(player, a_running);

		auto settings = Settings::GetSingleton();
		auto range = settings->Get<float>("UpdateLoopRange");
		for (auto handle : pl->highActorHandles) {
			auto ptr = handle.get();
			RE::Actor* actor = ptr.get();

			if (!actor || !actor->formType.all(RE::FormType::ActorCharacter))
				continue;

			auto race = actor->GetRace();
			if (race->data.flags.all(RE::RACE_DATA::Flag::kChild))
				continue;

			if (!IsWithinRadius(player, actor, range))
				continue;

			SetActor(actor, a_running);
		}
	}
}
