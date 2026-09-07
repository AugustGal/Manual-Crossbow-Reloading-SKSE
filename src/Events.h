#pragma once

class WeaponFireHandler
{
    public:

        static bool InstallHook(REL::Relocation<uintptr_t> a_relocation);

    private:

        static void ProcessWeaponFire(RE::TESObjectWEAP* a_weapon, RE::TESObjectREFR* a_source, RE::TESAmmo* a_ammo, RE::EnchantmentItem* a_ammoEnchantment, RE::AlchemyItem* a_poison);

        inline static REL::Relocation<decltype(ProcessWeaponFire)> _ProcessWeapon_Fire;
};

class AnimEventHandlerPC
{
    public:

        static bool InstallHook(REL::Relocation<uintptr_t> a_relocation);

    private:

        static RE::BSEventNotifyControl ProcessAnimEventPC([[maybe_unused]] RE::BSTEventSink<RE::BSAnimationGraphEvent>* a_sink,
            RE::BSAnimationGraphEvent* a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>* a_eventSource);

        inline static REL::Relocation<decltype(ProcessAnimEventPC)> _ProcessAnimEvent_PC;
};

class AnimEventHandlerNPC
{
    public:

        static bool InstallHook(REL::Relocation<uintptr_t> a_relocation);

    private:

        static RE::BSEventNotifyControl ProcessAnimEventNPC([[maybe_unused]] RE::BSTEventSink<RE::BSAnimationGraphEvent>* a_sink,
            RE::BSAnimationGraphEvent* a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>* a_eventSource);

        inline static REL::Relocation<decltype(ProcessAnimEventNPC)> _ProcessAnimEvent_NPC;
};


class ClipGeneratorHandler 
{
    public:

        static bool InstallHook(REL::Relocation<uintptr_t> a_relocation);

    private:

        static void ProcessClipGeneratorUpdate(RE::hkbClipGenerator* a_clipGenerator, const RE::hkbContext& a_context, float a_timesteps);

        inline static REL::Relocation<decltype(ProcessClipGeneratorUpdate)> _ProcessClipGeneratorUpdate;  
};

class NotifyAnimGraphHandlerPC
{
    public:
        static bool InstallHook(REL::Relocation<uintptr_t> a_relocation);

    private:

        static void NotifyAnimGraphPC(RE::IAnimationGraphManagerHolder* a_graphHolder, const RE::BSFixedString& a_eventName);

        inline static REL::Relocation<decltype(NotifyAnimGraphPC)> _NotifyAnimGraph_PC;
};

class PlayerUpdateHandler
{
    public:
        static bool InstallHook(REL::Relocation<uintptr_t> a_relocation);

    private:

        static void Update(RE::Actor* a_this, float a_delta);

        inline static REL::Relocation<decltype(Update)> _Update;

        inline static float stepsCounter = 0.0f;
};

[[nodiscard]] RE::BShkbAnimationGraph* GetGraphFromCharacter(RE::hkbCharacter* a_hkbCharacter);