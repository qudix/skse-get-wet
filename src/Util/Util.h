#pragma once

namespace Util
{
	uint32_t GetBaseID(RE::TESForm* a_form)
	{
		return a_form ? ((a_form->formID >> 24 == 0xFE) ? a_form->formID & 0x00000FFF : a_form->formID & 0x00FFFFFF) : 0;
	}

	uint32_t GetModIndex(RE::TESForm* a_form)
	{
		if (a_form->formID == 0)
			return 0;

		auto id = a_form->formID >> 24;
		if (id == 0xFE)
			id = a_form->formID >> 12;

		return id;
	}

	/*
	RE::TESForm* StringToForm(std::string a_str)
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

		uint32_t index = 0xFF;
		if (file)
			index = file->GetPartialIndex();

		uint32_t id = std::stoul(std::string(fid), nullptr, 0);
		if (index < 0xFF)
			id = (index << 24) | id;
		else
			id = (index << 12) | (id & 0x00000FFF);

		return id ? RE::TESForm::LookupByID(id) : nullptr;
	}*/

	std::optional<std::string> FormToString(RE::TESForm* a_form)
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


}