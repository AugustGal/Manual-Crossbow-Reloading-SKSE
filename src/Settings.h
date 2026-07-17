#pragma once

class Settings
{
	public:

		static Settings* GetSingleton();

		void LoadForms();

		void LoadSettings();


		RE::FormID ParseFormID(const std::string& str);

		inline static bool debug_logging{};

		bool weightIncreasesStaminaCost;
		bool skillDecreasesStaminaCost;

		float staminaCostBase;
		float staminaCostMult;

		float reloadSpeedPerkBonus;

		bool isBladeAndBluntLoaded;
		bool isRequiemLoaded;

		RE::SpellItem* CrossbowStaminaSpell;
		RE::SpellItem* CrossbowStaminaSpellNPC;
		RE::TESGlobal* MCR_IsCrossbowLoaded;
		RE::BGSSoundDescriptorForm* MCR_WPNCrossbowReloadPlayer;
		RE::BGSSoundDescriptorForm* MCR_WPNCrossbowReloadQuickShotPerkPlayer;

	private:		

};