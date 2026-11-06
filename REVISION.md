# PugMod — Code review

Review started on 2026-11-03 (commit `59a9487`), redone on 2026-11-05 (commit `2cea70d`) and checked again on 2026-11-05 (commit `a59a6d3`). The 2026-11-05 fixes are in the commit that added this file.

Legend:
- ✔ = checked directly in the code.
- No ✔ = found by agent review (read in the code, not tested on a server).
- 🆕 = item found in the 2026-11-05 reviews.
- `[x]` = done and validated (`git diff` plus a clean build); `[-]` = skipped by the maintainer's decision.

**Validation build:**
- `ubuntu:18.04` container (GCC 7.5, same environment as CI), `make prebuild linux32`: OK. The `.so` requires at most GLIBC_2.4 and depends only on `libcurl.so.4`, `libdl`, `libm`, `libc` and `ld-linux`.
- Arch (g++ 16): OK, local testing only.
- Windows (MSBuild, VS 18 Community, toolset v143): OK, only the `LNK4098 LIBCMT` warning (B5) (checked on commit `a59a6d3`).
- Warnings: none on Linux (after B9, L11 and L12).

---

## Summary

| Group | Done | Pending |
|---|---|---|
| Build (B) | B1, B2, B3, B6, B8, B9 (B4, B7 skipped) | B5, B10, B11, B12 🆕 |
| High (A) | A1–A15 | — |
| Medium (M) | M1–M12, M14, M15, M21–M23, M28, M29, M31, M37 | M13, M16–M20, M24–M27, M30, M32–M36 🆕 |
| Low (L) | L2, L4, L5, L8, L11–L15 | L1, L3, L6, L7, L9, L10 🆕 |

M7, M8, M9 and M10 were done in commits `c55a316` and `2cea70d`; the other checked items, in the 2026-11-05 fixes.

---

## 1. Build / requirements

- [x] **B1 ✔** C++14 standard in the `Makefile` (`-std=c++14`) and in the `.vcxproj` (`stdcpp14`). CI now uses the GCC 7.5 from Ubuntu 18.04 (GCC 4.8 was removed) (validated 2026-11-05).
- [x] **B2** `Makefile`: `-O0` replaced by `-O2`; `-fno-stack-protector` removed.
- [x] **B3 ✔** cURL explicitly linked as a shared library: `-Wl,--as-needed -lcurl -ldl -lm`, without `-lssl -lcrypto -lz` and without `-DCURL_STATICLIB`. The `.so` now depends only on `libcurl.so.4` (`CURL_OPENSSL_4` symbols), `libdl`, `libm`, `libc` and `ld-linux` (validated 2026-11-05).
  - README updated: server requirements (`libcurl4:i386` or `lib32-curl`, plus `ca-certificates`), C++14 and build packages.
