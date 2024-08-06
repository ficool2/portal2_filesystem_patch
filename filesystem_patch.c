/* by ficool2 */

#include <windows.h>
#include <Psapi.h>

void (*msg)(const char* fmt, ...) = NULL;

static char* find_signature(const MODULEINFO* info, const char* sig, size_t len)
{
    uintptr_t start = (uintptr_t)info->lpBaseOfDll;
    uintptr_t end = start + info->SizeOfImage - len;
    for (size_t i = start; i < end; i++)
    {
        BOOL found = TRUE;
        for (size_t j = 0; j < len; j++)
            found &= sig[j] == *(char*)(i + j);
        if (found)
            return (char*)i;
    }
    return NULL;
}

static void __declspec(naked) hook_fs_openforwrite(void)
{
    __asm
    {
        mov al, 1
        retn 4
    }
}

static BOOL init()
{
    static BOOL has_init = FALSE;
    if (has_init)
        return TRUE;
    has_init = TRUE;

    /* do not hijack the game */
    char exe_name[MAX_PATH];
    GetModuleFileNameA(NULL, exe_name, sizeof(exe_name));
    if (strstr(exe_name, "portal2.exe"))
        return TRUE;

    /* only hijack source tools */
    HMODULE tier0 = GetModuleHandle("tier0.dll");
    HMODULE filesystem = GetModuleHandle("filesystem_stdio.dll");
    if (!tier0 || !filesystem)
        return TRUE;

    msg = (void*)GetProcAddress(tier0, "Msg");
    if (!msg)
        return TRUE;

    MODULEINFO filesystem_info = { 0 };
    GetModuleInformation(GetCurrentProcess(), filesystem, &filesystem_info, sizeof(filesystem_info));
    char* find = find_signature(&filesystem_info, "\x8D\x85\xF4\xFE\xFF\xFF\x50", 7);
    if (!find)
    {
        msg("[FS_PATCH] Failed to find file write signature\n");
        return TRUE;
    }
    find += 0xA;

    DWORD prev_protect, dummy_protect;
    VirtualProtect(find, 4, PAGE_EXECUTE_READWRITE, &prev_protect);
    *(uintptr_t*)find = (uintptr_t)&hook_fs_openforwrite - (uintptr_t)find - 4;
    VirtualProtect(find, 4, prev_protect, &dummy_protect);

    msg("[FS_PATCH] Successfully patched unrestricted filesystem\n");
    return TRUE;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
    if (dwReason == DLL_THREAD_ATTACH)
        return init();
    return TRUE;
}