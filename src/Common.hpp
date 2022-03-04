#pragma once

namespace Util
{
	inline uint32_t GetBaseID(RE::TESForm* a_form)
	{
		return a_form ? ((a_form->formID >> 24 == 0xFE) ? a_form->formID & 0x00000FFF : a_form->formID & 0x00FFFFFF) : 0;
	}

	inline uint32_t GetModIndex(RE::TESForm* a_form)
	{
		if (a_form->formID == 0)
			return 0;

		auto id = a_form->formID >> 24;
		if (id == 0xFE)
			id = a_form->formID >> 12;

		return id;
	}

	inline RE::TESForm* AsForm(std::string a_str)
	{
		if (a_str.empty() || a_str.size() < 4)
			return nullptr;

		std::string_view str(a_str);

		auto pos = str.find('|');
		if (pos == std::string_view::npos)
			return nullptr;

		std::string_view mod = str.substr(0, pos);
		auto ext = mod.ends_with(".esp"sv) || mod.ends_with(".esm"sv) || mod.ends_with(".esl"sv);
		if (!ext)
			return nullptr;

		std::string_view fid = str.substr(pos + 1);
		auto handler = RE::TESDataHandler::GetSingleton();
		auto file = handler->LookupModByName(mod);

		RE::FormID index = file ? file->GetPartialIndex() : 0xFF;
		RE::FormID id = std::stoul(std::string(fid), nullptr, 16);
		logger::info("id: {:0x}", id);
		if (index < 0xFF)
			id = (index << 24) | id;
		else
			id = (index << 12) | (id & 0x00000FFF);

		return RE::TESForm::LookupByID(id);
	}

	inline std::string AsString(RE::TESForm* a_form)
	{
		if (!a_form)
			return {};

		std::stringstream ss;
		auto handler = RE::TESDataHandler::GetSingleton();
		auto index = GetModIndex(a_form);
		if (index < 0xFF) {
			auto file = handler->compiledFileCollection.files[index];
			ss << file->fileName;
		} else {
			auto file = handler->compiledFileCollection.smallFiles[index];
			ss << file->fileName;
		}

		ss << "|" << std::hex << GetBaseID(a_form);

		return ss.str();
	}

	// Version 1
	/*
	inline void VisitArmorAddon(RE::Actor* a_actor, RE::TESObjectARMO* a_armor, RE::TESObjectARMA* a_arma, std::function<void(bool, RE::NiAVObject&)> a_visitor)
	{
		enum
		{
			k3rd,
			k1st,
			kTotal
		};

		char addonString[WinAPI::MAX_PATH]{ '\0' };
		a_arma->GetNodeName(addonString, a_actor, a_armor, -1);
		std::array<RE::NiAVObject*, kTotal> skeletonRoot = { a_actor->Get3D(k3rd), a_actor->Get3D(k1st) };
		if (skeletonRoot[k1st] == skeletonRoot[k3rd]) {
			skeletonRoot[k1st] = nullptr;
		}
		for (std::size_t i = 0; i < skeletonRoot.size(); ++i) {
			if (skeletonRoot[i]) {
				const auto obj = skeletonRoot[i]->GetObjectByName(addonString);
				if (obj) {
					a_visitor(i == k1st, *obj);
				}
			}
		}
	}*/

	// Version 2
	inline void VisitArmorAddon(RE::Actor* a_actor, RE::TESObjectARMO* a_armor, RE::TESObjectARMA* a_arma, std::function<void(bool, RE::NiAVObject&)> a_visitor)
	{
		char addonString[WinAPI::MAX_PATH]{ '\0' };
		a_arma->GetNodeName(addonString, a_actor, a_armor, -1);
		const auto third = a_actor->Get3D();
		const auto first = a_actor->Get3D(true);

		auto obj = third ? third->GetObjectByName(addonString) : nullptr;
		if (obj) {
			a_visitor(false, *obj);
		}

		obj = (first && first != third) ? first->GetObjectByName(addonString) : nullptr;
		if (obj) {
			a_visitor(true, *obj);
		}
	}

	inline RE::TESObjectARMA* GetArmorAddonByMask(RE::TESObjectARMO* a_armo, RE::TESRace* a_race, RE::BGSBipedObjectForm::BipedObjectSlot a_slot)
	{
		for (auto& addon : a_armo->armorAddons) {
			if (addon && addon->IsValidRace(a_race) && addon->HasPartOf(a_slot)) {
				return addon;
			}
		}

		return nullptr;
	}

	inline bool IsWithinRadius(RE::TESObjectREFR* a_centerObj, RE::TESObjectREFR* a_objRef, float a_radius)
	{
		if (a_radius <= 0.0f)
			return false;

		RE::NiPoint3 a = a_centerObj->GetPosition();
		RE::NiPoint3 b = a_objRef->GetPosition();

		float tempx = std::abs(a.x - b.x);
		float tempy = std::abs(a.y - b.y);
		float tempz = std::abs(a.z - b.z);

		if (tempx + tempy + tempz < a_radius)
			return true;  // very small distances
		if (tempx + tempy + tempz > a_radius / 2)
			return false;  // very large distances

		tempx = tempx * tempx;
		tempy = tempy * tempy;
		tempz = tempz * tempz;

		float tempd = a_radius * a_radius;
		if (tempx + tempy + tempz < tempd)
			return true;  // near but within distance

		return false;
	}
}