- [-] **B4** ~~`make` with no target runs `PostBuild.sh`~~ — **skipped** (user's decision): without HLDS in the folder, the script just exits.
- [ ] **B5** MSBuild: `LNK4098 LIBCMT`. Release uses `MultiThreadedDLL` (/MD) with `libcurl_a.lib` (probably built with /MT). Switch to `MultiThreaded` (/MT) or use a /MD libcurl. Cannot be tested on Linux.
- [x] **B6 🆕 ✔** glibc and CPU compatibility. Built in the `ubuntu:18.04` container (same environment as CI, GCC 7.5), the `.so` requires at most **GLIBC_2.4**, does not depend on `libstdc++` and has no SSE4 instructions (`-march=i686 -msse2 -msse3` instead of `-msse4.1 -msse4.2`). The local Arch build (GLIBC 2.43) is for testing only; to distribute, use the CI artifact or the container (validated 2026-11-05).
- [-] **B7** ~~No header dependency tracking~~ — **skipped** (user's decision): builds are always clean.
- [x] **B8 🆕 ✔** `Makefile`: with `-static-libstdc++`, the `.so` exported 4,221 symbols (3,486 from libstdc++, such as `std::string` and `__cxa_*`), which could collide with other modules in the same process. It also carried unused libstdc++ code. Fixed with `-Wl,--exclude-libs,ALL -Wl,--gc-sections`. In the CI container: from 1.89 MB to **1.08 MB** and 57 exported symbols (Metamod entry points, `CreateInterface` and a few standard library templates), with the same dependencies and GLIBC_2.4 (validated 2026-11-05).
- [x] **B9 🆕 ✔** Slow build (60 s serial) with 828 warnings, almost all from SDK headers. Each `.cpp` re-parsed `precompiled.h` (json.hpp plus the SDK, about 1 s per file). Fixed in the `Makefile` (validated 2026-11-05):
  - Parallel build (`-j$(shell nproc)`) with guaranteed order: objects depend on `prebuild` (order-only) and `posbuild` depends on the link.
  - Real precompiled header (`precompiled.h.gch`, generated once; `*.gch` in `.gitignore`, removed by `make clean`). The `#pragma once` in `precompiled.h` became an include guard, because GCC 7 warns "#pragma once in main file".
  - `-isystem` for Metamod and CSSDK: warnings from inside the SDK no longer show up.
  - Result: 60 s → 19 s on Arch (4 cores) and 16 s in the CI container, **0 warnings**. The `.so` is the same (1.08 MB, 57 exported symbols, GLIBC_2.4).
- [ ] **B10 🆕** `PugMod.vcxproj:61, 83`: `PrecompiledHeader` is set to `Create` at project level, so every `.cpp` recreates the PCH. It should be `Use` at project level and `Create` only on `precompiled.cpp` (which already has `Create` on lines 103-104). Do it together with B5, on Windows.
- [ ] **B11 🆕 ✔** `PostBuild.sh:16, 47`: with several network interfaces (e.g. Docker bridges, 172.17.x/172.18.x/172.19.x), `ipAddress` gets several IPs, one per line, and `eval` breaks ("192.168.100.182: command not found"). The script has already killed hlds and copied the `.so` before failing, so the server does not start. Fix: take only the first IP (`... | head -n1`) or the default route IP (`ip -4 route get 1.1.1.1 | grep -oP '(?<=src )[\d.]+'`).
- [ ] **B12 🆕 ✔** `PostBuild.ps1:28`: same problem as B11 on Windows. With Ethernet and Wi-Fi both `Preferred`, `$hldsIP` becomes an array of IPs. Fix: `Select-Object -First 1`. The HLDS path is also hardcoded in the versioned script.

---

## 2. High priority — crash, server hang or stuck match

- [x] **A1** `PugCurl.cpp` — `curl_multi_perform` in a loop blocked the frame.
- [x] **A2** `PugClientCmd` — `nullptr` in the blocked commands array. (See suggestion S1.)
- [x] **A3** Format string with player name (AdminMenu, Leader, RoundStats).
- [x] **A4** `Q_snprintf` with the wrong size, and general switch to `snprintf`/`vsnprintf`.
- [x] **A5 ✔** `PugDM.cpp:109` — `RANDOM_LONG(0, size() - 1)`.
- [x] **A6 ✔** `PugDM.cpp:740` — `EquipRandomItem` now picks among candidates.
- [x] **A7** `PugStatus.cpp` — `dump()` with `error_handler_t::replace`.
- [x] **A8** `PugRageBan.cpp:13` — null check.
- [x] **A9** `PugLeader.cpp` — `std::string(5 - Count, '\n')`.
- [x] **A10** `PugVoteMap.cpp` — infinite loop in `GetRandom()`.

### New

- [x] **A11 🆕 ✔** `PugLeader.cpp:31` — infinite loop in the captain draw with 1 player. Fixed: `Init()` requires `Players.size() >= 2 && pug_players_min >= 2` (validated 2026-11-05, build OK). (See M37.)
- [x] **A12 🆕 ✔** Vote handlers (`PugVoteEnd`, `PugVoteOvertime`, `PugVoteSwapTeam`, `PugVoteTeam`, `PugVoteMap`) used `m_VoteList[Item.Info]` without a bounds check. Fixed with an index bounds check and with `m_Voter[]`, set in `Init()` for participants and cleared in `ServerActivate`/`ServerDeactivate` (validated 2026-11-05, clean build OK).
- [x] **A13 🆕 ✔** `PugLeader.cpp` — the captain flow could get stuck in STATE_CAPTAIN. Fixed (validated 2026-11-05, clean build OK):
  - `Init()`: the `else` schedules the knife round or the first half (together with A11). (See M37.)
  - `Next()`: always ends in `Stop()` when no menu is opened.
  - `GetPlayer()`: auto-pick with no spectators calls `Stop()`; with spectators, auto-pick now picks at random.
- [x] **A14 🆕 ✔** `PugUtil.cpp:815` — `GetPlayers()` (per-team array) now ignores `FL_PROXY`. HLTV is no longer part of the pick, the vote kick or the Status counts (validated 2026-11-05).
  - The pick HUD (`PugLeader.cpp:395`, `StartFrame`) also ignores HLTV.
- [x] **A15 🆕 ✔** `PugMod.cpp:236` — `m_ScoreOT` was not reset when a new overtime started (OT2 lasted 1 round). Fixed (validated 2026-11-05, build OK):
  - When entering STATE_OVERTIME, it is reset if the sum is `>= pug_rounds_ot` (previous OT finished). The second half of the OT and Sudden Death keep the score. (See L13.)
  - Explicit `case 3` (Sudden Death) in both `pug_ot_type` `switch` blocks: the match continues and the next round decides.

---

## 3. Medium priority — logic and security

- [x] **M1** `PugVoteOvertime.cpp` — menu with the wrong `E_MENU`.
- [x] **M2** `MetaMod.cpp` — VoteEnd/VoteOvertime without StartFrame/ServerActivate.
- [x] **M3** `PugMenu` — menus not reset on disconnect; handlers recheck access.
- [x] **M4** `ReAPI.cpp` — `CPugVoteMenu::DropClient` is now called.
- [x] **M5** `PugTeamManager.cpp` — `TERRORIST || CT`.
- [x] **M6** `PugVoteMenu.cpp` — `Players.size()` of a fixed-size array.
- [x] **M7 ✔** `PugMapList.cpp:16` — list limited to `MAX_VOTE_MAPS`; keys are `0..size-1`, so `m_VoteMap[...][MAX_VOTE_MAPS + 1]` no longer overflows.
- [x] **M8 ✔** `PugUtil.cpp:88, 327` — `vsnprintf(..., sizeof - 1)` leaves room for `strcat("\n")`. The `"%s\n"` echo in `PugClientCmd.cpp:142` was also fixed.
- [x] **M9 ✔** `PugCurl.cpp` — `curl_slist` stored in `P_CURL_MEMORY::Headers` and freed on `CURLMSG_DONE`; `CURLOPT_PRIVATE` via `(void *)(intptr_t)`.
- [x] **M10 ✔** `PugCurl.cpp:190` — `ServerExecute` now requires `pug_api_execute 1` (default 0) and an HTTPS final URL, plus `CURLOPT_MAXREDIRS 3`. Documented in the README and in `pugmod.cfg`.
- [x] **M11** `PugStatus.cpp` — `m_Data["Players"]` cleared.
- [x] **M12 ✔** `PugStats.cpp` — `Stats[STATE_OVERTIME]` was reset on every entry into OVERTIME (by `SetState` and by the LO3 `RestartRound`); only the last half of the last OT went into the JSON. Fixed (validated 2026-11-05, build OK):
  - OT accumulates across all overtimes and is reset at the start of the match (FIRST_HALF).
  - `RestartRound` no longer resets OT.
  - `SetAnimation`, `TakeDamage` and `SendDeathMessage` only count with `m_bGameStarted`, which ignores the LO3.
- [ ] **M13 ✔** `PugStats.cpp:40-60` — `m_Player` is only cleared in STATE_DEAD and in ServerActivate.
  - On END → VOTEMAP/DEATHMATCH on the same map, players from the previous match stay in the next match's JSON.
  - Those players carry the old Team, Winner and JoinGameTime, and also the old `Stats[OVERTIME]` if the new match has no OT.
- [x] **M14** `PugStats.cpp` — JSON with `replace` and `ofstream` checked.
- [x] **M15 ✔** `PugStats.cpp:1278` — `Result.Survives +=` (validated 2026-11-05).
- [ ] **M16 ✔** (description revised) `PugStats.cpp:392-406` — the trade test (`Victim.KillTime <= 3s`) is **nested** inside `if (Killer.KillTime > 0)`. A trade only counts if the killer had already killed someone that round. Move the trade block out of that `if`.
- [ ] **M17 ✔** `PugSpawnEdit.cpp:477` — `Index = size() + 1` can overwrite an existing spawn (keys are not contiguous after deletions). `:493-499` compares `m_Spawns[EntityIndex]` (the player's index) instead of the marked spawn.
- [ ] **M18 ✔** `PugSpread.cpp:35` — `Q_fabs(m_flNextPrimaryAttack) > 0.0f` is almost always true and zeroes the spread. There is also no cvar and no state check: it applies even in the first and second halves. Suggestion: `pug_spread_fix` cvar (default 0) plus a state check.
- [ ] **M19 ✔** `PugRestrictItem.cpp:24` — `m_RestrictItem->string[Item]` without checking `strlen`.
- [ ] **M20 ✔** `PugRageBan.cpp` — `m_Data` is not cleared on map change.
  - `CheckTime` uses the previous map's `gpGlobals->time`.
  - `banid` with a generic authid (`STEAM_ID_LAN`, `STEAM_ID_PENDING`, `VALVE_ID_LAN`) bans everyone with the same ID.
  - Messages do not go through `_T()`, so they are not translated.

### New

- [x] **M21 🆕 ✔** `PugVoteMap.cpp:41` — with an empty maplist, the switch to VOTETEAM is now scheduled with `gPugTask.Create(SET_STATE, 1.0f, ...)`, with no recursive `SetState`. The translation of the optional message "Map list is empty, skipping map vote." is in `language.cfg` (validated 2026-11-05).
- [x] **M22 🆕 ✔** `PugVoteMap.cpp` — an empty map menu (every map was the current or the last one) caused a vote loop. Fixed: `LoadMaps()` does not load the current or the last map; with an empty list, it goes to VOTETEAM with the message "No maps available to vote, skipping map vote." (translated in `language.cfg`). `This`/`Last` removed, `GetRandom()` simplified (validated 2026-11-05, clean build OK).
- [x] **M23 🆕 ✔** `PugAdmin.cpp` — `Access(..., ADMIN_ADMIN)` by index was always false (everyone gets "z"). Fixed: "y" is now tested as a regular flag (`Flags & Level`) in both versions of `Access`; "z" is still given to everyone. `adminlist.cfg` documents "d" and "v" (validated 2026-11-05).
- [ ] **M24 🆕** `PugAdminMenu.cpp:130-134, 177-210, 256-296, 341-369` and `PugVoteMenu.cpp:191-242` — the kick/ban/slap/vote kick target is identified only by entindex.
  - If the target leaves and another player (or an immune admin) takes the same slot before the click, the new occupant is punished.
  - Fix: store `GETPLAYERUSERID` in the item and, in the handler, validate the userid and check `ADMIN_IMMUNITY` again.
- [ ] **M25 🆕** Generic AuthIds treated as unique:
  - `PugAdminMenu.cpp:206`: `banid` with `STEAM_ID_LAN`/`STEAM_ID_PENDING`/`HLTV` bans everyone with the same ID.
  - `PugStats.cpp:191`: those players' stats are added together.
  - `PugTeamManager.cpp:70, 105`: one LAN player dropping locks the team choice of every other LAN player.
  - Fix: detect these ids and use IP/userid as the key, or refuse the ban (`addip`).
- [ ] **M26 🆕** `PugVoteMenu.cpp:211-224` (vote kick) — `Count` adds votes from every slot, including players who switched teams.
  - `Needs = size() - 1` can be 0 with `pug_players_min <= 1`, and the kick passes with 1 vote.
  - Fix: count only members of `Players[team]` and use `Needs = max(1, ...)`.
- [ ] **M27 🆕** `PugMod.cpp:294-296` — `SwapTeams` decides "start of OT" from a tied score. With a `pug_rounds_ot` whose half is even (e.g. 8), the score can be 2-2 at the OT halftime, and then teams do not switch sides. Decide from the state transition instead.
- [x] **M28 🆕 ✔** `PugStats.cpp:921` (`SayText`) — chat was recorded once per recipient. Fixed: only the copy sent to the sender is recorded (`ENTINDEX(pEntity) == GetByte(0)`), once per message, with the sender's team and alive/dead state (validated 2026-11-05).
- [x] **M29 🆕 ✔** `PugStats.cpp:476` — OneShot now uses `Victim->m_lastDamageAmount` (validated 2026-11-05).
- [ ] **M30 🆕** `PugTeamManager.cpp:80, 111-117` — the team saved for reconnection is only stored on the first drop and is not swapped at halftime. A player who drops in both the first and second halves comes back to the wrong team. Fix: always store it and swap TR/CT on `STATE_HALFTIME`.
- [x] **M31 🆕 ✔** `PugCvar.cpp:462, 470` — trailing space removed from the names of the `pug_dm_kill_repair_armor` and `pug_dm_kill_hp_msg` cvars (validated 2026-11-05).
- [ ] **M32 🆕** `PugDM.cpp:105-150` — `GetPlayerSpawnSpot` ignores `Spawn.Team`: CTs spawn on TR spawns and vice versa. Filter by `Team == 0 || Team == m_iTeam` before picking.
- [ ] **M33 🆕** Damage counting: `PugStats.cpp:313`, `PugRoundStats.cpp:141`, `ReGameDLL.cpp:308-316`.
  - The `TakeDamage` return value is ignored: Hits/Damage count even when it returned 0.
  - PugStats computes damage as `m_iLastClientHealth - health`, which only updates once per frame: with two attackers in the same frame, the second one gets the accumulated damage.
  - RoundStats uses `flDamage` without a cap: an AWP shot on a player with 20 HP counts as 400.
- [ ] **M34 🆕** `PugPause.cpp:97-103` — when leaving live, only `m_State` is reset. The `PAUSE_MATCH` task keeps running ("paused" HUD and `SetRoundTime`). If the map changes during a pause, `mp_buytime` keeps the pause value. Fix: `gPugTask.Remove(PAUSE_MATCH)` and restore `mp_buytime`, also in `ServerDeactivate`.
- [ ] **M35 🆕** `PugLO3.cpp:29-36` — the LO3 sequence is not cancelled if the match is cancelled during its ~9 s. It keeps issuing `sv_restart` and showing "LIVE" in DM. Abort if the state is not FIRST/SECOND_HALF/OVERTIME.
- [ ] **M36 🆕 ✔** `PugSmokeGrenade.cpp:11` — `v.origin.y -= 100` changes the grenade's **real** position (on the horizontal Y axis). The extra cloud ends up 100 units to the side and the entity is moved. Use a local copy of the origin (and probably the Z axis).
- [x] **M37 🆕 ✔** `PugLeader.cpp:81-93` (`Init`, follow-up to A11/A13) — when the captain pick cannot start (fewer than 2 players or `pug_players_min < 2`), the `else` prints "Pick failed: Teams are incomplete" and **still schedules the knife round or the first half**.
  - If someone leaves during the team vote, a live match can start with 0 or 1 player. `Stop()` handles the same case correctly by going to `STATE_DEATHMATCH`.
  - With `pug_players_min 1` (1v1) the match is valid, but the failure message is wrong.
  - Fix: with `pug_players_min < 2`, continue to the match without the failure message; with fewer than 2 players, schedule `STATE_DEATHMATCH` like `Stop()`.
  - Fixed: `Init()` requires at least 2 players on each team (teams are kept balanced by `mp_limitteams 1`, now in every `.cfg`, and `pug_players_max`); otherwise it prints "Pick failed" and schedules `STATE_DEATHMATCH`. Captains are drawn one from each team. Known limit: with `pug_players_min 1` (1v1) the captain pick always fails (validated 2026-11-05, Windows build OK).

---

## 4. Low priority

- [ ] **L1 ✔** `PugMenu.cpp:128-145` — a disabled item only changes the sound but still calls the handler (each handler has to handle `Item.Disabled`).
- [x] **L2** `PugAdminMenu.cpp` — permanent ban checks the admin.
- [ ] **L3 ✔** `PugVoteTeam.cpp:324-339` — `TeamsOptimize` fills `Skills` by entindex but compares `Skills[j]` by position in `Players`, and calls `erase` inside the loop.
- [x] **L4** `PugLeader.cpp` — `Leader->` without a null check.
- [x] **L5** `GetWinner` — random tie-break.
- [ ] **L6 ✔** `PugMod.cpp:435` — `VoteEnd::Init` runs before the drop; `m_VotesLeft` counts the player who left.
- [ ] **L7 ✔** `PugAdmin.cpp:32` — `BIT(*Flags++ - 'a')` is UB for uppercase letters or characters outside `a-z`. Filter `c >= 'a' && c <= 'z'`.
- [x] **L8** `PugMenu.cpp` — `Text.copy` bounded.
- [ ] **L9 ✔** `PugStats.cpp:376` — a teamkill counts as a frag (only checks `VictimIndex != KillerIndex`). `PugDM.cpp:782` — `ResetScore` requires `frags > 10`, but the message says "no frags".

### New

- [ ] **L10 🆕** `localtime()` used to format durations.
  - Locations: `PugVoteMap.cpp:253`, `PugVoteEnd.cpp:423`, `PugVoteOvertime.cpp:206`, `PugVoteSwapTeam.cpp:185`, `PugVoteTeam.cpp:462`, `PugTimer.cpp:112`, `PugPause.cpp:115-121`.
  - In half-hour time zones the minutes are wrong. Use `gmtime()` or `%02d:%02d` with `/60` and `%60`.
- [x] **L11 🆕 ✔** `PugUtil.cpp` — `*((int32_t *)&float)` replaced by `FloatBits()`, which uses `memcpy` (no strict-aliasing issue) (validated 2026-11-05).
- [x] **L12 🆕 ✔** `PugMenu.cpp:430` — `strncpy` replaced by `strnlen` + `memcpy` + terminator (no warning and no read past the string) (validated 2026-11-05).
- [x] **L13 🆕 ✔** `PugMod.cpp:236` (follow-up to A15) — the OT score reset compares the sum with `pug_rounds_ot`, but the OT tie happens at `2 * (int)(pug_rounds_ot / 2)` rounds. With an odd value (e.g. 5) the tie is 2-2 (sum 4 < 5), the score is not reset and the next OT ends after 1 round. The default 6 works. Fix: `>= 2 * static_cast<int>(gPugCvar.m_RoundsOT->value / 2.0f)`.
  - Fixed: `pug_rounds_ot` must be an even number greater than 1. Otherwise overtime is never started: a tied second half continues as sudden death. A tie inside OT now votes again (`pug_ot_type 0`) or plays another OT (any other type). Documented in `pugmod.cfg` and README. Known limit: an admin forcing `STATE_OVERTIME` or changing the cvar during OT can still hit the old reset rule (validated 2026-11-05, Windows build OK).
- [x] **L14 🆕 ✔** `PugVoteMap.cpp:273` — `SendHud(nullptr, g_VoteMap_HudParam[1], VoteList.c_str())` uses the map list as the format string. Map names come from the admin's `maplist.cfg`, but a `%` in a name breaks the HUD. Fix: `SendHud(..., "%s", VoteList.c_str())`.
- [x] **L15 🆕 ✔** Same pattern as L14 in `PugVoteEnd.cpp:219`, `PugVoteOvertime.cpp:247`, `PugVoteSwapTeam.cpp:226` and `PugVoteTeam.cpp:279`: the `VoteList` (option names from `_T()` translations) was used as the format. Now `SendHud(..., "%s", VoteList.c_str())`. Also, the overtime vote HUD used `g_VoteSwapTeam_HudParam` instead of `g_VoteOvertime_HudParam` (`PugVoteOvertime.cpp:232, 247, 251`; same values, no visible change) (validated 2026-11-05, Windows build OK).

---

## 5. Suggestions outside the items

- **S1** (A2) `g_Pug_Blocked_Commands` now **does not include `meta`**, `logaddress_add` or `rcon`.
  - An admin with access to the mod's rcon command can run `meta unload`.
  - Proposal: compare the first token of each `;`-separated command and include `meta`, `logaddress_add`, `rcon` and `plugin_*`.
- ~~`precompiled.h`: `#include <algorithm>`~~ — done ✔.
- ~~`PugStats.h`: `SaveData`/`UploadData` by `const &`~~ — done ✔.
- The captain draw became item **A11**.

## 6. Suggested order

1. ~~A11~~ ✔, ~~A12~~ ✔, ~~A13~~ ✔, ~~A14~~ ✔ (they hang or corrupt the server, and the fixes are small).
2. ~~M31~~ ✔, ~~M23~~ ✔, ~~M29~~ ✔, ~~M15~~ ✔ (one line each).
3. ~~A15~~ ✔, ~~M21~~ ✔, ~~M22~~ ✔, ~~M28~~ ✔.
4. Build: ~~B1~~ ✔, ~~B3~~ ✔, ~~B6~~ ✔, ~~B8~~ ✔, ~~B9~~ ✔, B4/B7 skipped; **B11** (PostBuild); **B5**, **B10** and **B12** (Windows) later.
5. ~~M37~~ ✔, ~~L13~~ ✔, ~~L14~~ ✔.
6. The rest of M and L.
