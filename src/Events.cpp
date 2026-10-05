#include "Events.h"
#include "CrossbowReloadManager.h"

bool AnimEventHandlerPC::InstallHook(REL::Relocation<uintptr_t> a_relocation) 
{
    _ProcessAnimEvent_PC = a_relocation.write_vfunc(0x1, ProcessAnimEventPC);

    return true;
}

RE::BSEventNotifyControl AnimEventHandlerPC::ProcessAnimEventPC([[maybe_unused]] RE::BSTEventSink<RE::BSAnimationGraphEvent>* a_sink,
    RE::BSAnimationGraphEvent* a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>* a_eventSource) 
{
    bool shouldInterruptEvent = CrossbowReloadManager::GetSingleton()->HandleAnimEventPC(a_event);
    if (shouldInterruptEvent)
    {
        RE::BSAnimationGraphEvent event = { "MCR_fakeEvent"sv, a_event->holder, nullptr };
        return _ProcessAnimEvent_PC(a_sink, &event, a_eventSource);
    }

    return _ProcessAnimEvent_PC(a_sink, a_event, a_eventSource);
}

bool AnimEventHandlerNPC::InstallHook(REL::Relocation<uintptr_t> a_relocation)
{
    _ProcessAnimEvent_NPC = a_relocation.write_vfunc(0x1, ProcessAnimEventNPC);

    return true;
}

RE::BSEventNotifyControl AnimEventHandlerNPC::ProcessAnimEventNPC([[maybe_unused]] RE::BSTEventSink<RE::BSAnimationGraphEvent>* a_sink,
    RE::BSAnimationGraphEvent* a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>* a_eventSource)
{
    CrossbowReloadManager::GetSingleton()->HandleAnimEventNPC(a_event);

    return _ProcessAnimEvent_NPC(a_sink, a_event, a_eventSource);
}

bool NotifyAnimGraphHandlerPC::InstallHook(REL::Relocation<uintptr_t> a_relocation)
{
    _NotifyAnimGraph_PC = a_relocation.write_vfunc(0x1, NotifyAnimGraphPC);

    return true;
}

void NotifyAnimGraphHandlerPC::NotifyAnimGraphPC(RE::IAnimationGraphManagerHolder* a_graphHolder, const RE::BSFixedString& a_eventName)
{
    CrossbowReloadManager::GetSingleton()->HandleNotifyAnimGraphPC(a_graphHolder, a_eventName);

    return _NotifyAnimGraph_PC(a_graphHolder, a_eventName);
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

        if (graph && a_clipGenerator)
        {
            CrossbowReloadManager::GetSingleton()->HandleClipGeneratorUpdate(a_clipGenerator, graph);
        }
    }

    return _ProcessClipGeneratorUpdate(a_clipGenerator, a_context, a_timesteps);
}

RE::BShkbAnimationGraph* GetGraphFromCharacter(RE::hkbCharacter* a_hkbCharacter)
{
    if (!a_hkbCharacter) { return nullptr; }

    return SKSE::stl::adjust_pointer<RE::BShkbAnimationGraph>(a_hkbCharacter, -0xC0);
}