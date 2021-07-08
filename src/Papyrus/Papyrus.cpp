#include "Papyrus.h"

#include "Game/Config.h"
#include "Game/MetaData.h"
#include "Game/Meta.h"
#include "Game/Presets.h"

#define BIND(a_method, ...) a_vm->RegisterFunction(#a_method##sv, obj, a_method __VA_OPT__(, ) __VA_ARGS__)

#include "Papyrus/PapyrusActor.h"
#include "Papyrus/PapyrusSettings.h"
#include "Papyrus/PapyrusUtil.h"

#undef BIND

namespace Papyrus
{
	void Bind()
	{
		auto papyrus = SKSE::GetPapyrusInterface();
		if (papyrus) {
			papyrus->Register(PapyrusActor::Bind);
			papyrus->Register(PapyrusSettings::Bind);
			papyrus->Register(PapyrusUtil::Bind);
		}
	}
}
