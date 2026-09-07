#pragma once

class Settings
{
	public:

		static Settings* GetSingleton();

		void LoadForms();

		void LoadSettings();

		RE::FormID ParseFormID(const std::string& str);
		void ProcessReloadSoundForms(RE::BGSSoundDescriptorForm* a_origSoundForm, RE::BGSSoundDescriptorForm* a_newSoundForm, RE::BGSSoundDescriptorForm* a_dummySoundForm);

		inline static bool debug_logging{};

		bool weightIncreasesStaminaCost;
		bool skillDecreasesStaminaCost;

		float staminaCostBase;
		float staminaCostMult;

		float reloadSpeedPerkBonus;

		bool isBladeAndBluntLoaded;
		bool isRequiemLoaded;

		bool disableReloadOnEquip;

		RE::SpellItem* CrossbowStaminaSpell;
		RE::SpellItem* CrossbowStaminaSpellNPC;

		RE::TESGlobal* MCR_IsCrossbowLoaded;

		RE::BGSSoundDescriptorForm* WPNCrossbowReload;
		RE::BGSSoundDescriptorForm* WPNCrossbowReloadQuickShotPerk;

		RE::BGSSoundDescriptorForm* MCR_WPNCrossbowReload;
		RE::BGSSoundDescriptorForm* MCR_WPNCrossbowReloadQuickShotPerk;

		RE::BGSSoundDescriptorForm* MCR_WPNCrossbowReload_Dummy;
		RE::BGSSoundDescriptorForm* MCR_WPNCrossbowReloadQuickShotPerk_Dummy;

		RE::TESIdleForm* ReloadRoot;

		RE::BGSPerk* QuickShot;

	private:

};



