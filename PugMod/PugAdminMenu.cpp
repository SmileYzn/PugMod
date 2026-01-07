#include "precompiled.h"

CPugAdminMenu gPugAdminMenu;

void CPugAdminMenu::Menu(CBasePlayer *Player)
{
    if (gPugAdmin.CheckAccess(Player, ADMIN_MENU))
    {
        gPugMenu[Player->entindex()].Create(true, E_MENU::ME_ADMIN_MENU, _T("Pug Mod Menu:"));

        gPugMenu[Player->entindex()].AddItem(ADMIN_MENU_KICK, false, ADMIN_KICK, _T("Kick Player"));

        gPugMenu[Player->entindex()].AddItem(ADMIN_MENU_BAN, false, ADMIN_BAN|ADMIN_BAN_TEMP, _T("Ban Player"));

        gPugMenu[Player->entindex()].AddItem(ADMIN_MENU_SLAP, false, ADMIN_SLAY, _T("Slap Player"));

        gPugMenu[Player->entindex()].AddItem(ADMIN_MENU_TEAM, false, ADMIN_LEVEL_A, _T("Player Team"));

        gPugMenu[Player->entindex()].AddItem(ADMIN_MENU_MAP, false, ADMIN_MAP, _T("Change Map"));

        gPugMenu[Player->entindex()].AddItem(ADMIN_MENU_PUG, false, ADMIN_VOTE|ADMIN_LEVEL_B, _T("Control Pug"));

        gPugMenu[Player->entindex()].AddItem(ADMIN_MENU_CVAR, false, ADMIN_CVAR, _T("Control Cvars"));

        gPugMenu[Player->entindex()].AddItem(ADMIN_MENU_PAUSE, false, ADMIN_VOTE|ADMIN_LEVEL_B, _T("Pause Match"));

        gPugMenu[Player->entindex()].AddItem(ADMIN_MENU_SPAWN_EDIT, false, ADMIN_LEVEL_D, _T("CSDM: Spawn Manager"));

        gPugMenu[Player->entindex()].Show(Player);
    }
}

void CPugAdminMenu::MenuHandle(CBasePlayer *Player, P_MENU_ITEM Item)
{
    if (Player)
    {
        if (gPugAdmin.CheckAccess(Player, Item.Extra))
        {
            switch (Item.Info)
            {
                case ADMIN_MENU_KICK:
                {
                    this->Kick(Player);
                    break;
                }
                case ADMIN_MENU_BAN:
                {
                    this->Ban(Player);
                    break;
                }
                case ADMIN_MENU_SLAP:
                {
                    this->Slap(Player);
                    break;
                }
                case ADMIN_MENU_TEAM:
                {
                    this->Team(Player);
                    break;
                }
                case ADMIN_MENU_MAP:
                {
                    this->Map(Player);
                    break;
                }
                case ADMIN_MENU_PUG:
                {
                    this->Pug(Player);
                    break;
                }
                case ADMIN_MENU_CVAR:
                {
                    gPugCvarControl.Menu(Player);
                    break;
                }
                case ADMIN_MENU_PAUSE:
                {
                    gPugPause.Init(Player);
                    break;
                }
                case ADMIN_MENU_SPAWN_EDIT:
                {
                    gPugSpawnEdit.EditSpawns(Player);
                    break;
                }
            }
        }
        else
        {
            this->Menu(Player);
        }
    }
}

void CPugAdminMenu::Kick(CBasePlayer *Player)
{
    if (gPugAdmin.CheckAccess(Player, ADMIN_KICK))
    {
        auto Players = gPugUtil.GetPlayers(false, false);

        if (Players.size() > 0)
        {
            gPugMenu[Player->entindex()].Create(true, E_MENU::ME_ADMIN_MENU_KICK, _T("Kick Player:"));

            for (auto const & Target : Players)
            {
                if (!gPugAdmin.Access(Target->entindex(), ADMIN_IMMUNITY))
                {
                    gPugMenu[Player->entindex()].AddItem(Target->entindex(), false, 0, STRING(Target->edict()->v.netname));
                }
            }

            gPugMenu[Player->entindex()].Show(Player);
        }
        else
        {
            this->Menu(Player);

            gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 No player found."), gPugCvar.m_Tag->string);
        }
    }
}

