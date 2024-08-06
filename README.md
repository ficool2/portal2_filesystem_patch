# Portal 2 Filesystem Patch
Removes the restrictions on the filesystem in Portal 2 which do not allow writing outside of the game directory, which was added as part of a security update. Unfortunately as a side effect, this made it inconvenient for tools such as Hammer, as maps couldn't be compiled outside of the game folder (resulting in a `Can't create LogFile` error).

## Installation
Download the [WINMM.dll](https://github.com/ficool2/portal2_filesystem_patch/releases) and place it into Portal 2's `bin` folder (where Hammer is located). That's all you need to do. If successful, you should see `[FS_PATCH] Successfully patched unrestricted filesystem` in the console output, and files should now successfully write outside of the game folder.

The patch will only load in tools and it will not be loaded in Portal 2 itself, so this is safe to use for multiplayer.