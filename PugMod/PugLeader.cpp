#include "precompiled.h"

CPugLeader gPugLeader;

void CPugLeader::ServerActivate()
{
    this->m_Run = false;

    this->m_NextFrame = 0.0f;

    this->m_PlayersMin = 0;

	this->m_Team.clear();
}

void CPugLeader::ServerDeactivate()
{
    this->m_Run = false;

    this->m_NextFrame = 0.0f;

    this->m_PlayersMin = 0;

	this->m_Team.clear();
}

void CPugLeader::Init()
{
	auto Players = gPugUtil.GetPlayers(false, true);

    this->m_PlayersMin = static_cast<unsigned int>(gPugCvar.m_PlayersMin->value);

	if (Players.size() > this->m_PlayersMin) // this->m_PlayersMin = 5
	{
		this->m_Run = true;

        this->m_NextFrame = 0.0f;

		this->m_Team.clear();

		g_engfuncs.pfnCvar_DirectSet(gPugCvar.m_AutoTeamJoin, "1");

		g_engfuncs.pfnCvar_DirectSet(gPugCvar.m_HumansJoinTeam, "SPEC");

		g_engfuncs.pfnCvar_DirectSet(gPugCvar.m_BotJoinTeam, "SPEC");

		g_engfuncs.pfnCvar_DirectSet(gPugCvar.m_AllowSpectators, "1");

		for (auto & Player : Players)
		{
			if (Player->m_iTeam != UNASSIGNED)
			{
				this->m_Team[Player->entindex()] = Player->m_iTeam;

				Player->edict()->v.deadflag = DEAD_DEAD;

				Player->CSPlayer()->JoinTeam(SPECTATOR);

				Player->ClearConditions(BIT_LEADER_TYPE);

				Player->ClearConditions(BIT_LEADER_MENU);
			}
		}

		int PlayerTR = RANDOM_LONG(0, Players.size() - 1);

		int PlayerCT = -1;

		do
		{
			PlayerCT = RANDOM_LONG(0, Players.size() - 1);
		}
		while (PlayerTR == PlayerCT);

		this->SetLeader(Players.at(PlayerTR), TERRORIST);

		this->SetLeader(Players.at(PlayerCT), CT);

		this->Menu(Players.at(RANDOM_LONG(0, 1) == 0 ? PlayerTR : PlayerCT));
	}
	else
	{
		this->Stop();
	}
}

void CPugLeader::Stop()
{
	if (this->m_Run)
	{
		this->m_Run = false;

		this->m_NextFrame = 0.0f;

		g_engfuncs.pfnCvar_DirectSet(gPugCvar.m_AutoTeamJoin, "0");

		g_engfuncs.pfnCvar_DirectSet(gPugCvar.m_HumansJoinTeam, "any");

		g_engfuncs.pfnCvar_DirectSet(gPugCvar.m_BotJoinTeam, "any");

		g_engfuncs.pfnCvar_DirectSet(gPugCvar.m_AllowSpectators, "0");

		auto Players = gPugUtil.GetPlayers();

		if ((Players[TERRORIST].size() >= this->m_PlayersMin) && (Players[CT].size() >= this->m_PlayersMin))
		{
			if (gPugCvar.m_KnifeRound->value)
			{
				gPugTask.Create(E_TASK::SET_STATE, 2.0f, false, STATE_KNIFE_ROUND);
			}
			else
			{
				gPugTask.Create(E_TASK::SET_STATE, 2.0f, false, STATE_FIRST_HALF);
			}

			gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Teams are full."), gPugCvar.m_Tag->string);
		}
		else
		{
			if (!this->m_Team.empty())
			{
				for (auto const & Backup : this->m_Team)
				{
					auto Player = UTIL_PlayerByIndexSafe(Backup.first);

					if (Player)
					{
						if (Player->m_iTeam != Backup.second)
						{
							Player->edict()->v.deadflag = DEAD_DEAD;
							
							Player->CSPlayer()->JoinTeam(static_cast<TeamName>(Backup.second));

							Player->edict()->v.deadflag = DEAD_NO;
						}
					}
				}

				this->m_Team.clear();
			}

			gPugTask.Create(E_TASK::SET_STATE, 2.0f, false, STATE_DEATHMATCH);

			gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Pick failed: Teams are incomplete.^1"), gPugCvar.m_Tag->string);
		}
	}
}

