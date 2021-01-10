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

	static constexpr char CLASS_NAME[] = "qdx_gw";

	bool Register(VM* a_vm)
	{
		if (!a_vm) {
			logger::critical("Papyrus Util: Couldn't get VM");
			return false;
		}

		a_vm->RegisterFunction("PrintConsole", CLASS_NAME, PrintConsole);

		return true;
	}
}