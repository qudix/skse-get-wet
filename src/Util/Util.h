#pragma once

namespace Util
{
	bool EndsWith(const std::string& str, const std::string& suffix)
	{
		return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
	}

	bool IsFormString(const std::string& a_str)
	{
		if (a_str.size() < 4 || a_str.find("|") == std::string::npos)
			return false;

		return EndsWith(a_str, ".esp") || EndsWith(a_str, ".esm") || EndsWith(a_str, ".esl");
	}

	uint32_t GetBaseID(RE::TESForm* a_form)
	{
		return a_form ? ((a_form->formID >> 24 == 0xFE) ? a_form->formID & 0x00000FFF : a_form->formID & 0x00FFFFFF) : 0;
	}

	uint32_t GetModIndex(RE::TESForm* a_form)
	{
		if (!a_form || a_form->formID == 0)
			return 0;

		auto id = a_form->formID >> 24;
		if (id == 0xFE)
			id = a_form->formID >> 12;

		return id;
	}

	RE::TESForm* ParseFormString(std::string a_str)
	{
		if (!IsFormString(a_str))
			return nullptr;

		std::size_t pos = a_str.find("|");
		std::string objID = a_str.substr(0, pos).c_str();

		uint32_t obj = std::atoi(objID.c_str());;

		std::string_view mod = a_str.substr(pos + 1);
		auto handler = RE::TESDataHandler::GetSingleton();
		auto file = handler->LookupModByName(mod);

		uint32_t index = 0xFF;
		if (file)
			index = file->GetPartialIndex();

		if (index < 0xFF)
			obj = (index << 24) | obj;
		else
			obj = (index << 12) | (obj & 0x00000FFF);

		return obj ? RE::TESForm::LookupByID(obj) : nullptr;
	}

	std::string GetFormString(RE::TESForm* a_form)
	{
		if (!a_form)
			return "";

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