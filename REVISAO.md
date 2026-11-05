# PugMod — Revisão de código

Revisão iniciada em 2026-11-03 (commit `59a9487`) e refeita em 2026-11-05 (commit `2cea70d`). As correções de 2026-11-05 estão no commit que adicionou este arquivo.

Legenda:
- ✔ = conferido diretamente no código.
- Sem ✔ = encontrado por revisão de agentes (lido no código, não testado em servidor).
- 🆕 = item encontrado na revisão de 2026-11-05.
- `[x]` = concluído e validado (`git diff` mais build limpo); `[-]` = ignorado por decisão do mantenedor.

**Build de validação:**
- Container `ubuntu:18.04` (GCC 7.5, mesmo ambiente do CI), `make prebuild linux32`: OK. O `.so` exige no máximo GLIBC_2.4 e depende só de `libcurl.so.4`, `libdl`, `libm`, `libc` e `ld-linux`.
- Arch (g++ 16): OK, só para teste local.
- Avisos restantes do projeto: 6 de `-Wstrict-aliasing` em `PugUtil.cpp:614-619` (L11) e 1 de `-Wstringop-truncation` em `PugMenu.cpp:430` (L12). Os demais são do CSSDK.

---

## Resumo

| Grupo | Concluídos | Pendentes |
|---|---|---|
| Build (B) | B1, B2, B3, B6 (B4, B7 ignorados) | B5 |
| Alta (A) | A1–A15 | — |
| Média (M) | M1–M12, M14, M15, M21–M23, M28, M29, M31 | M13, M16–M20, M24–M27, M30, M32–M36 🆕 |
| Baixa (L) | L2, L4, L5, L8 | L1, L3, L6, L7, L9, L10–L12 🆕 |

M7, M8, M9 e M10 foram concluídos nos commits `c55a316` e `2cea70d`; os demais itens marcados, nas correções de 2026-11-05.

---

## 1. Build / requisitos

- [x] **B1 ✔** Padrão C++14 no `Makefile` (`-std=c++14`) e no `.vcxproj` (`stdcpp14`). O CI agora usa o GCC 7.5 do Ubuntu 18.04 (o GCC 4.8 foi removido) (validado 2026-11-05).
- [x] **B2** `Makefile`: `-O0` trocado por `-O2`; `-fno-stack-protector` removido.
- [x] **B3 ✔** cURL dinâmico de forma explícita: `-Wl,--as-needed -lcurl -ldl -lm`, sem `-lssl -lcrypto -lz` e sem `-DCURL_STATICLIB`. O `.so` agora depende só de `libcurl.so.4` (símbolos `CURL_OPENSSL_4`), `libdl`, `libm`, `libc` e `ld-linux` (validado 2026-11-05).
  - README atualizado: requisitos do servidor (`libcurl4:i386` ou `lib32-curl`, mais `ca-certificates`), C++14 e pacotes de build.
- [-] **B4** ~~`make` sem alvo executa `PostBuild.sh`~~ — **ignorado** (decisão do usuário): sem o HLDS na pasta, o script só sai.
- [ ] **B5** MSBuild: `LNK4098 LIBCMT`. O Release usa `MultiThreadedDLL` (/MD) com `libcurl_a.lib` (provavelmente compilada com /MT). Trocar para `MultiThreaded` (/MT) ou usar uma libcurl /MD. Não dá para testar no Linux.
- [x] **B6 🆕 ✔** Compatibilidade de glibc e de CPU. Compilado no container `ubuntu:18.04` (mesmo ambiente do CI, GCC 7.5), o `.so` exige no máximo **GLIBC_2.4**, não depende de `libstdc++` e não tem instruções SSE4 (`-march=i686 -msse2 -msse3` no lugar de `-msse4.1 -msse4.2`). O build local do Arch (GLIBC 2.43) serve só para testes; para distribuir, use o artefato do CI ou o container (validado 2026-11-05).
- [-] **B7** ~~Sem rastreio de dependência de headers~~ — **ignorado** (decisão do usuário): sempre será feito build limpo.

---

## 2. Alta prioridade — crash, travamento do servidor ou partida presa

