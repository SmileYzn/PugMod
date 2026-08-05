#include "precompiled.h"

CPugRoundStats gPugRoundStats;

void CPugRoundStats::RoundStart()
{
    auto State = gPugMod.GetState();

	if (State == STATE_FIRST_HALF || State == STATE_SECOND_HALF || State == STATE_OVERTIME)
	{
		this->m_RoundDmg = {};
		this->m_RoundHit = {};

        this->m_Flags = R_STATS_ALL;

        if (gPugCvar.m_RoundStats->string)
        {
            if (gPugCvar.m_RoundStats->string[0u] != '\0')
            {
                this->m_Flags |= gPugAdmin.ReadFlags(gPugCvar.m_RoundStats->string);
            }
        }
	}
}

void CPugRoundStats::RoundEnd(int winStatus, ScenarioEventEndRound eventScenario, float tmDelay)
{
    auto State = gPugMod.GetState();

	if (State == STATE_FIRST_HALF || State == STATE_SECOND_HALF || State == STATE_OVERTIME)
    {
        if (g_pGameRules)
        {
            if (CSGameRules()->m_bRoundTerminating)
            {
                auto Players = gPugUtil.GetPlayers(true, false);

                if (Players.size() > 0)
                {
                    for (auto const & Player : Players)
                    {
						if (Player->IsAlive())
						{
							this->StartDeathCam(Player);
						}
                    }
                }
            }
        }
    }
}

