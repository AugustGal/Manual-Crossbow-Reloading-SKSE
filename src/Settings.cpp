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
    std::string REQFileName = "Requiem.esp";

    isBladeAndBluntLoaded = dataHandler->LookupLoadedLightModByName(BBFileName);
    isRequiemLoaded = dataHandler->LookupLoadedModByName(REQFileName);

    if (!isBladeAndBluntLoaded && !isRequiemLoaded)
    {
        CrossbowStaminaSpell = dataHandler->LookupForm(0x805, MCRFileName)->As<RE::SpellItem>();
        CrossbowStaminaSpellNPC = dataHandler->LookupForm(0x805, MCRFileName)->As<RE::SpellItem>();
    }

    if (isBladeAndBluntLoaded)
    {
        logger::info("Loading Blade and Blunt compatibility");

        auto spellForm = dataHandler->LookupForm(ParseFormID("0x873"), BBFileName);   
        if (!spellForm)
        {
            logger::info("Blade and Blunt player spell form not found");
        }

        CrossbowStaminaSpell = spellForm->As<RE::SpellItem>();

        auto spellFormNPC = dataHandler->LookupForm(ParseFormID("0x54"), BBFileName);

        if (spellFormNPC)
        {
            CrossbowStaminaSpellNPC = spellFormNPC->As<RE::SpellItem>();
        }
        else
        {
            CrossbowStaminaSpellNPC = spellForm->As<RE::SpellItem>();
            logger::info("Blade and Blunt NPC spell form not found");
        }

        logger::info("Blade and Blunt compatibility enabled");
    } 
    
    if (isRequiemLoaded)
    {
        logger::info("Loading Requiem compatibility");

        auto spellForm = dataHandler->LookupForm(0x6AA964, REQFileName);

        CrossbowStaminaSpell = spellForm->As<RE::SpellItem>();

        if (!spellForm)
        {
            logger::info("Requiem spell form not found");
        }

        logger::info("Requiem compatibility enabled");  
    }

	MCR_IsCrossbowLoaded = dataHandler->LookupForm(0x804, MCRFileName)->As<RE::TESGlobal>();
    MCR_WPNCrossbowReloadPlayer = dataHandler->LookupForm(0x80A, MCRFileName)->As<RE::BGSSoundDescriptorForm>();
    MCR_WPNCrossbowReloadQuickShotPerkPlayer = dataHandler->LookupForm(0x80B, MCRFileName)->As<RE::BGSSoundDescriptorForm>();

    logger::info("Loaded forms");

}

RE::FormID Settings::ParseFormID(const std::string& str)
{
    RE::FormID result;
    std::istringstream ss{ str };
    ss >> std::hex >> result;
    return result;
}
