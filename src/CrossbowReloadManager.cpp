#include "CrossbowReloadManager.h"
#include "Utility.h"
#include "Settings.h"

CrossbowReloadManager* CrossbowReloadManager::GetSingleton()
{
    static CrossbowReloadManager crossbowReloadManager;
    return &crossbowReloadManager;
}

void CrossbowReloadManager::HandleAnimEventPC(RE::BSAnimationGraphEvent* a_event)
{
    uint32_t eventHash = hash(a_event->tag.data(), a_event->tag.length());

    switch (eventHash)
    {
    case "reload"_h:
    case "ReloadFast"_h:
    {
        auto player = RE::PlayerCharacter::GetSingleton();
        player->SetGraphVariableBool("IsAttacking"sv, true);
        player->AsActorState()->actorState1.meleeAttackState = RE::ATTACK_STATE_ENUM::kBowDraw;
        pendingReloadSoundPlay = true;
        CastStaminaDrainSpellPlayer();
    }
        break;
    case "reloadStop"_h:
        SetCrossbowLoaded(true);
        break;
    case "arrowAttach"_h:
        InterruptArrowAttach(false);
        break;
    case "SoundPlay"_h:
    {
        uint32_t payloadHash = hash(a_event->payload.data(), a_event->payload.length());

        if (payloadHash == "WPNCrossbowReload"_h)
        {
            auto player = RE::PlayerCharacter::GetSingleton();

            if (pendingReloadSoundPlay)
            {
                PlaySFX(player, Settings::GetSingleton()->MCR_WPNCrossbowReloadPlayer, player->GetPosition(), 1);
                PlaySFX(player, Settings::GetSingleton()->MCR_WPNCrossbowReloadQuickShotPerkPlayer, player->GetPosition(), 1);
            }
            else
            {
                player->NotifyAnimationGraph("attackStop"sv);
            }
            pendingReloadSoundPlay = false;
        }
    }
        break;
    case "arrowRelease"_h:
        if (IsCrossbowEquipped())
        {
           SetCrossbowLoaded(false);
        }
        break;
    case "attackStop"_h:
    case "BeginWeaponDraw"_h:
    case "bowReset"_h:
    case "JumpUp"_h:
    case "JumpFall"_h:
    case "JumpFallDirectional"_h:
        EvaluateDrawState();
        break;
    /*
    case "DisableBumper"_h:
        if (IsCrossbowEquipped())
        {
            if (!IsCrossbowLoaded())
            {
                auto player = RE::PlayerCharacter::GetSingleton();

                InterruptArrowAttach(true);
                player->NotifyAnimationGraph("reloadStop"sv);

            }
        }
        EvaluateDrawState();
        break;
    */
    }
    
}

void CrossbowReloadManager::EvaluateDrawState()
{
    if (IsCrossbowEquipped())
    {
        auto player = RE::PlayerCharacter::GetSingleton();
        if (IsCrossbowLoaded())
        {
            player->NotifyAnimationGraph("forceBowDrawn"sv);
        }
        else
        {
            player->NotifyAnimationGraph("forceBowUndrawn"sv);
        }
    }
}

void CrossbowReloadManager::HandleClipGeneratorUpdate(RE::hkbClipGenerator* a_clipGenerator, RE::BShkbAnimationGraph* a_graph,
    bool a_isCrossbowWeapon)
{
    if (a_clipGenerator)
    {
        uint32_t generatorNameHash = hash(a_clipGenerator->name.data(), a_clipGenerator->name.length());
        float quickDrawBonus = 0.0f;

        if (!a_isCrossbowWeapon)
        {
            switch (generatorNameHash)
            {
            case "CrossBow_Reload"_h:
            case "CrossBow_ReloadDwarven"_h:
            case "SneakCrossBow_Reload"_h:
            case "SneakCrossBow_ReloadDwarven"_h:
                break;

            case "CrossBow_ReloadFast"_h:
            case "CrossBow_ReloadDwarvenFast"_h:
            case "SneakCrossBow_ReloadFast"_h:
            case "SneakCrossBow_ReloadDwarvenFast"_h:

            case "CrossBow_ReleaseFast"_h:
            case "CrossBow_ReleaseDwarvenFast"_h:
            case "SneakCrossBow_ReleaseFast"_h:
            case "SneakCrossBow_ReleaseDwarvenFast"_h:
                quickDrawBonus = Settings::GetSingleton()->reloadSpeedPerkBonus;
                break;

            default:
                return;
            }
        }
        else
        {
            switch (generatorNameHash)
            {
            case "CrossBow Reload Fast"_h:
            case "ReleaseFast"_h:
                quickDrawBonus = Settings::GetSingleton()->reloadSpeedPerkBonus;
                break;

            default:
                return;


            }
        }

        float weaponSpeedMult = 1.0f;
        a_graph->GetGraphVariableFloat("weaponSpeedMult"sv, weaponSpeedMult);

        // logger::debug("weaponSpeedMult var = {}, reloadSpeedPerkBonus = {}", weaponSpeedMult, quickDrawBonus);
        a_clipGenerator->playbackSpeed = weaponSpeedMult + quickDrawBonus;
    }

}