void CPugAdminMenu::KickHandle(CBasePlayer *Player, P_MENU_ITEM Item)
{
    if (Player)
    {
        auto Target = UTIL_PlayerByIndexSafe(Item.Info);

        if (Target)
        {
            gPugUtil.ClientDrop(Target->entindex(), _T("Kicked by %s"), STRING(Player->edict()->v.netname));
        }

        this->Kick(Player);
    }
}

void CPugAdminMenu::Ban(CBasePlayer *Player)
{
    if (gPugAdmin.CheckAccess(Player, ADMIN_BAN|ADMIN_BAN_TEMP))
    {
        auto Players = gPugUtil.GetPlayers(false, false);

        if (Players.size() > 0)
        {
            gPugMenu[Player->entindex()].Create(true, E_MENU::ME_ADMIN_MENU_BAN, _T("Ban Player:"));

            for (auto const & Target : Players)
            {
                if (!gPugAdmin.Access(Target->entindex(), ADMIN_IMMUNITY))
                {
                    gPugMenu[Player->entindex()].AddItem(Target->entindex(), false, -1, STRING(Target->edict()->v.netname));
                }
            }

            gPugMenu[Player->entindex()].Show(Player);
        }
        else
        {
            this->Menu(Player);

            gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 No player found."), gPugCvar.m_Tag->string);
        }
    }
}

void CPugAdminMenu::BanHandle(CBasePlayer *Player, P_MENU_ITEM Item)
{
    if (Player)
    {
        auto Target = UTIL_PlayerByIndexSafe(Item.Info);

        if (Target)
        {
            if (Item.Extra < 0)
            {
                gPugMenu[Player->entindex()].Create(true, E_MENU::ME_ADMIN_MENU_BAN, _T("Ban duration: ^w%s^y"), STRING(Target->edict()->v.netname));

                std::vector<time_t> BanTimes = { 5, 10, 15, 30, 60, 120, 240, 480, 960, 1440, 10080, 43200 };

                if (gPugAdmin.Access(Target->entindex(), ADMIN_BAN))
                {
                    BanTimes.insert(BanTimes.begin(), 0);
                }

				for (auto const& Time : BanTimes)
				{
                    auto TimeUnit = CTimeFormat::GetTimeLength(Time, CTimeFormat::TIMEUNIT_MINUTES);

                    if (TimeUnit.length() > 0)
                    {
                        gPugMenu[Player->entindex()].AddItem(Target->entindex(), false, Time, TimeUnit.c_str());
                    }
				}

                gPugMenu[Player->entindex()].Show(Player);
            }
            else
            {
				gPugUtil.ServerCommand(_T("banid %d %s"), Item.Extra, GETPLAYERAUTHID(Target->edict()));

				gPugUtil.ServerCommand(_T("wait;wait;writeid;writeip"));

                gPugUtil.ClientDrop(Target->entindex(), _T("You have been banned from the server"));

                this->Ban(Player);
            }
        }
    }
}

void CPugAdminMenu::Slap(CBasePlayer *Player)
{
    if (gPugAdmin.CheckAccess(Player, ADMIN_SLAY))
    {
        auto Players = gPugUtil.GetPlayers(false, true);

        if (Players.size() < 1)
        {
            this->Menu(Player);

            gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 No player found."), gPugCvar.m_Tag->string);
            return;
        }

        gPugMenu[Player->entindex()].Create(true, E_MENU::ME_ADMIN_MENU_SLAP, _T("Slap Player:"));

        for (auto const & Target : Players)
        {
            if (!gPugAdmin.Access(Target->entindex(), ADMIN_IMMUNITY))
            {
                if (Target->IsAlive())
                {
                    gPugMenu[Player->entindex()].AddItem(Target->entindex(), false, -1, STRING(Target->edict()->v.netname));
                }
            }
        }

        gPugMenu[Player->entindex()].Show(Player);
    }
}

