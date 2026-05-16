#include "precompiled.h"

CPugClientCmd gPugClientCmd;

void CPugClientCmd::ServerActivate() {
  this->m_Data.clear();

  this->m_FloodCount = {};

  this->m_FloodTimer = {};

  gPugUtil.ServerCommand("exec %s/cfg/clientcmd.cfg", gPugUtil.GetPath());
}

void CPugClientCmd::Set(std::string Index, std::string Name,
                        std::string Flags) {
  if (!Index.empty() && !Name.empty() && !Flags.empty()) {
    if (this->m_Data.find(Name) == this->m_Data.end()) {
      this->m_Data[Name].Index = std::stoi(Index);

      this->m_Data[Name].Name = Name;

      this->m_Data[Name].Flags = gPugAdmin.ReadFlags(Flags.c_str());
    }
  }
}

LP_CLIENT_CMD CPugClientCmd::Get(const char *Name) {
  if (Name) {
    if (this->m_Data.find(Name) != this->m_Data.end()) {
      return &this->m_Data.at(Name);
    }
  }

  return nullptr;
}

LP_CLIENT_CMD CPugClientCmd::Get(int Index) {
  for (auto const &Command : this->m_Data) {
    if (Command.second.Index == Index) {
      return &this->m_Data.at(Command.first);
    }
  }

  return nullptr;
}

bool CPugClientCmd::FilterFlood(const char *pszCommand, int EntityIndex) {
  if (gPugCvar.m_CmdFloodTime->value > 0.0f) {
    if (!gPugAdmin.Access(EntityIndex, ADMIN_IMMUNITY)) {
      if (Q_stricmp(pszCommand, "say") == 0 ||
          Q_stricmp(pszCommand, "say_team") == 0 ||
          Q_stricmp(pszCommand, "jointeam") == 0 ||
          Q_stricmp(pszCommand, "chooseteam") == 0) {
        if (this->m_FloodTimer[EntityIndex] > gpGlobals->time) {
          if (this->m_FloodCount[EntityIndex] >=
              gPugCvar.m_CmdFloodRepeat->value) {
            this->m_FloodTimer[EntityIndex] =
                (gpGlobals->time + gPugCvar.m_CmdFloodTime->value +
                 gPugCvar.m_CmdFloodNextTime->value);
            return true;
          }

          this->m_FloodCount[EntityIndex] += 1.0f;
        } else if (this->m_FloodCount[EntityIndex]) {
          this->m_FloodCount[EntityIndex] -= 1.0f;
        }

        this->m_FloodTimer[EntityIndex] =
            gpGlobals->time + gPugCvar.m_CmdFloodTime->value;
      }
    }
  }

  return false;
}

bool CPugClientCmd::Command(edict_t *pEntity) {
  if (!FNullEnt(pEntity)) {
    auto pszCommand = CMD_ARGV(0);

    if (pszCommand) {
      if (pszCommand[0u] != '\0') {
        auto Player = UTIL_PlayerByIndexSafe(ENTINDEX(pEntity));

        if (Player) {
          if (this->FilterFlood(pszCommand, Player->entindex())) {
            gPugUtil.ClientPrint(pEntity, E_PRINT::CONSOLE,
                                 _T("*** FLOODING DETECTED ***"));
            return true;
          }

          if (_stricmp(pszCommand, "menuselect") == 0) {
            auto pszArgv = CMD_ARGV(1);

            if (pszArgv) {
              if (gPugMenu[Player->entindex()].Handle(Player,
                                                      std::atoi(pszArgv))) {
                return true;
              }
            }
          } else if (_stricmp(pszCommand, "drop") == 0) {
            if (gPugDM.DropItem(Player)) {
              return true;
            }
          } else if (_stricmp(pszCommand, "say") == 0 ||
                     _stricmp(pszCommand, "say_team") == 0) {
            auto pszArgv = CMD_ARGV(1);

            if (pszArgv) {
              if (pszArgv[0u] == gPugCvar.m_CmdPrefixPlayer->string[0u] ||
                  pszArgv[0u] == gPugCvar.m_CmdPrefixAdmin->string[0u]) {
                gPugUtil.ClientCommand(pEntity, "%s\n", CMD_ARGS());
                return true;
              }

              // Log all-chat messages to stats (say only, not say_team)
              if (_stricmp(pszCommand, "say") == 0) {
                gPugStats.LogChat(Player, pszArgv);
              }
            }
          } else {
            auto pCmd = this->Get(pszCommand);

            if (pCmd) {
              if (gPugAdmin.Access(Player->entindex(), pCmd->Flags)) {
                switch (pCmd->Index) {
                // Players
                case CMD_STATUS: {
                  gPugMod.Status(Player);
                  return true;
                }
                case CMD_SCORE: {
                  gPugMod.Scores(Player);
                  return true;
                }
                case CMD_READY: {
                  gPugReady.Ready(Player);
                  return true;
                }
                case CMD_NOTREADY: {
                  gPugReady.NotReady(Player);
                  return true;
                }
                case CMD_MENU: {
                  return true;
                }
                case CMD_GUNS: {
                  gPugDM.DropItem(Player);
                  return true;
                }
                case CMD_RESPAWN: {
                  gPugDM.Respawn(Player);
                  return true;
                }
                case CMD_RESET_SCORE: {
                  gPugDM.ResetScore(Player);
                  return true;
                }
                case CMD_HELP: {
                  this->Help(Player);
                  return true;
                }
                case CMD_RECORD: {
                  gPugDemoRecord.Menu(Player);
                  return true;
                }
                case CMD_HP: {
                  gPugRoundStats.ShowHP(Player);
                  return true;
                }
                case CMD_DMG: {
                  gPugRoundStats.ShowDamage(Player);
                  return true;
                }
                case CMD_RDMG: {
                  gPugRoundStats.ShowReceivedDamage(Player);
                  return true;
                }
                case CMD_SUM: {
                  gPugRoundStats.ShowSummary(Player);
                  return true;
                }
                case CMD_STATS: {
                  gPugRoundStats.ShowStats(Player);
                  return true;
                }
                case CMD_VOTE_MENU: {
                  gPugVoteMenu.Menu(Player);
                  return true;
                }
                //
                // Admins
                case CMD_HELP_ADMIN: {
                  this->HelpAdmin(Player);
                  return true;
                }
                case CMD_ADMIN_MENU: {
                  gPugAdminMenu.Menu(Player);
                  return true;
                }
                case CMD_ADMIN_KICK: {
                  gPugAdminMenu.Kick(Player);
                  return true;
                }
                case CMD_ADMIN_BAN: {
                  gPugAdminMenu.Ban(Player);
                  return true;
                }
                case CMD_ADMIN_SLAP: {
                  gPugAdminMenu.Slap(Player);
                  return true;
                }
                case CMD_ADMIN_TEAM: {
                  gPugAdminMenu.Team(Player);
                  return true;
                }
                case CMD_ADMIN_MAP: {
                  gPugAdminMenu.Map(Player);
                  return true;
                }
                case CMD_ADMIN_MESSAGE: {
                  this->Message(Player);
                  return true;
                }
                case CMD_ADMIN_RCON: {
                  this->Rcon(Player);
                  return true;
                }
                case CMD_ADMIN_CVAR: {
                  gPugCvarControl.Menu(Player);
                  return true;
                }
                case CMD_DM_SPAWN_EDITOR: {
                  gPugSpawnEdit.EditSpawns(Player);
                  return true;
                }
                case CMD_PAUSE_MATCH: {
                  gPugPause.Init(Player);
                  return true;
                }
                }
              }
            }
          }
        }
      }
    }
  }

  return false;
}