void CPugLeader::PutInServer(edict_t *pEntity)
{
	if (this->m_Run)
	{
		auto Player = UTIL_PlayerByIndexSafe(ENTINDEX(pEntity));

		if (Player)
		{
			Player->ClearConditions(BIT_LEADER_TYPE);
			Player->ClearConditions(BIT_LEADER_MENU);
		}
	}
}

void CPugLeader::DropClient(edict_t *pEntity)
{
	if (this->m_Run)
	{
		auto Leader = UTIL_PlayerByIndexSafe(ENTINDEX(pEntity));

		if (Leader->HasConditions(BIT_LEADER_TYPE))
		{
			auto Players = gPugUtil.GetPlayers();

			if (Players[SPECTATOR].size() > 0)
			{
				this->SetLeader(Players[SPECTATOR].at(0), Leader->m_iTeam);

				if (Leader->HasConditions(BIT_LEADER_MENU))
				{
					this->Menu(Players[SPECTATOR].at(0));
				}
			}
			else
			{
				this->Stop();
			}
		}
	}
}

void CPugLeader::SetLeader(CBasePlayer* Player, int Team)
{
    if (Player)
    {
        Player->CSPlayer()->JoinTeam(static_cast<TeamName>(Team));

        if (!Player->IsAlive())
        {
            Player->RoundRespawn();
        }

        Player->SetConditions(BIT_LEADER_TYPE);

        Player->ClearConditions(BIT_LEADER_MENU);

        gPugUtil.PrintColor(nullptr, (Team == TERRORIST ? E_PRINT_TEAM::RED : E_PRINT_TEAM::BLUE), _T("^4[%s]^1 New captain of ^3%s^1: %s"), gPugCvar.m_Tag->string, g_Pug_TeamName[Team], STRING(Player->edict()->v.netname));
    }
}

void CPugLeader::GetPlayer(CBasePlayer* Leader, CBasePlayer* Target)
{
	if (Leader)
	{
		gPugTask.Remove(E_TASK::LEADER_MENU);

		Leader->ClearConditions(BIT_LEADER_MENU);

		gPugMenu[Leader->entindex()].Hide(Leader);

		if (Target)
		{
			if (Target->m_iTeam == SPECTATOR)
			{
				Target->CSPlayer()->JoinTeam(Leader->m_iTeam);

				if (!Target->IsAlive())
				{
					Target->RoundRespawn();
				}

				gPugUtil.PrintColor(nullptr, (Leader->m_iTeam == TERRORIST ? E_PRINT_TEAM::RED : E_PRINT_TEAM::BLUE), _T("^4[%s]^1 ^3%s^1 picked ^3%s^1"), gPugCvar.m_Tag->string, STRING(Leader->edict()->v.netname), STRING(Target->edict()->v.netname));

				this->Next(Leader);
			}
			else
			{
				gPugUtil.PrintColor(nullptr, (Target->m_iTeam == TERRORIST ? E_PRINT_TEAM::RED : E_PRINT_TEAM::BLUE), _T("^4[%s]^1 ^3%s^1 is already on a team."), gPugCvar.m_Tag->string, STRING(Target->edict()->v.netname));

				this->Menu(Leader);
			}
		}
		else
		{
			auto Players = gPugUtil.GetPlayers();

			if (Players[SPECTATOR].size() > 0)
			{
				this->GetPlayer(Leader, Players[SPECTATOR].at(0));
			}
		}
	}
}

void CPugLeader::Next(CBasePlayer* Leader)
{
	auto Players = gPugUtil.GetPlayers();

	if (Players[SPECTATOR].size() > 0)
	{
		for (auto const& Player : Players[Leader->m_iTeam == TERRORIST ? CT : TERRORIST])
		{
			if (Player->HasConditions(BIT_LEADER_TYPE))
			{
				if (Players[Player->m_iTeam].size() < this->m_PlayersMin)
				{
					this->Menu(Player);
					return;
				}
			}
		}

		if (Players[Leader->m_iTeam].size() < this->m_PlayersMin)
		{
			this->Menu(Leader);
			return;
		}
	}
    else
    {
        this->Stop();
    }
}