- [x] **A1** `PugCurl.cpp` — `curl_multi_perform` em loop bloqueava o frame.
- [x] **A2** `PugClientCmd` — `nullptr` no array de comandos bloqueados. (Ver sugestão S1.)
- [x] **A3** Format string com nick (AdminMenu, Leader, RoundStats).
- [x] **A4** `Q_snprintf` com tamanho errado e troca geral para `snprintf`/`vsnprintf`.
- [x] **A5 ✔** `PugDM.cpp:109` — `RANDOM_LONG(0, size() - 1)`.
- [x] **A6 ✔** `PugDM.cpp:740` — `EquipRandomItem` agora sorteia entre candidatos.
- [x] **A7** `PugStatus.cpp` — `dump()` com `error_handler_t::replace`.
- [x] **A8** `PugRageBan.cpp:13` — checagem de nulo.
- [x] **A9** `PugLeader.cpp` — `std::string(5 - Count, '\n')`.
- [x] **A10** `PugVoteMap.cpp` — `GetRandom()` em loop infinito.

### Novos

- [x] **A11 🆕 ✔** `PugLeader.cpp:31` — loop infinito no sorteio de capitães com 1 jogador. Corrigido: `Init()` exige `Players.size() >= 2 && pug_players_min >= 2` (validado 2026-11-05, build OK).
- [x] **A12 🆕 ✔** Handlers de voto (`PugVoteEnd`, `PugVoteOvertime`, `PugVoteSwapTeam`, `PugVoteTeam`, `PugVoteMap`) usavam `m_VoteList[Item.Info]` sem checar o limite. Corrigido com checagem de limite do índice e com `m_Voter[]`, marcado no `Init()` para os participantes e limpo no `ServerActivate`/`ServerDeactivate` (validado 2026-11-05, build limpo OK).
- [x] **A13 🆕 ✔** `PugLeader.cpp` — fluxo do capitão podia ficar preso em STATE_CAPTAIN. Corrigido (validado 2026-11-05, build limpo OK):
  - `Init()`: o `else` agenda knife ou 1º tempo (junto com o A11).
  - `Next()`: termina sempre em `Stop()` quando nenhum menu é aberto.
  - `GetPlayer()`: auto-pick sem spectators chama `Stop()`; com spectators, o auto-pick agora sorteia.
- [x] **A14 🆕 ✔** `PugUtil.cpp:815` — `GetPlayers()` (array por time) agora ignora `FL_PROXY`. O HLTV não entra mais no pick, no vote kick nem nas contagens de Status (validado 2026-11-05).
  - O HUD do pick (`PugLeader.cpp:395`, `StartFrame`) também ignora o HLTV.
- [x] **A15 🆕 ✔** `PugMod.cpp:236` — `m_ScoreOT` não era zerado ao começar um novo overtime (o OT2 durava 1 round). Corrigido (validado 2026-11-05, build OK):
  - Ao entrar em STATE_OVERTIME, zera se a soma for `>= pug_rounds_ot` (OT anterior completo). A 2ª metade do OT e o Sudden Death preservam o placar.
  - `case 3` (Sudden Death) explícito nos dois `switch` de `pug_ot_type`: continua a partida e o próximo round decide.

---

## 3. Média prioridade — lógica e segurança

- [x] **M1** `PugVoteOvertime.cpp` — menu com o `E_MENU` errado.
- [x] **M2** `MetaMod.cpp` — VoteEnd/VoteOvertime sem StartFrame/ServerActivate.
- [x] **M3** `PugMenu` — menus não resetados no disconnect; handlers revalidam acesso.
- [x] **M4** `ReAPI.cpp` — `CPugVoteMenu::DropClient` agora é chamado.
- [x] **M5** `PugTeamManager.cpp` — `TERRORIST || CT`.
- [x] **M6** `PugVoteMenu.cpp` — `Players.size()` de array fixo.
- [x] **M7 ✔** `PugMapList.cpp:16` — lista limitada a `MAX_VOTE_MAPS`; as chaves são `0..size-1`, então `m_VoteMap[...][MAX_VOTE_MAPS + 1]` não estoura mais.
- [x] **M8 ✔** `PugUtil.cpp:88, 327` — `vsnprintf(..., sizeof - 1)` deixa espaço para o `strcat("\n")`. O eco `"%s\n"` em `PugClientCmd.cpp:142` também foi corrigido.
- [x] **M9 ✔** `PugCurl.cpp` — `curl_slist` guardado em `P_CURL_MEMORY::Headers` e liberado no `CURLMSG_DONE`; `CURLOPT_PRIVATE` via `(void *)(intptr_t)`.
- [x] **M10 ✔** `PugCurl.cpp:190` — `ServerExecute` agora exige `pug_api_execute 1` (padrão 0) e URL final HTTPS, além de `CURLOPT_MAXREDIRS 3`. Documentado no README e no `pugmod.cfg`.
- [x] **M11** `PugStatus.cpp` — `m_Data["Players"]` limpo.
- [x] **M12 ✔** `PugStats.cpp` — `Stats[STATE_OVERTIME]` era zerado a cada entrada em OVERTIME (pelo `SetState` e pelo `RestartRound` do LO3); só a última metade do último OT ia para o JSON. Corrigido (validado 2026-11-05, build OK):
  - O OT acumula entre todos os overtimes e é zerado no início da partida (FIRST_HALF).
  - O `RestartRound` não zera mais o OT.
  - `SetAnimation`, `TakeDamage` e `SendDeathMessage` só contam com `m_bGameStarted`, o que ignora o LO3.
