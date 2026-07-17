#pragma once

static constexpr uint32_t hash(const char* data, const size_t size) noexcept
{
    uint32_t hash = 5381;

    for (const char* c = data; c < data + size; ++c)
    {
        hash = ((hash << 5) + hash) + (unsigned char)*c;
    }

    return hash;
}

constexpr uint32_t operator"" _h(const char* str, size_t size) noexcept
{
    return hash(str, size);
}

static inline void PlaySFX(RE::Actor* actor, RE::BGSSoundDescriptorForm* descriptor, RE::NiPoint3 position,
    float volume)
{
    if (!actor || !descriptor)
        return;

    auto* audioManager = RE::BSAudioManager::GetSingleton();
    if (!audioManager)
        return;

    RE::BSSoundHandle handle;
    audioManager->GetSoundHandle(handle, descriptor);
    if (!handle.IsValid())
        return;
    handle.SetPosition(position);
    handle.SetVolume(volume);
    handle.SetObjectToFollow(actor->Get3D());
    handle.Play();
}