void CPugLeader::Menu(CBasePlayer* Leader)
{
    if (Leader)
    {
		auto Players = gPugUtil.GetPlayers();

		if (Players[SPECTATOR].size() > 0)
		{
			Leader->SetConditions(BIT_LEADER_MENU);

			if (Players[SPECTATOR].size() == 1 || Leader->IsBot())
			{
				gPugTask.Create(E_TASK::LEADER_MENU, 2.0f, false, Leader->entindex());
			}
			else
			{
				gPugMenu[Leader->entindex()].Create(false, E_MENU::ME_VOTE_LEADER, _T("Players:"));

				for (auto const& Player : Players[SPECTATOR])
				{
					gPugMenu[Leader->entindex()].AddItem(Player->entindex(), false, Player->m_iTeam, STRING(Player->edict()->v.netname));
				}

				gPugMenu[Leader->entindex()].Show(Leader);

				gPugTask.Create(E_TASK::LEADER_MENU, 10.0f, false, Leader->entindex());

				gPugUtil.PrintColor(Leader->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Choose a player within 10 seconds."), gPugCvar.m_Tag->string);
			}
		}
		else
		{
			this->Stop();
		}
    }
}

void CPugLeader::MenuHandle(CBasePlayer *Player, P_MENU_ITEM Item)
{
	if (Player)
	{
		auto Spectactor = UTIL_PlayerByIndexSafe(Item.Info);

		if (Spectactor)
		{
			this->GetPlayer(Player, Spectactor);
		}
	}
}

void CPugLeader::GetRandomPlayer(int EntityIndex)
{
	auto Player = UTIL_PlayerByIndexSafe(EntityIndex);

	if (Player)
	{
		this->GetPlayer(Player, nullptr);
	}
}

void CPugLeader::StartFrame()
{
    if (this->m_Run)
    {
        if (gpGlobals->time >= this->m_NextFrame)
        {
            std::map<int, std::string> PlayerList = {};

            std::map<int, int> Count = {};

            for (auto i = 1; i <= gpGlobals->maxClients; ++i)
            {
                auto Player = UTIL_PlayerByIndexSafe(i);

                if (Player)
                {
                    if (!Player->IsDormant() && (Player->m_iTeam != UNASSIGNED))
                    {
						Count[Player->m_iTeam]++;

						PlayerList[Player->m_iTeam] += STRING(Player->edict()->v.netname);

						if (Player->HasConditions(BIT_LEADER_TYPE))
						{
							PlayerList[Player->m_iTeam] += " (C) ";

							if (Player->HasConditions(BIT_LEADER_MENU))
							{
								PlayerList[Player->m_iTeam] += "*";
							}
						}

						PlayerList[Player->m_iTeam] += "\n";
                    }
                }
            }

			PlayerList[CT] += std::string((5 - Count[CT]), '\n');

            gPugUtil.SendHud(nullptr, g_Leader_HudParam[0], "%s (%d / %d):", g_Pug_TeamName[TERRORIST], Count[TERRORIST], this->m_PlayersMin);

            gPugUtil.SendHud(nullptr, g_Leader_HudParam[1], "\n%s", PlayerList[TERRORIST].c_str());

            if (Count[SPECTATOR] < 1)
            {
                gPugUtil.SendHud(nullptr, g_Leader_HudParam[2], "%s (%d / %d):", g_Pug_TeamName[CT], Count[CT], this->m_PlayersMin);

                gPugUtil.SendHud(nullptr, g_Leader_HudParam[3], "\n%s", PlayerList[CT].c_str());
            }
            else
            {
                gPugUtil.SendHud(nullptr, g_Leader_HudParam[4], "%s (%d / %d):\n\n\n\n\n\n%s (%d):", g_Pug_TeamName[CT], Count[CT], this->m_PlayersMin, g_Pug_TeamName[SPECTATOR], Count[SPECTATOR]);

                gPugUtil.SendHud(nullptr, g_Leader_HudParam[5], "\n%s\n%s", PlayerList[CT].c_str(), PlayerList[SPECTATOR].c_str());
            }

            this->m_NextFrame = (gpGlobals->time + 0.9f);
        }
    }
}