- [ ] **M13 ✔** `PugStats.cpp:40-60` — `m_Player` só é limpo em STATE_DEAD e no ServerActivate.
  - Em END → VOTEMAP/DEATHMATCH no mesmo mapa, jogadores da partida anterior continuam no JSON da próxima.
  - Esses jogadores levam Team, Winner e JoinGameTime antigos, e também `Stats[OVERTIME]` antigo se a nova partida não tiver OT.
- [x] **M14** `PugStats.cpp` — JSON com `replace` e `ofstream` checado.
- [x] **M15 ✔** `PugStats.cpp:1278` — `Result.Survives +=` (validado 2026-11-05).
- [ ] **M16 ✔** (descrição revisada) `PugStats.cpp:392-406` — o teste de trade (`Victim.KillTime <= 3s`) está **aninhado** dentro de `if (Killer.KillTime > 0)`. Só conta trade se o killer já tinha matado alguém no round. Mover o bloco do trade para fora desse `if`.
- [ ] **M17 ✔** `PugSpawnEdit.cpp:477` — `Index = size() + 1` pode sobrescrever um spawn existente (as chaves não são contíguas depois de apagar). `:493-499` compara `m_Spawns[EntityIndex]` (índice do jogador) em vez do spawn marcado.
- [ ] **M18 ✔** `PugSpread.cpp:35` — `Q_fabs(m_flNextPrimaryAttack) > 0.0f` quase sempre é verdadeiro e zera o spread. Também não há cvar nem checagem de estado: vale até no 1º e 2º tempo. Sugestão: cvar `pug_spread_fix` (padrão 0) mais checagem de estado.
- [ ] **M19 ✔** `PugRestrictItem.cpp:24` — `m_RestrictItem->string[Item]` sem checar `strlen`.
- [ ] **M20 ✔** `PugRageBan.cpp` — `m_Data` não é limpo na troca de mapa.
  - `CheckTime` usa o `gpGlobals->time` do mapa anterior.
  - `banid` com authid genérico (`STEAM_ID_LAN`, `STEAM_ID_PENDING`, `VALVE_ID_LAN`) bane todos com o mesmo ID.
  - As mensagens não passam por `_T()`, então não têm tradução.

### Novos

- [x] **M21 🆕 ✔** `PugVoteMap.cpp:41` — com o maplist vazio, a troca para VOTETEAM agora é agendada com `gPugTask.Create(SET_STATE, 1.0f, ...)`, sem `SetState` recursivo. A tradução da mensagem opcional "Map list is empty, skipping map vote." está no `language.cfg` (validado 2026-11-05).
- [x] **M22 🆕 ✔** `PugVoteMap.cpp` — menu de mapas vazio (todos os mapas eram o atual ou o último) gerava loop de votação. Corrigido: o `LoadMaps()` não carrega o mapa atual nem o último; com a lista vazia, vai para VOTETEAM com a mensagem "No maps available to vote, skipping map vote." (traduzida no `language.cfg`). `This`/`Last` removidos, `GetRandom()` simplificado (validado 2026-11-05, build limpo OK).
- [x] **M23 🆕 ✔** `PugAdmin.cpp` — `Access(..., ADMIN_ADMIN)` era sempre falso por índice (todos recebem "z"). Corrigido: "y" passa a ser testada como flag comum (`Flags & Level`) nas duas versões do `Access`; "z" continua para todos. `adminlist.cfg` documenta "d" e "v" (validado 2026-11-05).
- [ ] **M24 🆕** `PugAdminMenu.cpp:130-134, 177-210, 256-296, 341-369` e `PugVoteMenu.cpp:191-242` — o alvo de kick/ban/slap/vote kick é identificado só pelo entindex.
  - Se o alvo sai e outro jogador (ou um admin imune) entra no mesmo slot antes do clique, o novo ocupante é punido.
  - Correção: guardar `GETPLAYERUSERID` no item e, no handler, validar o userid e checar `ADMIN_IMMUNITY` de novo.
