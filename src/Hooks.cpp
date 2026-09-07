#include "Hooks.h"
#include "Events.h"
#include "Settings.h"

namespace Hooks
{
	bool InstallHooks()
	{
		if (!AnimEventHandlerPC::InstallHook(AnimEventVtbl_PC)) { return false; }
		if (!AnimEventHandlerNPC::InstallHook(AnimEventVtbl_NPC)) { return false; }
		if (!NotifyAnimGraphHandlerPC::InstallHook(AnimGraphVtbl_PC)) { return false; }

		return true;
	}

	bool InstallDataLoadedHooks()
	{
		if (!ClipGeneratorHandler::InstallHook(vtblhkbClipGenerator)) { return false; }

		if (!Settings::GetSingleton()->isRequiemLoaded)
		{
			if (!WeaponFireHandler::InstallHook(arrow_release_handler)) { return false; }
		}

		return true;
	}
}