void CPugRoundStats::StartDeathCam(CBasePlayer *Player)
{
    auto State = gPugMod.GetState();

    if (State == STATE_FIRST_HALF || State == STATE_SECOND_HALF || State == STATE_OVERTIME)
    {
        if (Player)
        {
            if (g_pGameRules)
            {
                if (!Player->IsBot())
                {
                    if (gPugCvar.m_RoundEndStats->string)
                    {
                        switch (gPugCvar.m_RoundEndStats->string[0u])
                        {
                            case 'a':
                            {
                                this->ShowHP(Player);
                                break;
                            }
                            case 'b':
                            {
                                this->ShowDamage(Player);
                                break;
                            }
                            case 'c':
                            {
                                this->ShowReceivedDamage(Player);
                                break;
                            }
                            case 'd':
                            {
                                this->ShowSummary(Player);
                                break;
                            }
                            case 'e':
                            {
                                this->ShowStats(Player);
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
}

void CPugRoundStats::GetIntoGame(CBasePlayer *Player)
{
    this->m_RoundDmg[Player->entindex()] = {};
    this->m_RoundHit[Player->entindex()] = {};
}

void CPugRoundStats::DropClient(edict_t *pEntity)
{
	if (!FNullEnt(pEntity))
	{
		auto EntityIndex = ENTINDEX(pEntity);

		if (EntityIndex > 0 && EntityIndex <= gpGlobals->maxClients)
		{
			for (auto i = 1; i <= gpGlobals->maxClients; i++)
			{
				this->m_RoundDmg[i][EntityIndex] = 0;
				this->m_RoundHit[i][EntityIndex] = 0;
			}
		}
	}
}

void CPugRoundStats::TakeDamage(CBasePlayer *Player, entvars_t *pevInflictor, entvars_t *pevAttacker, float &flDamage, int bitsDamageType)
{
    auto State = gPugMod.GetState();

	if (State == STATE_FIRST_HALF || State == STATE_SECOND_HALF || State == STATE_OVERTIME)
	{
        auto Attacker = UTIL_PlayerByIndexSafe(ENTINDEX(pevAttacker));

		if (!Player->m_bKilledByBomb)
		{
			if (Attacker)
			{
				if (Player->entindex() != Attacker->entindex())
				{
					if (CSGameRules()->FPlayerCanTakeDamage(Player, Attacker))
					{
						this->m_RoundDmg[Attacker->entindex()][Player->entindex()] += flDamage;
						this->m_RoundHit[Attacker->entindex()][Player->entindex()] += 1;
					}
				}
			}
		}
		else
		{
            gPugUtil.SendDeathMsg(NULL, NULL, Player, NULL, NULL, "c4", 0, 0);
		}
	}
}

bool CPugRoundStats::ShowHP(CBasePlayer *Player)
{
    auto State = gPugMod.GetState();

    if (State == STATE_FIRST_HALF || State == STATE_SECOND_HALF || State == STATE_OVERTIME)
    {
        if (this->m_Flags & R_STATS_HP)
        {
            if (g_pGameRules)
            {
                if (!Player->IsAlive() || CSGameRules()->m_bRoundTerminating)
                {
                    if (Player->m_iTeam == TERRORIST || Player->m_iTeam == CT)
                    {
                        auto StatsCount = 0;

                        auto Players = gPugUtil.GetPlayers(true, true);

                        for (auto Target : Players)
                        {
                            if (Target->m_iTeam != Player->m_iTeam)
                            {
                                if (Target->IsAlive())
                                {
                                    StatsCount++;

                                    gPugUtil.PrintColor(Player->edict(), Target->entindex(), _T("^4[%s]^1 ^3%s^1 with %d HP (%d AP)"), gPugCvar.m_Tag->string, STRING(Target->edict()->v.netname), (int)Target->edict()->v.health, (int)Target->edict()->v.armorvalue);
                                }
                            }
                        }

                        if (!StatsCount)
                        {
                            gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 No player alive."), gPugCvar.m_Tag->string);
                        }

                        return true;
                    }
                }
            }
        }
    }

    gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Command unavailable."), gPugCvar.m_Tag->string);

	return false;
}

bool CPugRoundStats::ShowDamage(CBasePlayer *Player)
{
    auto State = gPugMod.GetState();
        
    if (State == STATE_FIRST_HALF || State == STATE_SECOND_HALF || State == STATE_OVERTIME)
    {
        if (this->m_Flags & R_STATS_DMG)
        {
            if (g_pGameRules)
            {
                if (!Player->IsAlive() || CSGameRules()->m_bRoundTerminating || CSGameRules()->IsFreezePeriod())
                {
                    if (Player->m_iTeam == TERRORIST || Player->m_iTeam == CT)
                    {
                        auto StatsCount = 0;

                        auto Players = gPugUtil.GetPlayers(true, true);

                        for (auto Target : Players)
                        {
                            if (this->m_RoundHit[Player->entindex()][Target->entindex()])
                            {
                                StatsCount++;

                                gPugUtil.PrintColor
                                (
                                    Player->edict(),
                                    Target->entindex(),
                                    _T("^4[%s]^1 Hit ^3%s^1 %d time(s) (Damage %d)"),
                                    gPugCvar.m_Tag->string,
                                    STRING(Target->edict()->v.netname),
                                    this->m_RoundHit[Player->entindex()][Target->entindex()],
                                    this->m_RoundDmg[Player->entindex()][Target->entindex()]
                                );
                            }
                        }

                        if (!StatsCount)
                        {
                            gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 No damage this round."), gPugCvar.m_Tag->string);
                        }

                        return true;
                    }
                }
            }
        }
    }

    gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Command unavailable."), gPugCvar.m_Tag->string);

	return false;
}

bool CPugRoundStats::ShowReceivedDamage(CBasePlayer *Player)
{
    auto State = gPugMod.GetState();
        
    if (State == STATE_FIRST_HALF || State == STATE_SECOND_HALF || State == STATE_OVERTIME)
    {
        if (this->m_Flags & R_STATS_RDMG)
        {
            if (g_pGameRules)
            {
                if (!Player->IsAlive() || CSGameRules()->m_bRoundTerminating || CSGameRules()->IsFreezePeriod())
                {
                    if (Player->m_iTeam == TERRORIST || Player->m_iTeam == CT)
                    {
                        auto StatsCount = 0;

                        auto Players = gPugUtil.GetPlayers(true, true);

                        for (auto Target : Players)
                        {
                            if (this->m_RoundHit[Target->entindex()][Player->entindex()])
                            {
                                StatsCount++;

                                gPugUtil.PrintColor
                                (
                                    Player->edict(),
                                    Target->entindex(),
                                    _T("^4[%s]^1 Damage received from ^3%s^1 %d time(s) (Damage %d)"),
                                    gPugCvar.m_Tag->string,
                                    STRING(Target->edict()->v.netname),
                                    this->m_RoundHit[Target->entindex()][Player->entindex()],
                                    this->m_RoundDmg[Target->entindex()][Player->entindex()]
                                );
                            }
                        }

                        if (!StatsCount)
                        {
                            gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 No damage received this round."), gPugCvar.m_Tag->string);
                        }

                        return true;
                    }
                }
            }
        }
    }

    gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Command unavailable."), gPugCvar.m_Tag->string);

	return false;
}

bool CPugRoundStats::ShowSummary(CBasePlayer *Player)
{
    auto State = gPugMod.GetState();
        
    if (State == STATE_FIRST_HALF || State == STATE_SECOND_HALF || State == STATE_OVERTIME)
    {
        if (this->m_Flags & R_STATS_SUM)
        {
            if (g_pGameRules)
            {
                if (!Player->IsAlive() || CSGameRules()->m_bRoundTerminating || CSGameRules()->IsFreezePeriod())
                {
                    if (Player->m_iTeam == TERRORIST || Player->m_iTeam == CT)
                    {
                        auto StatsCount = 0;
    
                        auto Players = gPugUtil.GetPlayers(true, true);
    
                        for (auto Target : Players)
                        {
                            if (this->m_RoundHit[Player->entindex()][Target->entindex()] || this->m_RoundHit[Target->entindex()][Player->entindex()])
                            {
                                StatsCount++;
    
                                gPugUtil.PrintColor
                                (
                                    Player->edict(),
                                    Target->entindex(),
                                    _T("^4[%s]^1 (%d dmg | %d hits) to (%d dmg | %d hits) from ^3%s^1 (%d HP)"),
                                    gPugCvar.m_Tag->string,
                                    this->m_RoundDmg[Player->entindex()][Target->entindex()],
                                    this->m_RoundHit[Player->entindex()][Target->entindex()],
                                    this->m_RoundDmg[Target->entindex()][Player->entindex()],
                                    this->m_RoundHit[Target->entindex()][Player->entindex()],
                                    STRING(Target->edict()->v.netname),
                                    Target->IsAlive() ? (int)Target->edict()->v.health : 0
                                );
                            }
                        }
    
                        if (!StatsCount)
                        {
                            gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 No damage this round."), gPugCvar.m_Tag->string);
                        }
    
                        return true;
                    }
                }
            }
        }
    }

    gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Command unavailable."), gPugCvar.m_Tag->string);

	return false;
}

bool CPugRoundStats::ShowStats(CBasePlayer *Player)
{
    auto State = gPugMod.GetState();
        
    if (State == STATE_FIRST_HALF || State == STATE_SECOND_HALF || State == STATE_OVERTIME)
    {
        if (this->m_Flags & R_STATS_STATS)
        {
            if (g_pGameRules)
            {
                if (!Player->IsAlive() || CSGameRules()->m_bRoundTerminating || CSGameRules()->IsFreezePeriod())
                {
                    if (!Player->IsBot())
                    {
                        if (Player->m_iTeam == TERRORIST || Player->m_iTeam == CT)
                        {
                            auto Players = gPugUtil.GetPlayers(true, true);
    
                            if (Players.size() > 0)
                            {
                                char HudList[3][511] = {};
    
                                Q_memset(HudList, 0, sizeof(HudList));
    
                                auto EntityIndex = Player->entindex();
    
                                for (auto const& Target : Players)
                                {
                                    auto TargetId = Target->entindex();
    
                                    if (this->m_RoundHit[EntityIndex][TargetId])
                                    {
                                        Q_snprintf(HudList[0] + strlen(HudList[0]), sizeof(HudList[0]), _T("%s -- %d hit(s) / %d dmg\n"), STRING(Target->edict()->v.netname), this->m_RoundHit[EntityIndex][TargetId], this->m_RoundDmg[EntityIndex][TargetId]);
                                    }
    
                                    if (this->m_RoundHit[TargetId][EntityIndex])
                                    {
                                        Q_snprintf(HudList[1] + strlen(HudList[1]), sizeof(HudList[1]), _T("%s -- %d hit(s) / %d dmg\n"), STRING(Target->edict()->v.netname), this->m_RoundHit[TargetId][EntityIndex], this->m_RoundDmg[TargetId][EntityIndex]);
                                    }
                                }

                                if (HudList[0][0u] != '\0')
                                {
                                    Q_snprintf(HudList[2], sizeof(HudList[2]), _T("Victims:^n%s"), HudList[0]);
                                }
    
                                if (HudList[1][0u] != '\0')
                                {
                                    Q_snprintf(HudList[2] + strlen(HudList[2]), sizeof(HudList[2]), _T("^n^n^n^nAttackers:\n%s"), HudList[1]);
                                }
    
                                if (HudList[2][0u] != '\0')
                                {
                                    gPugUtil.SendHud(Player->edict(), g_RoundStats_HudParam, HudList[2]);
                                }
                            }
    
                            return true;
                        }
                    }
                }
            }
        }
    }

    return false;
}
