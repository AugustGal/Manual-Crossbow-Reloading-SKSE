#include "Events.h"
#include "CrossbowReloadManager.h"
#include "Utility.h"

bool WeaponFireHandler::InstallHook(REL::Relocation<uintptr_t> a_relocation)
{
    auto& trampoline = SKSE::GetTrampoline();
    _ProcessWeapon_Fire = trampoline.write_call<5>(a_relocation.address(), ProcessWeaponFire);

    return true;
}

void WeaponFireHandler::ProcessWeaponFire(RE::TESObjectWEAP* a_weapon, RE::TESObjectREFR* a_source, RE::TESAmmo* a_ammo, RE::EnchantmentItem* a_ammoEnchantment, RE::AlchemyItem* a_poison)
{
    _ProcessWeapon_Fire(a_weapon, a_source, a_ammo, a_ammoEnchantment, a_poison);

    if (!a_source) { return; }

    CrossbowReloadManager::GetSingleton()->HandleWeaponFire(a_source->As<RE::Actor>(), a_weapon);
}

bool AnimEventHandler::InstallHook(REL::Relocation<uintptr_t> a_relocation) 
{
    _ProcessAnimEvent_PC = a_relocation.write_vfunc(0x1, ProcessAnimEventPC);

    return true;
}

RE::BSEventNotifyControl AnimEventHandler::ProcessAnimEventPC([[maybe_unused]] RE::BSTEventSink<RE::BSAnimationGraphEvent>* a_sink,
    RE::BSAnimationGraphEvent* a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>* a_eventSource) 
{
    if (a_event->holder)
    {
        uint32_t eventHash = hash(a_event->tag.data(), a_event->tag.length());

        RE::Actor* actor = const_cast<RE::Actor*>(a_event->holder->As<RE::Actor>());

        if (actor) 
        {
            CrossbowReloadManager::GetSingleton()->HandleAnimEventPC(eventHash);
        }
    }

    return _ProcessAnimEvent_PC(a_sink, a_event, a_eventSource);
}

bool ClipGeneratorHandler::InstallHook(REL::Relocation<uintptr_t> a_relocation)
{
    _ProcessClipGeneratorUpdate = a_relocation.write_vfunc(0x05, ProcessClipGeneratorUpdate);

    return true;
}

void ClipGeneratorHandler::ProcessClipGeneratorUpdate(RE::hkbClipGenerator* a_clipGenerator, const RE::hkbContext& a_context, float a_timesteps)
{
    if (a_context.character)
    {
        auto graph = GetGraphFromCharacter(a_context.character);

        if (graph)
        {
            auto actor = graph->holder;

            if (actor)
            {
                int rightHandType = 0;
                
                graph->GetGraphVariableInt("iRightHandType"sv, rightHandType);

                if (rightHandType == 12)
                {
                    CrossbowReloadManager::GetSingleton()->HandleClipGeneratorUpdate(a_clipGenerator, graph, false);
                }
            }
            else
            {
                uint32_t projectNameHash = hash(graph->projectName.data(), graph->projectName.length());

                switch (projectNameHash)
                {
                case "CrossbowProject"_h:
                case "DwarvenCrossbowProject"_h:
                    CrossbowReloadManager::GetSingleton()->HandleClipGeneratorUpdate(a_clipGenerator, graph, true);
                    break;
                }
            }
        }
    }

    return _ProcessClipGeneratorUpdate(a_clipGenerator, a_context, a_timesteps);
}

[[nodiscard]] RE::BShkbAnimationGraph* GetGraphFromCharacter(RE::hkbCharacter* a_hkbCharacter)
{
    if (!a_hkbCharacter) { return nullptr; }

    return SKSE::stl::adjust_pointer<RE::BShkbAnimationGraph>(a_hkbCharacter, -0xC0);
}