#include "precompiled.h"

CPugTeamManager gPugTeamManager;

void CPugTeamManager::ServerActivate()
{
    this->m_Data.clear();
}

void CPugTeamManager::ServerDeactivate()
{
    this->m_Data.clear();
}

bool CPugTeamManager::ChooseTeam(CBasePlayer *Player, int Slot)
{
    if (Player->m_iTeam == UNASSIGNED)
    {
        if (!Player->IsBot())
        {
            gPugUtil.TeamInfo(Player->edict(), MAX_CLIENTS + TERRORIST + 1, "TERRORIST");
            gPugUtil.TeamInfo(Player->edict(), MAX_CLIENTS + CT + 1, "CT");
        }
    }

	if (Slot == MENU_SLOT_TEAM_TERRORIST || Slot == MENU_SLOT_TEAM_CT)
	{
        auto Players = gPugUtil.GetPlayers();

		if (Players[Slot].size() >= static_cast<size_t>(gPugCvar.m_PlayersMax->value))
		{
			gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Team ^3%s^1 is already full."), gPugCvar.m_Tag->string, g_Pug_TeamName[Slot]);
			return true;
		}
	}

    if (Slot == MENU_SLOT_TEAM_RANDOM)
    {
        gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Auto selection not allowed."), gPugCvar.m_Tag->string);
        return true;
    }

    if (Slot == MENU_SLOT_TEAM_SPECT)
    {
        if (gPugCvar.m_AllowSpectators->value < 1.0f)
        {
            gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Spectators not allowed."), gPugCvar.m_Tag->string);
            return true;
        }
    }

    if (Player->m_iTeam == Slot)
    {
        gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 You are already on team ^3%s^1."), gPugCvar.m_Tag->string, g_Pug_TeamName[Player->m_iTeam]);
        return true;
    }

    auto State = gPugMod.GetState();

    if (Player->m_iTeam == TERRORIST || Player->m_iTeam == CT)
    {
        if (State >= STATE_VOTEMAP && State <= STATE_OVERTIME)
        {
            gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Team switching not allowed during the match."), gPugCvar.m_Tag->string);
            return true;
        }
    }
    else if (Player->m_iTeam == UNASSIGNED || Player->m_iTeam == SPECTATOR)
    {
        auto Auth = GETPLAYERAUTHID(Player->edict());

        if (Auth)
        {
            if (Auth[0u] != '\0')
            {
                if (this->m_Data.find(Auth) != this->m_Data.end())
                {
                    if (Slot != MENU_SLOT_TEAM_SPECT)
                    {
                        if (Slot != this->m_Data[Auth])
                        {
                            if (!gPugAdmin.Access(Player->entindex(), ADMIN_IMMUNITY))
                            {
                                gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Team change by reconnect is not allowed."), gPugCvar.m_Tag->string);
                                return true;
                            }
                        }
                    }
                }
            }
        }
    }

    return false;
}

void CPugTeamManager::DropClient(edict_t *pEntity)
{
    auto Player = UTIL_PlayerByIndexSafe(ENTINDEX(pEntity));

    if (Player)
    {
        if (Player->m_iTeam == TERRORIST || Player->m_iTeam == CT)
        {
            auto Auth = GETPLAYERAUTHID(Player->edict());

            if (Auth)
            {
                if (Auth[0u] != '\0')
                {
                    if (this->m_Data.find(Auth) == this->m_Data.end())
                    {
                        auto State = gPugMod.GetState();

                        if (State >= STATE_KNIFE_ROUND && State <= STATE_OVERTIME)
                        {
                            this->m_Data.insert(std::make_pair(Auth, static_cast<int>(Player->m_iTeam)));
                        }
                    }
                }
            }
        }
    }
}
