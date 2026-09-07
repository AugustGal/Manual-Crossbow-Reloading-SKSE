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

    weightIncreasesStaminaCost = ini.GetBoolValue("Settings", "weightIncreasesStaminaCost", false);
    skillDecreasesStaminaCost = ini.GetBoolValue("Settings", "skillDecreasesStaminaCost", false);
    staminaCostBase = std::stof(ini.GetValue("Settings", "staminaCostBase", "0.0"));
    staminaCostMult = std::stof(ini.GetValue("Settings", "staminaCostMult", "1.0"));

    disableReloadOnEquip = ini.GetBoolValue("Settings", "disableReloadOnEquip", true);

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

    WPNCrossbowReload = dataHandler->LookupForm(0x8E3A, "Dawnguard.esm")->As<RE::BGSSoundDescriptorForm>();
    WPNCrossbowReloadQuickShotPerk = dataHandler->LookupForm(0x18BC4, "Dawnguard.esm")->As<RE::BGSSoundDescriptorForm>();

    MCR_WPNCrossbowReload = dataHandler->LookupForm(0x80A, MCRFileName)->As<RE::BGSSoundDescriptorForm>();
    MCR_WPNCrossbowReloadQuickShotPerk = dataHandler->LookupForm(0x80B, MCRFileName)->As<RE::BGSSoundDescriptorForm>();

    MCR_WPNCrossbowReload_Dummy = dataHandler->LookupForm(0x80F, MCRFileName)->As<RE::BGSSoundDescriptorForm>();
    MCR_WPNCrossbowReloadQuickShotPerk_Dummy = dataHandler->LookupForm(0x810, MCRFileName)->As<RE::BGSSoundDescriptorForm>();

    ProcessReloadSoundForms(WPNCrossbowReload, MCR_WPNCrossbowReload, MCR_WPNCrossbowReload_Dummy);
    ProcessReloadSoundForms(WPNCrossbowReloadQuickShotPerk, MCR_WPNCrossbowReloadQuickShotPerk, MCR_WPNCrossbowReloadQuickShotPerk_Dummy);

    QuickShot = dataHandler->LookupForm(0x105F19, "Skyrim.esm")->As<RE::BGSPerk>();

    ReloadRoot = dataHandler->LookupForm(0x590E, "Dawnguard.esm")->As<RE::TESIdleForm>();

    // Disable vanilla forced reload when equpping/switching bolts
    if (disableReloadOnEquip)
    {
        ReloadRoot->animEventName = ""sv;
    }

    logger::info("Loaded forms");
}

RE::FormID Settings::ParseFormID(const std::string& str)
{
    RE::FormID result;
    std::istringstream ss{ str };
    ss >> std::hex >> result;
    return result;
}

void Settings::ProcessReloadSoundForms(RE::BGSSoundDescriptorForm* a_origSoundForm, RE::BGSSoundDescriptorForm* a_newSoundForm, RE::BGSSoundDescriptorForm* a_dummySoundForm)
{
    auto* origSoundDescriptor = a_origSoundForm->soundDescriptor;
    if (!origSoundDescriptor) { return; }

    auto* origStandardSound = static_cast<RE::BGSStandardSoundDef*>(origSoundDescriptor);

    auto* dummySoundDescriptor = a_dummySoundForm->soundDescriptor;
    if (!dummySoundDescriptor) { return; }

    auto* dummyStandardSound = static_cast<RE::BGSStandardSoundDef*>(dummySoundDescriptor);

    auto* newSoundDescriptor = a_newSoundForm->soundDescriptor;
    if (!newSoundDescriptor) { return; }

    auto* newStandardSound = static_cast<RE::BGSStandardSoundDef*>(newSoundDescriptor);

    // Copy over the conditions to fully disable the original reload sound form.
    origStandardSound->conditions = dummyStandardSound->conditions;

    // Copy over the settings from the original reload sound form, for patches.
    newStandardSound->category = origStandardSound->category;
    newStandardSound->soundCharacteristics = origStandardSound->soundCharacteristics;
    newStandardSound->alternateSoundFormID = origStandardSound->alternateSoundFormID;
    newStandardSound->lengthCharacteristics = origStandardSound->lengthCharacteristics;
    newStandardSound->outputModel = origStandardSound->outputModel;
    newStandardSound->soundFiles = origStandardSound->soundFiles;
}