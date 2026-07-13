#include "Settings.h"
#include <SimpleIni.h>

Settings* Settings::GetSingleton()
{
	static Settings settings;
	return &settings;
}

void Settings::LoadSettings()
{
    logger::info("Loading settings");

    CSimpleIniA ini;

    ini.SetUnicode();
    ini.LoadFile(R"(.\Data\SKSE\Plugins\ManualCrossbowReloading.ini)");

    debug_logging = ini.GetBoolValue("Log", "Debug");

    if (debug_logging) 
    {
        spdlog::set_level(spdlog::level::debug);
        logger::debug("Debug logging enabled");
    }

    weightIncreasesStaminaCost = ini.GetBoolValue("Settings", "weightIncreasesStaminaCost", true);
    skillDecreasesStaminaCost = ini.GetBoolValue("Settings", "skillDecreasesStaminaCost", true);
    staminaCostBase = std::stof(ini.GetValue("Settings", "staminaCostBase", "0.0"));
    staminaCostMult = std::stof(ini.GetValue("Settings", "staminaCostMult", "1.0"));

    reloadSpeedPerkBonus = std::stof(ini.GetValue("Settings", "reloadSpeedPerkBonus", "0.3"));

    logger::info("Loaded settings");
}

void Settings::LoadForms()
{
    logger::info("Loading forms");

	auto dataHandler = RE::TESDataHandler::GetSingleton();

    std::string MCRFileName = "ManualCrossbowReloading.esp";
    std::string BBFileName = "BladeAndBlunt.esp";

    if (!dataHandler->LookupLoadedLightModByName(BBFileName))
    {
        CrossbowStaminaSpell = dataHandler->LookupForm(0x805, MCRFileName)->As<RE::SpellItem>();
        CrossbowStaminaSpellNPC = dataHandler->LookupForm(0x805, MCRFileName)->As<RE::SpellItem>();
    }
    else 
    {
        logger::info("Loading Blade and Blunt compatibility");

        auto spellForm = dataHandler->LookupForm(ParseFormID("0x873"), BBFileName);
        
        CrossbowStaminaSpell = spellForm->As<RE::SpellItem>();

        auto spellFormNPC = dataHandler->LookupForm(ParseFormID("0x54"), BBFileName);

        if (spellFormNPC)
        {
            CrossbowStaminaSpellNPC = spellFormNPC->As<RE::SpellItem>();
        }
        else
        {
            CrossbowStaminaSpellNPC = spellForm->As<RE::SpellItem>();
        }
        
        isBladeAndBluntLoaded = true;

        logger::info("Blade and Blunt compatibility enabled");
    }

	MCR_IsCrossbowLoaded = dataHandler->LookupForm(0x804, MCRFileName)->As<RE::TESGlobal>();

    logger::info("Loaded forms");

}

RE::FormID Settings::ParseFormID(const std::string& str)
{
    RE::FormID result;
    std::istringstream ss{ str };
    ss >> std::hex >> result;
    return result;
}
