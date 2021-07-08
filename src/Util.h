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

		auto ext = str.ends_with(".esp"sv) || str.ends_with(".esm"sv) || str.ends_with(".esl"sv);
		if (!ext)
			return nullptr;

		std::string_view fid = str.substr(0, pos);
		std::string_view mod = str.substr(pos + 1);
		auto handler = RE::TESDataHandler::GetSingleton();
		auto file = handler->LookupModByName(mod);

		RE::FormID index = 0xFF;
		if (file)
			index = file->GetPartialIndex();

		RE::FormID id = std::stoul(std::string(fid), nullptr, 16);
		if (index < 0xFF)
			id = (index << 24) | id;
		else
			id = (index << 12) | (id & 0x00000FFF);

		return id ? RE::TESForm::LookupByID(id) : nullptr;
	}

	inline std::string AsString(RE::TESForm* a_form)
	{
		if (!a_form)
			return {};

		std::stringstream ss;
		ss << std::hex << GetBaseID(a_form) << "|";

		auto handler = RE::TESDataHandler::GetSingleton();
		auto index = GetModIndex(a_form);
		if (index < 0xFF) {
			auto file = handler->compiledFileCollection.files[index];
			ss << file->fileName;
		} else {
			auto file = handler->compiledFileCollection.smallFiles[index];
			ss << file->fileName;
		}

		return ss.str();
	}

	inline void VisitArmorAddon(RE::Actor* a_actor, RE::TESObjectARMO* a_armor, RE::TESObjectARMA* a_arma, std::function<void(bool a_firstPerson, RE::NiAVObject& a_obj)> a_visitor)
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
}
