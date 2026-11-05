# PugMod — Plugin MetaMod para Counter-Strike 1.6

Repositório: https://github.com/SmileYzn/PugMod (branch `main`)
Responda sempre em português.

## Arquivos de trabalho

- `CLAUDE.md` (este arquivo): instruções e contexto do projeto.
- `REVISAO.md`: relatório da revisão de código e progresso, item por item.
- Código em `PugMod/`; configs, traduções e spawns em `cstrike/addons/pugmod/`.

## Stack

- C++14 (`-std=c++14` no `Makefile`, `stdcpp14` no `.vcxproj`). O CI compila o Linux com o GCC 7.5 do container `ubuntu:18.04`: não use recursos além de C++14.
- MetaMod-R: https://github.com/rehlds/metamod-r
- ReHLDS 3.15.0.908+ e ReGameDLL_CS 5.30.0.831+ (versões do GitHub Actions, não apenas releases)
- cURL (multi, não bloqueante) e nlohmann/json 3.12.0 (`PugMod/include/json.hpp`)
- Binário 32 bits: `pugmod_mm.so` (Linux) / `pugmod_mm.dll` (Windows)

## Build

### Linux

Arch (requer o repositório `[multilib]` habilitado em `/etc/pacman.conf`):

```sh
sudo pacman -S --needed base-devel lib32-gcc-libs lib32-glibc lib32-curl
cd PugMod
make clean && make prebuild linux32
```

Debian/Ubuntu: `sudo apt-get install build-essential gcc-multilib g++-multilib libcurl4-openssl-dev:i386`.

- Saída: `PugMod/Release/pugmod_mm.so`.
- `make` sem alvo também executa o `PostBuild.sh`, que mata o hlds, copia o `.so` e reinicia o servidor de teste. Se a pasta do HLDS não existir, o script só sai. Para só compilar, use `make prebuild linux32`.
- Sempre faça build limpo (`make clean` antes): o Makefile não rastreia dependências de `.h`.
- cURL é ligado dinamicamente (`-Wl,--as-needed -lcurl -ldl -lm`). O `.so` depende só de `libcurl.so.4`, `libdl`, `libm`, `libc` e `ld-linux`; `libstdc++` e `libgcc` são estáticas.
- `-Wl,--exclude-libs,ALL` evita exportar os símbolos da libstdc++ estática (colisão com outros módulos), e `-Wl,--gc-sections` descarta o código não usado. Confira com `nm -D --defined-only Release/pugmod_mm.so | wc -l` (cerca de 57).
- O binário exige a glibc da máquina onde foi compilado. Um build numa distro recente serve só para teste local. Para distribuir, use o artefato do CI ou o container (resultado: GLIBC 2.4 no máximo):

```sh
docker run --rm -v "$PWD":/desktop ubuntu:18.04 bash -c "dpkg --add-architecture i386 && apt-get update > /dev/null && apt-get install -qq -y build-essential gcc-multilib g++-multilib libcurl4-openssl-dev:i386 > /dev/null && cd /desktop/PugMod && make prebuild linux32"
```

- Flags de CPU: `-march=i686 -mtune=generic -msse2 -msse3` (sem SSE4, para não dar "illegal instruction" em CPUs antigas).

### Windows

```powershell
& "C:\Program Files\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\MSBuild.exe" PugMod\PugMod.vcxproj -property:configuration=Release -property:platform=Win32 -nologo -verbosity:minimal
```

- Toolset v143, Win32. A cURL é estática (`PugMod/include/curl/lib/libcurl_a.lib`).
- Pendente (B5 no `REVISAO.md`): passar `RuntimeLibrary` para `MultiThreaded` (`/MT`, Release) e `MultiThreadedDebug` (`/MTd`, Debug). Isso remove a dependência do VC++ Redistributable e o aviso `LNK4098 LIBCMT`. Conferir com `dumpbin /dependents Release\pugmod_mm.dll` (sem `VCRUNTIME140.dll`).

