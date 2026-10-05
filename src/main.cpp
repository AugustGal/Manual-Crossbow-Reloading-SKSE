#include "Hooks.h"
#include "Settings.h"
#include "CrossbowReloadManager.h"

void InitListener(SKSE::MessagingInterface::Message* a_msg)
{
	auto settings = Settings::GetSingleton();
	switch (a_msg->type)
	{
	case SKSE::MessagingInterface::kNewGame:
		break;
	case SKSE::MessagingInterface::kPostLoadGame:
	{
		CrossbowReloadManager::GetSingleton()->PostLoadMaintenance();
		break;
	}

	case SKSE::MessagingInterface::kPostLoad:
		break;
	case SKSE::MessagingInterface::kDataLoaded:

		if (settings)
		{
			settings->LoadForms();
		}

		Hooks::InstallDataLoadedHooks();

		break;
	}
}

SKSEPluginLoad(const SKSE::LoadInterface* a_skse)
{
	SKSE::Init(a_skse);

	const auto plugin{ SKSE::PluginDeclaration::GetSingleton() };
	const auto name{ plugin->GetName() };
	const auto version{ plugin->GetVersion() };

	logger::init();

	logger::info("{} {} is loading...", name, version);

	Settings::GetSingleton()->LoadSettings();

	if (!Hooks::InstallHooks())
	{
		logger::error("Hook installation failed");
		return false;
	}

	auto messaging = SKSE::GetMessagingInterface();
	if (!messaging->RegisterListener(InitListener))
	{
		return false;
	}

	return true;
}