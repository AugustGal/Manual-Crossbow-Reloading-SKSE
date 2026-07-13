#pragma once

class WeaponFireHandler
{
    public:

        static bool InstallHook(REL::Relocation<uintptr_t> a_relocation);

    private:

        static void ProcessWeaponFire(RE::TESObjectWEAP* a_weapon, RE::TESObjectREFR* a_source, RE::TESAmmo* a_ammo, RE::EnchantmentItem* a_ammoEnchantment, RE::AlchemyItem* a_poison);

        inline static REL::Relocation<decltype(ProcessWeaponFire)> _ProcessWeapon_Fire;
};

class AnimEventHandler
{
    public:

        static bool InstallHook(REL::Relocation<uintptr_t> a_relocation);

    private:

        static RE::BSEventNotifyControl ProcessAnimEventPC([[maybe_unused]] RE::BSTEventSink<RE::BSAnimationGraphEvent>* a_sink,
            RE::BSAnimationGraphEvent* a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>* a_eventSource);

        inline static REL::Relocation<decltype(ProcessAnimEventPC)> _ProcessAnimEvent_PC;
};


class ClipGeneratorHandler 
{
    public:

        static bool InstallHook(REL::Relocation<uintptr_t> a_relocation);

    private:

        static void ProcessClipGeneratorUpdate(RE::hkbClipGenerator* a_clipGenerator, const RE::hkbContext& a_context, float a_timesteps);

        inline static REL::Relocation<decltype(ProcessClipGeneratorUpdate)> _ProcessClipGeneratorUpdate;
};

[[nodiscard]] RE::BShkbAnimationGraph* GetGraphFromCharacter(RE::hkbCharacter* a_hkbCharacter);