O CI (GitHub Actions, `.github/workflows/build.yml`) compila Linux e Windows a cada push.

## Arquitetura

- `MetaMod.cpp`: distribui os eventos da engine (ServerActivate/Deactivate, StartFrame, ClientCommand, Message*) para os módulos.
- `ReGameDLL.cpp`: hookchains do ReGameDLL. `ReAPI.cpp`: `SV_DropClient` do ReHLDS.
- Cada módulo é uma classe `CPug*` com instância global `gPug*` (ex.: `gPugVoteMap`).
- Um módulo novo com `StartFrame`/`ServerActivate`/`ServerDeactivate` precisa ser registrado em `MetaMod.cpp`, e o `DropClient` em `ReAPI.cpp`.
- `precompiled.h` inclui todos os headers do projeto.
- Troca de estado (`CPugMod::SetState`) a partir de código que já roda dentro de um `SetState` deve ser agendada com `gPugTask.Create(E_TASK::SET_STATE, ...)`, nunca chamada direto.
- Configs, traduções e spawns: `cstrike/addons/pugmod/` (`cfg/language.cfg`, `cfg/clientcmd.cfg`, `spawns/`).

## Convenções de código

- Indentação de 4 espaços, chaves na linha de baixo (Allman), uma linha em branco entre instruções. Alguns arquivos antigos usam TAB: mantenha o estilo do arquivo que estiver editando.
- O repositório tem arquivos com final de linha CRLF e LF: mantenha o do arquivo que estiver editando.
- Prefixo `this->` nos membros; variáveis em PascalCase.
- Textos exibidos ao jogador usam `_T("...")` e precisam de entrada no `cfg/language.cfg` (UTF-8), com a chave idêntica ao texto do código, mais `en:`, `ru:` e `br:`.

## Regras de segurança aprendidas na revisão

- Use `snprintf`/`vsnprintf` padrão, nunca `Q_snprintf`/`Q_vsnprintf`/`_snprintf` (no MSVC não terminam a string ao truncar).
- Nick de jogador ou qualquer texto externo nunca vai como formato: use `AddItem(..., "%s", nome)`, `SendHud(..., "%s", texto)`.
- Ao concatenar em buffer: `snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf), ...)`, ou use `std::string`.
- Sempre cheque `if (Player)` depois de `UTIL_PlayerByIndexSafe` (o private data pode ser nulo para quem ainda está conectando).
- `RANDOM_LONG(low, high)` inclui o `high`: para índice, use `size() - 1`.
- Sem loops "sortear até achar": filtre candidatos primeiro e sorteie entre eles.
- JSON: `dump(-1, ' ', false, nlohmann::json::error_handler_t::replace)` (nick fora de UTF-8 lança exceção e derruba o servidor).
- Exceções não podem escapar de callbacks da engine. Use `try/catch` só onde há dado externo (JSON, `std::stoi` de config), não de forma genérica.
- Handlers de menu revalidam permissão (`gPugAdmin.CheckAccess`), estado (ex.: `m_Run`), limite do índice (`Item.Info < size()`) e se o jogador participa da votação (`m_Voter[]`).
- `gPugUtil.GetPlayers()` (as duas versões) ignora o HLTV (`FL_PROXY`); laços próprios sobre `maxClients` precisam fazer o mesmo.
- Nada de extensões exclusivas do MSVC (ex.: `std::string::_Copy_s`): o código precisa compilar no g++.

## Fluxo de trabalho

- O trabalho segue o `REVISAO.md`, item por item (códigos B = build, A = alta, M = média, L = baixa).
- Em geral o usuário aplica as correções. O Claude mostra a correção, depois valida (`git diff` mais build limpo) e marca o item como `[x]` no `REVISAO.md`.
- Só edite o código diretamente quando o usuário pedir.
- Não faça commit nem push sem o usuário pedir.
