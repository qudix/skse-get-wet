#pragma once

#define BIND(a_method, ...) a_vm.RegisterFunction(#a_method##sv, obj, a_method __VA_OPT__(, ) __VA_ARGS__)
#include "Papyrus/PapyrusActor.h"
#include "Papyrus/PapyrusSettings.h"
#include "Papyrus/PapyrusUtil.h"
#undef BIND

namespace Papyrus
{
	using VM = RE::BSScript::IVirtualMachine;

	bool Bind(VM* a_vm)
	{
		if (!a_vm) {
			return false;
		}

		PapyrusActor::Bind(*a_vm);
		PapyrusSettings::Bind(*a_vm);
		PapyrusUtil::Bind(*a_vm);

		return true;
	}
};
