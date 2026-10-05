#pragma once

class CrossbowReloadManager
{
    public:

        static CrossbowReloadManager* GetSingleton();

        void PostLoadMaintenance();

        bool HandleAnimEventPC(RE::BSAnimationGraphEvent* a_event);
        void HandleAnimEventNPC(RE::BSAnimationGraphEvent* a_event);
        void HandleNotifyAnimGraphPC(RE::IAnimationGraphManagerHolder* a_graphHolder, const RE::BSFixedString& a_eventName);
        void HandleClipGeneratorUpdate(RE::hkbClipGenerator* a_clipGenerator, RE::BShkbAnimationGraph* a_graph);

    private:

        bool IsCrossbowLoaded() const;
        void SetCrossbowLoaded(bool a_loaded);
        bool IsCrossbowEquipped(RE::Actor* a_actor) const;
        void EvaluateDrawState(RE::Actor* a_player);

        float GetReloadStaminaCost(RE::Actor* a_player);
        void CastStaminaDrainSpell(RE::Actor* a_actor);
        void CastStaminaDrainSpellPC();
};