void CrossbowReloadManager::HandleWeaponFire(RE::Actor* a_actor, RE::TESObjectWEAP* a_weapon)
{
    if (!a_actor->IsPlayerRef() && a_weapon->IsCrossbow())
    {
        CastStaminaDrainSpell(a_actor, false);
    }
}

bool CrossbowReloadManager::IsCrossbowEquipped() const
{
    auto player = RE::PlayerCharacter::GetSingleton();

    auto weaponForm = player->GetEquippedObject(false);
    if (!weaponForm) { return false; }
    if (!weaponForm->IsWeapon()) { return false; }

    auto weapon = weaponForm->As<RE::TESObjectWEAP>();
    if (!weapon) { return false; }

    return weapon->IsCrossbow();
}


bool CrossbowReloadManager::IsCrossbowLoaded() const
{
    return Settings::GetSingleton()->MCR_IsCrossbowLoaded->value;
}

void CrossbowReloadManager::SetCrossbowLoaded(bool a_loaded)
{
    Settings::GetSingleton()->MCR_IsCrossbowLoaded->value = a_loaded;
}

void CrossbowReloadManager::InterruptArrowAttach(bool a_bypassCheck)
{
    auto player = RE::PlayerCharacter::GetSingleton();

    if (IsCrossbowEquipped() && !IsCrossbowLoaded())
    {
        bool isEquipping = false;
        player->GetGraphVariableBool("IsEquipping"sv, isEquipping);
        bool isJumping = false;
        
        switch (player->GetCharController()->wantState)
        {
        case RE::hkpCharacterStateType::kJumping:
        case RE::hkpCharacterStateType::kInAir:
            isJumping = true;
        }

        if (isEquipping || isJumping || a_bypassCheck)
        {
            auto equipManager = RE::ActorEquipManager::GetSingleton();
            auto equippedAmmo = player->GetCurrentAmmo();

            if (equippedAmmo)
            {
                equipManager->UnequipObject(player, equippedAmmo, nullptr, 1U, nullptr, false, false, false, true);
                equipManager->EquipObject(player, equippedAmmo, nullptr, 1U, nullptr, true, false, false, false);
            }
        }
    }
}

float CrossbowReloadManager::GetReloadStaminaCost(RE::Actor* a_actor)
{
    if (!a_actor)
    {
        return 0.0f;
    }

    RE::TESObjectWEAP* weapon = nullptr;
    RE::TESForm* weaponForm = a_actor->GetEquippedObject(false);

    if (weaponForm)
    {
        weapon = weaponForm->As<RE::TESObjectWEAP>();
    }

    if (!weapon)
    {
        return 0.0f;
    }

    auto settings = Settings::GetSingleton();

    float cost = settings->staminaCostBase;

    if (settings->weightIncreasesStaminaCost)
    {
        cost = cost + (weapon->weight);
    }

    if (settings->skillDecreasesStaminaCost)
    {
        auto* avOwner = a_actor->AsActorValueOwner();
        float skill = avOwner->GetActorValue(RE::ActorValue::kArchery);
        if (skill > 100.0f) 
        {
            skill = 100.0f;
        }

        cost = cost * (1.0f - (skill / 200.0f));
    }

    cost = cost * (settings->staminaCostMult);

    if (cost < 0.0f) 
    {
        cost = 0.0f;
    }

    logger::debug("Final Stamina drain cost = {}", cost);

    return cost;
}

void CrossbowReloadManager::CastStaminaDrainSpell(RE::Actor* a_actor, bool a_isPlayer)
{
    RE::SpellItem* spell;
    auto settings = Settings::GetSingleton();

    if (a_isPlayer)
    {
        spell = settings->CrossbowStaminaSpell;
    }
    else
    {
        spell = settings->CrossbowStaminaSpellNPC;
    }

    float cost = 0.0f;

    if (!settings->isBladeAndBluntLoaded)
    {
        cost = GetReloadStaminaCost(a_actor);
    }

    a_actor->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant)
        ->CastSpellImmediate(spell, false, a_actor, 1.0f, false, cost, nullptr);
}

void CrossbowReloadManager::CastStaminaDrainSpellPlayer()
{
    auto player = RE::PlayerCharacter::GetSingleton();

    if (!Settings::GetSingleton()->isRequiemLoaded)
    {
        if (!player->IsGodMode())
        {
            CastStaminaDrainSpell(player, true);
        }
    }
    else
    {
        player->AddSpell(Settings::GetSingleton()->CrossbowStaminaSpell);
    }
}