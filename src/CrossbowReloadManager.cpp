#include "CrossbowReloadManager.h"
#include "Utility.h"
#include "Settings.h"

CrossbowReloadManager* CrossbowReloadManager::GetSingleton()
{
    static CrossbowReloadManager crossbowReloadManager;
    return &crossbowReloadManager;
}

void CrossbowReloadManager::PostLoadMaintenance()
{
    auto player = RE::PlayerCharacter::GetSingleton();
    if (IsCrossbowEquipped(player) && player->AsActorState()->actorState1.meleeAttackState == RE::ATTACK_STATE_ENUM::kNone)
    {
        auto camera = RE::PlayerCamera::GetSingleton();
        auto cameraState = camera->currentState.get();

        if (camera->IsInFirstPerson())
        {
            camera->ForceThirdPerson();
            SKSE::GetTaskInterface()->AddTask([=]()
            {
                camera->SetState(cameraState);
            });
        }
        else
        {
            camera->ForceFirstPerson();
            SKSE::GetTaskInterface()->AddTask([=]()
            {
                camera->SetState(cameraState);
            });
        }

        std::thread([=]()
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            SKSE::GetTaskInterface()->AddTask([=]()
            {
                EvaluateDrawState(player);
            });
        }).detach();
    }
}

bool CrossbowReloadManager::HandleAnimEventPC(RE::BSAnimationGraphEvent* a_event)
{
    if (!a_event->holder) { return false; }

    RE::Actor* player = const_cast<RE::Actor*>(a_event->holder->As<RE::Actor>());

    if (!player) { return false; }


    if (IsCrossbowEquipped(player))
    {
        uint32_t eventHash = hash(a_event->tag.data(), a_event->tag.length());

        switch (eventHash)
        {
        case "reload"_h:
        case "ReloadFast"_h:
            player->SetGraphVariableBool("IsAttacking"sv, true);
            player->AsActorState()->actorState1.meleeAttackState = RE::ATTACK_STATE_ENUM::kBowDraw;
            CastStaminaDrainSpellPC();
            break;
        case "reloadStop"_h:
            SetCrossbowLoaded(true);
            break;
        case "arrowAttach"_h:
        {
            if (!IsCrossbowLoaded())
            {
                bool isEquipping;
                player->GetGraphVariableBool("IsEquipping"sv, isEquipping);

                if (isEquipping)
                {
                    return true;
                }
            }
        }
            break;
        case "BowRelease"_h:
        case "BowReleaseFast"_h:
            SetCrossbowLoaded(false);
            break;
        case "bowReset"_h:
        case "BeginWeaponDraw"_h:
        case "attackStop"_h:
            EvaluateDrawState(player);
            break;
        case "SoundPlay"_h:
        {
            uint32_t payloadHash = hash(a_event->payload.data(), a_event->payload.length());

            if (payloadHash == "WPNCrossbowReload"_h)
            {
                bool isReloading;
                player->GetGraphVariableBool("IsReloading", isReloading);

                if (isReloading)
                {
                    auto settings = Settings::GetSingleton();

                    if (!player->HasPerk(settings->QuickShot))
                    {
                        PlaySFX(player, settings->MCR_WPNCrossbowReload, player->GetPosition(), 1);
                    }
                    else
                    {
                        PlaySFX(player, settings->MCR_WPNCrossbowReloadQuickShotPerk, player->GetPosition(), 1);
                    }
                }
                else
                {
                    player->NotifyAnimationGraph("attackStop"sv);
                }
            }
        }
            break;
        }
    }

    return false;
}

void CrossbowReloadManager::HandleNotifyAnimGraphPC(RE::IAnimationGraphManagerHolder* a_graphHolder, [[maybe_unused]] const RE::BSFixedString& a_eventName)
{
    RE::Actor* player = static_cast<RE::Actor*>(a_graphHolder);

    if (!player) { return; }


    if (IsCrossbowEquipped(player))
    {
        uint32_t eventHash = hash(a_eventName.data(), a_eventName.length());

        switch (eventHash)
        {
        case "attackStop"_h:
            EvaluateDrawState(player);
            break;
        }
    }
}