- [ ] **M25 🆕** AuthIds genéricos tratados como únicos:
  - `PugAdminMenu.cpp:206`: `banid` com `STEAM_ID_LAN`/`STEAM_ID_PENDING`/`HLTV` bane todos com o mesmo ID.
  - `PugStats.cpp:191`: os stats desses jogadores são somados juntos.
  - `PugTeamManager.cpp:70, 105`: a queda de um jogador LAN trava a escolha de time de todos os outros LAN.
  - Correção: detectar esses ids e usar IP/userid como chave, ou recusar o ban (`addip`).
- [ ] **M26 🆕** `PugVoteMenu.cpp:211-224` (vote kick) — `Count` soma votos de todos os slots, inclusive de quem trocou de time.
  - `Needs = size() - 1` pode dar 0 com `pug_players_min <= 1`, e o kick sai com 1 voto.
  - Correção: contar só os membros de `Players[team]` e usar `Needs = max(1, ...)`.
- [ ] **M27 🆕** `PugMod.cpp:294-296` — `SwapTeams` decide "início de OT" pelo placar empatado. Com `pug_rounds_ot` cuja metade é par (ex.: 8), pode estar 2-2 no intervalo do OT, e aí os times não trocam de lado. Decidir pela transição de estado.
- [x] **M28 🆕 ✔** `PugStats.cpp:921` (`SayText`) — o chat era gravado para cada destinatário. Corrigido: só registra a cópia enviada ao próprio remetente (`ENTINDEX(pEntity) == GetByte(0)`), uma vez por mensagem e com o time e o estado (vivo ou morto) de quem enviou (validado 2026-11-05).
- [x] **M29 🆕 ✔** `PugStats.cpp:476` — OneShot agora usa `Victim->m_lastDamageAmount` (validado 2026-11-05).
- [ ] **M30 🆕** `PugTeamManager.cpp:80, 111-117` — o time salvo para reconexão só é gravado na primeira queda e não é invertido no intervalo. Quem cai no 1º e no 2º tempo volta para o time errado. Correção: gravar sempre e inverter TR/CT no `STATE_HALFTIME`.
- [x] **M31 🆕 ✔** `PugCvar.cpp:462, 470` — espaço no fim do nome dos cvars `pug_dm_kill_repair_armor` e `pug_dm_kill_hp_msg` removido (validado 2026-11-05).
- [ ] **M32 🆕** `PugDM.cpp:105-150` — `GetPlayerSpawnSpot` ignora `Spawn.Team`: CT nasce em spawn de TR e vice-versa. Filtrar por `Team == 0 || Team == m_iTeam` antes de sortear.
- [ ] **M33 🆕** Contagem de dano: `PugStats.cpp:313`, `PugRoundStats.cpp:141`, `ReGameDLL.cpp:308-316`.
  - O retorno do `TakeDamage` é ignorado: Hits/Damage contam mesmo quando ele retornou 0.
  - O PugStats calcula o dano por `m_iLastClientHealth - health`, que só atualiza uma vez por frame: com dois atacantes no mesmo frame, o segundo recebe o dano acumulado.
  - O RoundStats usa `flDamage` sem limite: um AWP em quem tem 20 HP conta 400.