void CPugAdminMenu::SlapHandle(CBasePlayer *Player, P_MENU_ITEM Item)
{
    if (Player)
    {
        auto Target = UTIL_PlayerByIndexSafe(Item.Info);

        if (Target)
        {
            if (Item.Extra < 0)
            {
                gPugMenu[Player->entindex()].Create(true, E_MENU::ME_ADMIN_MENU_SLAP, _T("Slap: ^w%s^y"), STRING(Target->edict()->v.netname));

				gPugMenu[Player->entindex()].AddItem(Target->entindex(), false, 0, _T("No Damage"));

                gPugMenu[Player->entindex()].AddItem(Target->entindex(), false, 1, _T("Slap 1 HP"));

                gPugMenu[Player->entindex()].AddItem(Target->entindex(), false, 5, _T("Slap 5 HP"));

                gPugMenu[Player->entindex()].AddItem(Target->entindex(), false, 10, _T("Slap 10 HP"));

                gPugMenu[Player->entindex()].AddItem(Target->entindex(), false, 25, _T("Slap 25 HP"));

                gPugMenu[Player->entindex()].AddItem(Target->entindex(), false, 50, _T("Slap 50 HP"));

                gPugMenu[Player->entindex()].AddItem(Target->entindex(), false, 100, _T("Kill Player"));

                gPugMenu[Player->entindex()].Show(Player);
            }
            else
            {
                auto Damage = static_cast<float>(Item.Extra);

                if (Target->edict()->v.health > Damage)
                {
                    gPugUtil.ClientSlap(Target->edict(), Damage, false);

                    Item.Extra = -1;

                    this->SlapHandle(Player, Item);
                }
                else
                {
                    gPugUtil.ClientSlap(Target->edict(), Damage, false);

                    this->Slap(Player);
                }
            }
        }
    }
}

void CPugAdminMenu::Team(CBasePlayer *Player)
{
    if (gPugAdmin.CheckAccess(Player, ADMIN_LEVEL_A))
    {
        auto Players = gPugUtil.GetPlayers(false, true);

        if (Players.size() < 1)
        {
            this->Menu(Player);

            gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 No player found."), gPugCvar.m_Tag->string);
            return;
        }

        gPugMenu[Player->entindex()].Create(true, E_MENU::ME_ADMIN_MENU_TEAM, _T("Player Team:"));

        for (auto Target : Players)
        {
            if (Target->m_iTeam != UNASSIGNED)
            {
                if (!gPugAdmin.Access(Target->entindex(), ADMIN_IMMUNITY))
                {
                    gPugMenu[Player->entindex()].AddItem(0, false, Target->entindex(), "%s \\R\\y%s", STRING(Target->edict()->v.netname), g_Pug_TeamNameShort[Target->m_iTeam]);
                }
            }
        }

        gPugMenu[Player->entindex()].Show(Player);
    }
}

void CPugAdminMenu::TeamHandle(CBasePlayer *Player, P_MENU_ITEM Item)
{
    if (Player)
    {
        auto Target = UTIL_PlayerByIndexSafe(Item.Extra);

        if (Target)
        {
            if (Item.Info == 0)
            {
                gPugMenu[Player->entindex()].Create(true, E_MENU::ME_ADMIN_MENU_TEAM, _T("Transfer Team: ^w%s^y"), STRING(Target->edict()->v.netname));

				gPugMenu[Player->entindex()].AddItem(TERRORIST, Target->m_iTeam == TERRORIST, Target->entindex(), g_Pug_TeamName[TERRORIST]);

                gPugMenu[Player->entindex()].AddItem(CT, Target->m_iTeam == CT, Target->entindex(), g_Pug_TeamName[CT]);

                gPugMenu[Player->entindex()].AddItem(SPECTATOR, Target->m_iTeam == SPECTATOR, Target->entindex(), g_Pug_TeamName[SPECTATOR]);

                gPugMenu[Player->entindex()].Show(Player);
            }
            else
            {
				if (Item.Info == SPECTATOR)
				{
					if (Target->IsAlive())
					{
						Target->edict()->v.deadflag = DEAD_DEAD;

                        Target->edict()->v.health = 0.0f;
					}
				}

                Target->CSPlayer()->JoinTeam((TeamName)Item.Info);

                this->Team(Player);
            }
        }
    }
}

