#pragma once

class CrossbowReloadManager
{
public:

    static CrossbowReloadManager* GetSingleton();

    void HandleAnimEventPC(uint32_t a_eventHash);
    void HandleWeaponFire(RE::Actor* a_source, RE::TESObjectWEAP* a_weapon);
    void HandleClipGeneratorUpdate(RE::hkbClipGenerator* a_clipGenerator, RE::BShkbAnimationGraph* a_graph,
        bool a_isCrossbowWeapon);

    bool IsCrossbowLoaded() const;
private:

    void SetCrossbowLoaded(bool a_loaded);
    bool IsCrossbowEquipped() const;
    bool pendingRelease = false;

    void FinishRelease();
    void InterruptArrowAttach();
    void EvaluateDrawState();

    float GetReloadStaminaCost(RE::Actor* a_player);
    void CastStaminaDrainSpellPlayer();
    void CastStaminaDrainSpell(RE::Actor* a_actor, bool a_isPlayer);
};