- [ ] **M34 🆕** `PugPause.cpp:97-103` — ao sair de live, só `m_State` é zerado. A task `PAUSE_MATCH` continua (HUD "paused" e `SetRoundTime`). Se o mapa troca durante o pause, `mp_buytime` fica com o valor do pause. Correção: `gPugTask.Remove(PAUSE_MATCH)` e restaurar `mp_buytime`, inclusive no `ServerDeactivate`.
- [ ] **M35 🆕** `PugLO3.cpp:29-36` — a sequência LO3 não é cancelada se a partida for cancelada durante os cerca de 9 s. Ela continua dando `sv_restart` e mostrando "LIVE" no DM. Abortar se o estado não for FIRST/SECOND_HALF/OVERTIME.
- [ ] **M36 🆕 ✔** `PugSmokeGrenade.cpp:11` — `v.origin.y -= 100` altera a posição **real** da granada (no eixo horizontal Y). A nuvem extra fica 100 unidades para o lado e a entidade fica deslocada. Usar uma cópia local da origem (e provavelmente o eixo Z).

---

## 4. Baixa prioridade

- [ ] **L1 ✔** `PugMenu.cpp:128-145` — item desabilitado só muda o som, mas ainda chama o handler (cada handler precisa tratar `Item.Disabled`).
- [x] **L2** `PugAdminMenu.cpp` — ban permanente checa o admin.
- [ ] **L3 ✔** `PugVoteTeam.cpp:324-339` — `TeamsOptimize` preenche `Skills` por entindex mas compara `Skills[j]` por posição em `Players`, e faz `erase` dentro do loop.
- [x] **L4** `PugLeader.cpp` — `Leader->` sem checar nulo.
- [x] **L5** `GetWinner` — desempate aleatório.
- [ ] **L6 ✔** `PugMod.cpp:435` — `VoteEnd::Init` roda antes do drop; `m_VotesLeft` conta quem saiu.
- [ ] **L7 ✔** `PugAdmin.cpp:32` — `BIT(*Flags++ - 'a')` é UB para letra maiúscula ou caractere fora de `a-z`. Filtrar `c >= 'a' && c <= 'z'`.
- [x] **L8** `PugMenu.cpp` — `Text.copy` limitado.
- [ ] **L9 ✔** `PugStats.cpp:376` — teamkill contado como frag (só checa `VictimIndex != KillerIndex`). `PugDM.cpp:782` — `ResetScore` exige `frags > 10`, mas a mensagem diz "sem frags".

### Novos

- [ ] **L10 🆕** `localtime()` usado para formatar duração.
  - Locais: `PugVoteMap.cpp:253`, `PugVoteEnd.cpp:423`, `PugVoteOvertime.cpp:206`, `PugVoteSwapTeam.cpp:185`, `PugVoteTeam.cpp:462`, `PugTimer.cpp:112`, `PugPause.cpp:115-121`.
  - Em fusos com meia hora os minutos saem errados. Usar `gmtime()` ou `%02d:%02d` com `/60` e `%60`.
- [ ] **L11 🆕 ✔** `PugUtil.cpp:614-619` — `*((int32_t *)&TextParams.x)` gera warning de strict-aliasing com `-O2`. Trocar por `memcpy` para um `int32_t` local.
- [ ] **L12 🆕 ✔** `PugMenu.cpp:430` — o `strncpy` gera o warning `-Wstringop-truncation`, mas é seguro, porque o terminador é escrito na linha 432. Opcional: trocar por `memcpy` para silenciar.

---

## 5. Sugestões fora dos itens

- **S1** (A2) `g_Pug_Blocked_Commands` agora **não inclui `meta`** nem `logaddress_add`/`rcon`.
  - Um admin com acesso ao comando de rcon do mod pode fazer `meta unload`.
  - Proposta: comparar o primeiro token de cada comando separado por `;` e incluir `meta`, `logaddress_add`, `rcon` e `plugin_*`.
- ~~`precompiled.h`: `#include <algorithm>`~~ — feito ✔.
- ~~`PugStats.h`: `SaveData`/`UploadData` por `const &`~~ — feito ✔.
- O sorteio de capitães virou o item **A11**.

## 6. Ordem sugerida

1. ~~A11~~ ✔, ~~A12~~ ✔, ~~A13~~ ✔, ~~A14~~ ✔ (travam ou corrompem o servidor, e as correções são pequenas).
2. ~~M31~~ ✔, ~~M23~~ ✔, ~~M29~~ ✔, ~~M15~~ ✔ (uma linha cada).
3. ~~A15~~ ✔, ~~M21~~ ✔, ~~M22~~ ✔, ~~M28~~ ✔.
4. Build: ~~B1~~ ✔, ~~B3~~ ✔, ~~B6~~ ✔, B4/B7 ignorados; **B5** (Windows, `/MT`) depois.
5. Restante de M e L.
