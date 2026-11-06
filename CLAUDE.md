# PugMod — MetaMod plugin for Counter-Strike 1.6

Repository: https://github.com/SmileYzn/PugMod (branch `main`)
Always reply to the user in Brazilian Portuguese (pt-BR). Files in the repository (code, comments, docs, commit messages) are written in English.

## Working files

- `CLAUDE.md` (this file): project instructions and context.
- `REVISION.md`: code review report and progress, item by item.
- Code in `PugMod/`; configs, translations and spawns in `cstrike/addons/pugmod/`.

## Stack

- C++14 (`-std=c++14` in the `Makefile`, `stdcpp14` in the `.vcxproj`). CI builds Linux with the GCC 7.5 from the `ubuntu:18.04` container: do not use features beyond C++14.
- MetaMod-R: https://github.com/rehlds/metamod-r
- ReHLDS 3.15.0.908+ and ReGameDLL_CS 5.30.0.831+ (GitHub Actions builds, not only releases)
- cURL (multi, non-blocking) and nlohmann/json 3.12.0 (`PugMod/include/json.hpp`)
- 32-bit binary: `pugmod_mm.so` (Linux) / `pugmod_mm.dll` (Windows)

## Build

### Linux

Arch (requires the `[multilib]` repository enabled in `/etc/pacman.conf`):

```sh
sudo pacman -S --needed base-devel lib32-gcc-libs lib32-glibc lib32-curl
cd PugMod
make clean && make prebuild linux32
```

Debian/Ubuntu: `sudo apt-get install build-essential gcc-multilib g++-multilib libcurl4-openssl-dev:i386`.

- Output: `PugMod/Release/pugmod_mm.so`.
- `make` with no target also runs `PostBuild.sh`, which kills hlds, copies the `.so` and restarts the test server. If the HLDS folder does not exist, the script just exits. To only build, use `make prebuild linux32`.
- Always do a clean build (`make clean` first): the Makefile does not track `.h` dependencies.
- The Makefile builds in parallel (`-j$(shell nproc)`) and generates the precompiled header `PugMod/precompiled.h.gch` (about 120 MB, ignored by git, removed by `make clean`). The SDK and Metamod are included with `-isystem`: the expected build has **0 warnings**. A new warning means a problem in the project's code.
- `precompiled.h` uses an include guard (`PUGMOD_PRECOMPILED_H`), not `#pragma once` (GCC 7 warns when generating the PCH).
- cURL is linked dynamically (`-Wl,--as-needed -lcurl -ldl -lm`). The `.so` depends only on `libcurl.so.4`, `libdl`, `libm`, `libc` and `ld-linux`; `libstdc++` and `libgcc` are static.
- `-Wl,--exclude-libs,ALL` avoids exporting the static libstdc++ symbols (collisions with other modules), and `-Wl,--gc-sections` drops unused code. Check with `nm -D --defined-only Release/pugmod_mm.so | wc -l` (about 57).
- The binary requires the glibc of the machine where it was built. A build on a recent distro is for local testing only. To distribute, use the CI artifact or the container (result: GLIBC 2.4 at most):

```sh
docker run --rm -v "$PWD":/desktop ubuntu:18.04 bash -c "dpkg --add-architecture i386 && apt-get update > /dev/null && apt-get install -qq -y build-essential gcc-multilib g++-multilib libcurl4-openssl-dev:i386 > /dev/null && cd /desktop/PugMod && make prebuild linux32"
```

- CPU flags: `-march=i686 -mtune=generic -msse2 -msse3` (no SSE4, to avoid "illegal instruction" on old CPUs).

### Windows

```powershell
& "C:\Program Files\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\MSBuild.exe" PugMod\PugMod.vcxproj -property:configuration=Release -property:platform=Win32 -nologo -verbosity:minimal
```