bool CPugClientCmd::Help(CBasePlayer *Player) {
  if (gPugCvar.m_MotdFile->string) {
    if (gPugCvar.m_MotdFile->string[0u] != '\0') {
      gPugUtil.ShowMotd(Player->edict(), gPugCvar.m_MotdFile->string,
                        strlen(gPugCvar.m_MotdFile->string));
      return true;
    }
  }

  return false;
}

bool CPugClientCmd::HelpAdmin(CBasePlayer *Player) {
  if (gPugAdmin.Access(Player->entindex(), ADMIN_LEVEL_C)) {
    if (gPugCvar.m_MotdFileAdmin->string) {
      if (gPugCvar.m_MotdFileAdmin->string[0u] != '\0') {
        gPugUtil.ShowMotd(Player->edict(), gPugCvar.m_MotdFileAdmin->string,
                          strlen(gPugCvar.m_MotdFileAdmin->string));

        return true;
      }
    }
  }

  return false;
}

bool CPugClientCmd::Message(CBasePlayer *Player) {
  if (gPugAdmin.CheckAccess(Player, ADMIN_CHAT)) {
    auto pCmdArgs = CMD_ARGS();

    if (pCmdArgs) {
      if (pCmdArgs[0u] != '\0') {
        gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT,
                            _T("^4[%s]^1 (^3%s^1) %s"), gPugCvar.m_Tag->string,
                            STRING(Player->edict()->v.netname), pCmdArgs);
        return true;
      }
    }

    gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT,
                        _T("^4[%s]^1 Usage: ^3%s^1 <message>"),
                        gPugCvar.m_Tag->string, CMD_ARGV(0));
    return true;
  }

  return false;
}

bool CPugClientCmd::Rcon(CBasePlayer *Player) {
  if (gPugAdmin.CheckAccess(Player, ADMIN_RCON)) {
    auto pCmdArgs = CMD_ARGS();

    if (pCmdArgs) {
      if (pCmdArgs[0u] != '\0') {
        std::string Args = pCmdArgs;

        // Convert to lowercase for case-insensitive comparison
        std::string ArgsLower = Args;
        std::transform(ArgsLower.begin(), ArgsLower.end(), ArgsLower.begin(),
                       ::tolower);

        // Block sensitive commands that could compromise the server
        if (ArgsLower.find("rcon_password") != std::string::npos ||
            ArgsLower.find("sv_password") != std::string::npos ||
            ArgsLower.find("quit") != std::string::npos ||
            ArgsLower.find("exit") != std::string::npos ||
            ArgsLower.find("exec") != std::string::npos ||
            ArgsLower.find("cmdlist") != std::string::npos ||
            ArgsLower.find("cvarlist") != std::string::npos) {
          gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT,
                              _T("^4[%s]^1 Command blocked."),
                              gPugCvar.m_Tag->string);
          return true;
        }

        gPugUtil.ServerCommand("%s", pCmdArgs);

        gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT,
                            _T("^4[%s]^1 Executed: ^3%s^1"),
                            gPugCvar.m_Tag->string, pCmdArgs);

        return true;
      }
    }

    gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT,
                        _T("^4[%s]^1 Usage: ^3%s^1 <command>"),
                        gPugCvar.m_Tag->string, CMD_ARGV(0));
    return true;
  }

  return false;
}