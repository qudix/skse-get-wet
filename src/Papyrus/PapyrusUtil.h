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

	bool Bind(VM* a_vm)
	{
		const auto obj = "qdx_gw"sv;

		BIND(PrintConsole);

		return true;
	}
}
