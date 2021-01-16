#pragma once

namespace Actor
{
	using BipedSlot = RE::BIPED_MODEL::BipedObjectSlot;

	struct ActorProps
	{
		RE::FormID m_ID = 0;
		float m_GlossinessMin = 30;
		float m_GlossinessMax = 30;
		float m_SpecularMin = 3;
		float m_SpecularMax = 3;

		// TODO?
		float m_Wetness = 0;
		float m_WetnessRate = 0;
		bool m_WetnessForced = false;

		bool m_HasSettings = false;
	};

	RE::TESQuest* m_quest = nullptr;

	using ActorPropsPtr = std::shared_ptr<ActorProps>;
	std::vector<ActorPropsPtr> m_actorProps;

	bool IsValid(RE::Actor* a_actor)
	{
		if (!a_actor || !a_actor->Is3DLoaded() || a_actor->IsDead())
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

	bool IsValidProp(RE::Actor* a_actor)
	{
		auto settings = Settings::GetSingleton();

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
		auto keywordBeast = dobj->GetObject<RE::BGSKeyword>(RE::DEFAULT_OBJECT::kKeywordBeastRace);
		bool beast = a_actor->HasKeyword(keywordBeast);
		if (beast && !settings->Get<bool>("ApplyBeast"))
			return false;

		return true;
	}

	void SetObjProperties(RE::NiAVObject* a_obj, ActorPropsPtr a_props, bool a_glossiness, bool a_specular)
	{
		auto task = SKSE::GetTaskInterface();
		task->AddTask([a_obj, a_props, a_glossiness, a_specular]() {
			if (!a_obj)
				return;

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
						material->specularPower = a_props->m_GlossinessMin;
					if (a_specular)
						material->specularColorScale = a_props->m_SpecularMin;
				}

				return Visit::kContinue;
			});
		});
	}

	void SetHeadVisual(RE::Actor* a_actor, ActorPropsPtr a_props)
	{
		auto obj = a_actor->GetHeadPartObject(RE::BGSHeadPart::HeadPartType::kFace);
		if (obj) {
			auto settings = Settings::GetSingleton();
			bool glossiness = settings->Get<bool>("VisualGlossinessHead");
			bool specular = settings->Get<bool>("VisualSpecularHead");
			SetObjProperties(obj, a_props, glossiness, specular);
		}
	}

	void SetSlotVisual(RE::Actor* a_actor, ActorPropsPtr a_props, BipedSlot a_slot, std::string a_glossiness, std::string a_specular)
	{
		auto skin = a_actor->GetSkin(a_slot);
		if (!skin)
			return;

		auto addon = skin->GetArmorAddonByMask(a_actor->race, a_slot);
		if (!addon)
			return;

		bool isPlayer = a_actor->IsPlayerRef();
		auto settings = Settings::GetSingleton();
		bool glossiness = settings->Get<bool>(a_glossiness);
		bool specular = settings->Get<bool>(a_specular);
		a_actor->VisitArmorAddon(skin, addon, [&](bool a_firstPerson, RE::NiAVObject& a_obj) {
			RE::NiAVObject* obj = &a_obj;
			if (!a_firstPerson || (a_firstPerson && isPlayer)) {
				SetObjProperties(obj, a_props, glossiness, specular);
			}
		});
	}

	ActorPropsPtr GetProps(RE::Actor* a_actor)
	{
		auto it = std::find_if(m_actorProps.begin(), m_actorProps.end(), [&](ActorPropsPtr props) {
			return a_actor == RE::TESObjectREFR::LookupByID<RE::Actor>(props->m_ID);
		});

		if (it != m_actorProps.end())
			return *it;

		return nullptr;
	}

	void RemoveProps(RE::Actor* a_actor)
	{
		auto it = std::find_if(m_actorProps.begin(), m_actorProps.end(), [&](ActorPropsPtr props) {
			return a_actor == RE::TESObjectREFR::LookupByID<RE::Actor>(props->m_ID);
		});

		if (it != m_actorProps.end())
			m_actorProps.erase(it);
	}

	void SetVisuals(RE::Actor* a_actor, ActorPropsPtr a_props)
	{
		if (!a_actor || !a_props)
			return;

		SetHeadVisual(a_actor, a_props);
		SetSlotVisual(a_actor, a_props, BipedSlot::kBody, "VisualGlossinessBody", "VisualSpecularBody");
		SetSlotVisual(a_actor, a_props, BipedSlot::kHands, "VisualGlossinessHands", "VisualSpecularHands");
		SetSlotVisual(a_actor, a_props, BipedSlot::kFeet, "VisualGlossinessFeet", "VisualSpecularFeet");
		SetSlotVisual(a_actor, a_props, BipedSlot::kUnnamed52, "VisualGlossinessOther", "VisualSpecularOther");
	}

	void ResetActor(RE::Actor* a_actor)
	{
		auto props = std::make_shared<ActorProps>();
		RemoveProps(a_actor);
		SetVisuals(a_actor, props);
	}

	void UpdateActor(RE::Actor* a_actor)
	{
		if (!m_quest || !m_quest->IsRunning())
			return;

		if (!IsValid(a_actor))
			return;

		// TODO: check if stored formid is still the same
		bool valid = IsValidProp(a_actor);
		if (!valid) {
			ResetActor(a_actor);
			return;
		}

		bool is_new = false;
		auto props = GetProps(a_actor);
		if (!props) {
			props = std::make_shared<ActorProps>();
			props->m_ID = a_actor->GetFormID();
			is_new = true;
		}

		auto settings = Settings::GetSingleton();
		auto set = SettingsActor::GetActorSettings(a_actor);
		if (!set.empty()) {
			props->m_GlossinessMin = set["VisualGlossinessMin"].get<float>();
			props->m_GlossinessMax = set["VisualGlossinessMax"].get<float>();
			props->m_SpecularMin = set["VisualSpecularMin"].get<float>();
			props->m_SpecularMax = set["VisualSpecularMax"].get<float>();
			props->m_HasSettings = true;
		} else {
			props->m_GlossinessMin = settings->Get<float>("VisualGlossinessMin");
			props->m_GlossinessMax = settings->Get<float>("VisualGlossinessMax");
			props->m_SpecularMin = settings->Get<float>("VisualSpecularMin");
			props->m_SpecularMax = settings->Get<float>("VisualSpecularMax");
			props->m_HasSettings = false;
		}

		if (settings->Get<bool>("ApplyGlobal"))
			SetVisuals(a_actor, props);

		if (is_new)
			m_actorProps.push_back(std::move(props));
	}

	void Update()
	{
		if (!m_quest || !m_quest->IsRunning())
			return;

		auto player = RE::PlayerCharacter::GetSingleton();
		UpdateActor(player);

		auto pl = RE::ProcessLists::GetSingleton();
		for (auto& handle : pl->highActorHandles) {
			auto actor = handle.get().get();
			UpdateActor(actor);
		}
	}

	void Setup()
	{
		auto handler = RE::TESDataHandler::GetSingleton();
		m_quest = handler->LookupForm(RE::FormID(0x800), "qdx-get-wet.esp")->As<RE::TESQuest>();
	}
}