void CPugAdminMenu::Map(CBasePlayer *Player)
{
    if (gPugAdmin.CheckAccess(Player, ADMIN_MAP))
    {
        auto MapList = gPugMapList.Get();

        if (MapList.empty())
        {
            gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 No map found in the list."), gPugCvar.m_Tag->string);
            return;
        }

        gPugMenu[Player->entindex()].Create(true, E_MENU::ME_ADMIN_MENU_MAP, _T("Change Map:"));

        for (const auto& Map : MapList)
        {
            gPugMenu[Player->entindex()].AddItem(Map.first, (Map.second.compare(STRING(gpGlobals->mapname)) == 0), 0, Map.second.c_str());
        }

        gPugMenu[Player->entindex()].Show(Player);
    }
}

void CPugAdminMenu::MapHandle(CBasePlayer *Player, P_MENU_ITEM Item)
{
    if (Player)
    {
        gPugTask.Create(E_TASK::MAP_CHANGE, 5.0f, false, Item.Info);

        gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Changing map to: ^3%s^1."), gPugCvar.m_Tag->string, Item.Text.c_str());
    }
}

void CPugAdminMenu::Pug(CBasePlayer *Player)
{
    if (gPugAdmin.CheckAccess(Player, ADMIN_VOTE|ADMIN_LEVEL_B))
    {
        gPugMenu[Player->entindex()].Create(true, E_MENU::ME_ADMIN_MENU_PUG, _T("Choose an operation:"));

        auto State = gPugMod.GetState();

        if (State >= STATE_DEAD && State <= STATE_KNIFE_ROUND)
        {
            gPugMenu[Player->entindex()].AddItem(STATE_DEAD, false, 0, _T("Disable Pug"));
            gPugMenu[Player->entindex()].AddItem(STATE_DEATHMATCH, false, 0, _T("Deathmatch"));
            gPugMenu[Player->entindex()].AddItem(STATE_VOTEMAP, false, 0, _T("Map Vote"));
            gPugMenu[Player->entindex()].AddItem(STATE_VOTETEAM, false, 0, _T("Team Vote"));
            gPugMenu[Player->entindex()].AddItem(STATE_CAPTAIN, false, 0, _T("Captain Pick"));
            gPugMenu[Player->entindex()].AddItem(STATE_KNIFE_ROUND, false, 0, _T("Knife Round"));
            gPugMenu[Player->entindex()].AddItem(STATE_FIRST_HALF, false, 0, _T("Start Match"));
        }
        else if (State >= STATE_FIRST_HALF && State <= STATE_OVERTIME)
        {
            gPugMenu[Player->entindex()].AddItem(STATE_FIRST_HALF, false, 0, _T("Restart Match"));
            gPugMenu[Player->entindex()].AddItem(STATE_DEAD, false, 0, _T("Cancel Match"));
            gPugMenu[Player->entindex()].AddItem(STATE_END, false, 0, _T("End Match"));
        }

        gPugMenu[Player->entindex()].Show(Player);
    }
}

void CPugAdminMenu::PugHandle(CBasePlayer *Player, P_MENU_ITEM Item)
{
    if (Player)
    {
        if (Item.Info >= STATE_DEAD && Item.Info <= STATE_END)
        {
            gPugTask.Create(E_TASK::SET_STATE, 1.0f, false, Item.Info);

            gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 State changed: ^3%s^1."), gPugCvar.m_Tag->string, gPugMod.GetString(Item.Info));
        }
    }
}
