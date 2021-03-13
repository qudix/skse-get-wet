#pragma once

namespace Papyrus::PapyrusUtil
{
	using VM = RE::BSScript::IVirtualMachine;

	inline void PrintConsole(RE::StaticFunctionTag*, std::string a_message)
	{
		const auto log = RE::ConsoleLog::GetSingleton();
		if (log) {
			log->Print(a_message.c_str());
		}
	}

	inline void Bind(VM& a_vm)
	{
		const auto obj = "qdx_gw"sv;

		BIND(PrintConsole);
	}
}