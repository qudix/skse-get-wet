#pragma once

namespace PapyrusUtil
{
	using VM = RE::BSScript::IVirtualMachine;

	void PrintConsole(RE::StaticFunctionTag*, std::string a_message)
	{
		const auto log = RE::ConsoleLog::GetSingleton();
		if (log) {
			log->Print(a_message.c_str());
		}
	}

	void Bind(VM& a_vm)
	{
		const auto obj = "qdx_gw"sv;

		BIND(PrintConsole);
	}
}