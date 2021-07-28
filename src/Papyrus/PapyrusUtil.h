#pragma once

namespace PapyrusUtil
{
	using VM = RE::BSScript::IVirtualMachine;

	void PrintConsole(RE::StaticFunctionTag*, std::string a_msg)
	{
		const auto log = RE::ConsoleLog::GetSingleton();
		if (log) {
			log->Print(a_msg.c_str());
		}
	}

	std::string FormatFloat(RE::StaticFunctionTag*, std::string a_fmt, float a_val)
	{
		return fmt::format(a_fmt, a_val);
	}

	bool Bind(VM* a_vm)
	{
		const auto obj = "qdx_gw"sv;

		BIND(PrintConsole);
		BIND(FormatFloat);

		return true;
	}
}
