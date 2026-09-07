#pragma once

namespace Hooks
{
    bool InstallHooks();
    bool InstallDataLoadedHooks();

    inline REL::Relocation<uintptr_t> arrow_release_handler { REL::RelocationID(41778, 42859), REL::Relocate(0x133, 0x138) };

    inline REL::Relocation<uintptr_t> AnimEventVtbl_PC { RE::VTABLE_PlayerCharacter[2] };
    inline REL::Relocation<uintptr_t> AnimEventVtbl_NPC{ RE::VTABLE_Character[2] };

    inline REL::Relocation<uintptr_t> vtblhkbClipGenerator { RE::VTABLE_hkbClipGenerator[0] };

    inline REL::Relocation<uintptr_t> AnimGraphVtbl_PC{ RE::VTABLE_PlayerCharacter[3] };
}