#pragma once

#include "Papyrus/PapyrusActor.h"
#include "Papyrus/PapyrusSettings.h"
#include "Papyrus/PapyrusUtil.h"

namespace Papyrus
{
	bool Register()
	{
		auto papyrus = SKSE::GetPapyrusInterface();
		if (!papyrus) {
			logger::critical("Papyrus: Couldn't get Interface");
			return false;
		}

		papyrus->Register(PapyrusActor::Register);
		papyrus->Register(PapyrusSettings::Register);
		papyrus->Register(PapyrusUtil::Register);

		return true;
	}
}