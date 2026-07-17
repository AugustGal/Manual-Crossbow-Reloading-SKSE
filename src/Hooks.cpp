#include "Hooks.h"
#include "Events.h"
#include "Settings.h"

namespace Hooks
{
	bool InstallHooks()
	{
		if (!AnimEventHandler::InstallHook(AnimEventVtbl_PC)) { return false; }

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