void CrossbowReloadManager::HandleAnimEventNPC([[maybe_unused]] RE::BSAnimationGraphEvent* a_event)
{
    if (!a_event->holder) { return; }

    RE::Actor* actor = const_cast<RE::Actor*>(a_event->holder->As<RE::Actor>());

    if (!actor) { return; }


    if (IsCrossbowEquipped(actor))
    {
        uint32_t eventHash = hash(a_event->tag.data(), a_event->tag.length());

        switch (eventHash)
        {
        case "attackRelease"_h:
            if (!Settings::GetSingleton()->isRequiemLoaded)
            {
                CastStaminaDrainSpell(actor);
            }
            break;
        case "SoundPlay"_h:
        {
            uint32_t payloadHash = hash(a_event->payload.data(), a_event->payload.length());

            if (payloadHash == "WPNCrossbowReload"_h)
            {
                auto settings = Settings::GetSingleton();

                if (!actor->HasPerk(settings->QuickShot))
                {
                    PlaySFX(actor, settings->MCR_WPNCrossbowReload, actor->GetPosition(), 1);
                }
                else
                {
                    PlaySFX(actor, settings->MCR_WPNCrossbowReloadQuickShotPerk, actor->GetPosition(), 1);
                }
            }
        }
            break;
        }
    }
}

void CrossbowReloadManager::HandleClipGeneratorUpdate(RE::hkbClipGenerator* a_clipGenerator, RE::BShkbAnimationGraph* a_graph)
{
    uint32_t generatorNameHash = hash(a_clipGenerator->name.data(), a_clipGenerator->name.length());
    auto actor = a_graph->holder;

    float quickDrawBonus = 0.0f;
    static float perkBonus = Settings::GetSingleton()->reloadSpeedPerkBonus;

    if (actor)
    {
        int rightHandType;
        a_graph->GetGraphVariableInt("iRightHandType"sv, rightHandType);

        if (rightHandType == 12)
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

            case "CrossBow_ReleaseFastPlayer"_h:
            case "CrossBow_ReleaseDwarvenFastPlayer"_h:
            case "SneakCrossBow_ReleaseFastPlayer"_h:
            case "SneakCrossBow_ReleaseDwarvenFastPlayer"_h:
                quickDrawBonus = perkBonus;
                break;

            default:
                return;
            }
        }
    }
    else
    {
        uint32_t projectNameHash = hash(a_graph->projectName.data(), a_graph->projectName.length());

        switch (projectNameHash)
        {
        case "CrossbowProject"_h:
        case "DwarvenCrossbowProject"_h:
            switch (generatorNameHash)
            {
            case "CrossBow Reload.hkx"_h:
                break;

            case "CrossBow Reload Fast"_h:
            case "ReleaseFast"_h:
                quickDrawBonus = perkBonus;
                break;

            default:
                return;
            }
            break;
        }
    }

    float weaponSpeedMult;
    a_graph->GetGraphVariableFloat("weaponSpeedMult"sv, weaponSpeedMult);

    a_clipGenerator->playbackSpeed = weaponSpeedMult + quickDrawBonus;
}

void CrossbowReloadManager::EvaluateDrawState(RE::Actor* a_player)
{
    if (IsCrossbowLoaded())
    {
        a_player->NotifyAnimationGraph("crossbowDrawn"sv);
    }
    else
    {
        a_player->NotifyAnimationGraph("crossbowUndrawn"sv);
        a_player->DetachArrow(a_player->AsReference()->GetBiped());
    }
}

bool CrossbowReloadManager::IsCrossbowEquipped(RE::Actor* a_actor) const
{
    auto weaponForm = a_actor->GetEquippedObject(false);
    if (!weaponForm) { return false; }
    if (!weaponForm->IsWeapon()) { return false; }

    auto weapon = weaponForm->As<RE::TESObjectWEAP>();
    if (!weapon) { return false; }

    return weapon->IsCrossbow();
}

bool CrossbowReloadManager::IsCrossbowLoaded() const
{
    auto player = RE::PlayerCharacter::GetSingleton();

    bool isCrossbowLoaded = false;
    player->GetGraphVariableBool("IsCrossbowLoaded"sv, isCrossbowLoaded);

    return isCrossbowLoaded;
}

void CrossbowReloadManager::SetCrossbowLoaded(bool a_loaded)
{
    auto player = RE::PlayerCharacter::GetSingleton();

    player->SetGraphVariableBool("IsCrossbowLoaded"sv, a_loaded);
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

void CrossbowReloadManager::CastStaminaDrainSpell(RE::Actor* a_actor)
{
    RE::SpellItem* spell;
    auto settings = Settings::GetSingleton();

    if (a_actor->IsPlayerRef())
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

void CrossbowReloadManager::CastStaminaDrainSpellPC()
{
    auto player = RE::PlayerCharacter::GetSingleton();

    if (!Settings::GetSingleton()->isRequiemLoaded)
    {
        if (!player->IsGodMode())
        {
            CastStaminaDrainSpell(player);
        }
    }
    else
    {
        player->AddSpell(Settings::GetSingleton()->CrossbowStaminaSpell);
    }
}