- Toolset v143, Win32. cURL is static (`PugMod/include/curl/lib/libcurl_a.lib`).
- Pending (B5 in `REVISION.md`): set `RuntimeLibrary` to `MultiThreaded` (`/MT`, Release) and `MultiThreadedDebug` (`/MTd`, Debug). This removes the VC++ Redistributable dependency and the `LNK4098 LIBCMT` warning. Check with `dumpbin /dependents Release\pugmod_mm.dll` (no `VCRUNTIME140.dll`).

CI (GitHub Actions, `.github/workflows/build.yml`) builds Linux and Windows on every push.

## Architecture

- `MetaMod.cpp`: dispatches engine events (ServerActivate/Deactivate, StartFrame, ClientCommand, Message*) to the modules.
- `ReGameDLL.cpp`: ReGameDLL hookchains. `ReAPI.cpp`: ReHLDS `SV_DropClient`.
- Each module is a `CPug*` class with a global `gPug*` instance (e.g. `gPugVoteMap`).
- A new module with `StartFrame`/`ServerActivate`/`ServerDeactivate` must be registered in `MetaMod.cpp`, and its `DropClient` in `ReAPI.cpp`.
- `precompiled.h` includes every project header.
- A state change (`CPugMod::SetState`) from code that is already running inside a `SetState` must be scheduled with `gPugTask.Create(E_TASK::SET_STATE, ...)`, never called directly.
- Configs, translations and spawns: `cstrike/addons/pugmod/` (`cfg/language.cfg`, `cfg/clientcmd.cfg`, `spawns/`).

## Code conventions

- Code (`.cpp`, `.h`) and config files (`.cfg`): always TAB, with a tab width of 4 (see `.editorconfig`). Never indent or align with spaces.
  - Code: braces on the next line (Allman), one blank line between statements.
  - Config: values aligned with TABs in each block of `name value` lines; single-space lines stay as they are.
  - Vendored code in `PugMod/include/` keeps its own style.
- Line endings: the repository stores LF; with `core.autocrlf=true` (Windows) the working copy is CRLF. Do not commit line ending changes.
- `this->` prefix on members; PascalCase variables.
- Texts shown to players use `_T("...")` and need an entry in `cfg/language.cfg` (UTF-8), with the key identical to the text in the code, plus `en:`, `ru:` and `br:`.

## Safety rules learned in the review

- Use standard `snprintf`/`vsnprintf`, never `Q_snprintf`/`Q_vsnprintf`/`_snprintf` (on MSVC they do not terminate the string when truncating).
- A player name or any external text never goes in as the format: use `AddItem(..., "%s", name)`, `SendHud(..., "%s", text)`.
- When appending to a buffer: `snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf), ...)`, or use `std::string`.
- Always check `if (Player)` after `UTIL_PlayerByIndexSafe` (private data can be null for a player who is still connecting).
- `RANDOM_LONG(low, high)` includes `high`: for an index, use `size() - 1`.
- No "draw until found" loops: filter candidates first and pick among them.
- JSON: `dump(-1, ' ', false, nlohmann::json::error_handler_t::replace)` (a non-UTF-8 name throws an exception and crashes the server).
- Exceptions must not escape engine callbacks. Use `try/catch` only where there is external data (JSON, `std::stoi` from config), not as a generic wrapper.
- Menu handlers recheck permission (`gPugAdmin.CheckAccess`), state (e.g. `m_Run`), index bounds (`Item.Info < size()`) and whether the player takes part in the vote (`m_Voter[]`).
- `gPugUtil.GetPlayers()` (both versions) ignores HLTV (`FL_PROXY`); custom loops over `maxClients` must do the same.
- No MSVC-only extensions (e.g. `std::string::_Copy_s`): the code must build with g++.

## Workflow

- Work follows `REVISION.md`, item by item (codes B = build, A = high, M = medium, L = low).
- Usually the user applies the fixes. Claude shows the fix, then validates it (`git diff` plus a clean build) and marks the item as `[x]` in `REVISION.md`.
- Only edit the code directly when the user asks.
- Do not commit or push unless the user asks.
