#include "precompiled.h"

CPugVoteMap gPugVoteMap;

void CPugVoteMap::ServerActivate()
{
    this->m_Run = false;

    this->m_NextFrame = 0.0f;

    this->m_Time = 0;

    this->m_VotesLeft = 0;

    this->m_MapList.clear();
}

void CPugVoteMap::ServerDeactivate()
{
    this->m_Run = false;

    this->m_NextFrame = 0.0f;

    this->m_Time = 0;

    this->m_VotesLeft = 0;

    this->m_MapList.clear();
}

void CPugVoteMap::Init()
{
    this->LoadMaps();
    
    if (this->m_MapList.empty())
    {
        gPugMod.SetState(STATE_VOTETEAM);
    }
    else
    {
        this->m_Run = true;

        this->m_NextFrame = 0.0f;

        this->m_Time = time(nullptr) + static_cast<time_t>(gPugCvar.m_VoteDelay->value);

        if (g_pGameRules)
        {
            if (gPugCvar.m_MpFreezeTime)
            {
                g_engfuncs.pfnCvar_DirectSet(gPugCvar.m_MpFreezeTime, "120.0");
            }

            CSGameRules()->m_flRestartRoundTime = (gpGlobals->time + 0.1);
            CSGameRules()->m_bCompleteReset = true;
        }

        auto Players = gPugUtil.GetPlayers(true, false);

        this->m_VotesLeft = Players.size();

        for (auto const &Player : Players)
        {
            gPugMenu[Player->entindex()].Create(false, E_MENU::ME_VOTE_MAP, _T("Choose the map:"));

            for (size_t i = 0; i < this->m_MapList.size(); ++i)
            {
                this->m_MapList[i].Votes = 0;

                if (!this->m_MapList[i].This && !this->m_MapList[i].Last)
                {
                    gPugMenu[Player->entindex()].AddItem(i, false, i, this->m_MapList[i].Name.c_str());
                }
            }

            if (gPugCvar.m_SoundEnable->value != 0.0f)
            {
                gPugUtil.ClientCommand(Player->edict(), g_VoteMap_Sound[RANDOM_LONG(0, 1)]);
            }

            gPugMenu[Player->entindex()].Show(Player);
        }

        gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Starting new map selection."), gPugCvar.m_Tag->string);
    }
}

void CPugVoteMap::Stop()
{
    if (this->m_Run)
    {
        this->m_Run = false;

        this->m_NextFrame = 0.0f;

        auto Winner = this->GetWinner();

        if (gPugCvar.m_SoundEnable->value != 0.0f)
        {
            gPugUtil.ClientCommand(nullptr, g_VoteMap_Sound[2]);
        }

        if (Winner.Votes)
        {
            gPugTask.Create(E_TASK::MAP_CHANGE, 5.0f, false, Winner.Index);

            gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Changing map to: ^3%s^1."), gPugCvar.m_Tag->string, Winner.Name.c_str());
        }
        else
        {
            gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Selection failed: No votes."), gPugCvar.m_Tag->string);

            if (gPugUtil.GetPlayers(true, false).size() >= static_cast<size_t>(gPugCvar.m_PlayersMin->value))
            {
                gPugTask.Create(E_TASK::SET_STATE, 1.0f, false, STATE_VOTEMAP);

                gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Restarting map selection."), gPugCvar.m_Tag->string);
            }
            else
            {
                auto Random = this->GetRandom();

                gPugTask.Create(E_TASK::MAP_CHANGE, 5.0f, false, Random.Index);

                gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 The next map will be: ^3%s^1."), gPugCvar.m_Tag->string, Random.Name.c_str());
            }
        }
    }
}

void CPugVoteMap::LoadMaps()
{
    auto MapList = gPugMapList.Get();

    if (!MapList.empty())
    {
        this->m_MapList.clear();

        for (const auto& Map : MapList)
        {
            P_VOTE_MAP_INFO MapInfo;

            MapInfo.Index = Map.first;

            MapInfo.Votes = 0;

            MapInfo.Name = Map.second;

            MapInfo.This = (Map.second.compare(STRING(gpGlobals->mapname)) == 0);

            MapInfo.Last = (gPugCvar.m_LastMap->string && (Map.second.compare(gPugCvar.m_LastMap->string) == 0));

            this->m_MapList.push_back(MapInfo);
        }
    }
}

P_VOTE_MAP_INFO CPugVoteMap::GetWinner()
{
    P_VOTE_MAP_INFO Winner = {0, 0, STRING(gpGlobals->mapname), true};

    if (!this->m_MapList.empty())
    {
        for (auto const& Item : this->m_MapList)
        {
            if (Item.Votes >= Winner.Votes)
            {
                Winner = Item;
            }
            else if (Item.Votes == Winner.Votes)
            {
                if (RANDOM_LONG(0, 1))
                {
                    Winner = Item;
                }
            }
        }
    }

    return Winner;
}

P_VOTE_MAP_INFO CPugVoteMap::GetRandom()
{
    P_VOTE_MAP_INFO Map = {0, 0, STRING(gpGlobals->mapname), true};

    if (!this->m_MapList.empty())
    {
        do
        {
            Map = this->m_MapList.at(RANDOM_LONG(0, this->m_MapList.size() - 1));
        }
        while (Map.This || Map.Last);
    }

    return Map;
}

void CPugVoteMap::MenuHandle(CBasePlayer *Player, P_MENU_ITEM Item)
{
    if (this->m_Run)
    {
        if (Player)
        {
            this->m_VotesLeft -= 1;
            this->m_MapList[Item.Info].Votes += 1;

            gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 ^3%s^1 chose ^3%s^1."), gPugCvar.m_Tag->string, STRING(Player->edict()->v.netname), this->m_MapList[Item.Info].Name.c_str());

            if (this->m_VotesLeft < 1)
            {
                this->Stop();
            }
        }
    }
}

void CPugVoteMap::StartFrame()
{
    if (this->m_Run)
    {
        if (gpGlobals->time >= this->m_NextFrame)
        {
            if (!this->m_MapList.empty())
            {
                auto RemainTime = (this->m_Time - time(nullptr));

                if (RemainTime > 0)
                {
                    struct tm *tm_info = localtime(&RemainTime);

                    if (tm_info)
                    {
                        char szTime[32] = { 0 };

                        strftime(szTime, sizeof(szTime), "%M:%S", tm_info);
                        
                        gPugUtil.SendHud(nullptr, g_VoteMap_HudParam[0], _T("Map Choice: %s"), szTime);
                    }

                    std::string VoteList = "";

                    for (const auto &Item : this->m_MapList)
                    {
                        if (Item.Votes)
                        {
                            VoteList += "[" + std::to_string(Item.Votes) + "] " + Item.Name + "\n";
                        }
                    }

                    if (!VoteList.empty())
                    {
                        gPugUtil.SendHud(nullptr, g_VoteMap_HudParam[1], VoteList.c_str());
                    }
                    else
                    {
                        gPugUtil.SendHud(nullptr, g_VoteMap_HudParam[1], _T("No votes registered."));
                    }
                }
                else
                {
                    this->Stop();
                }
            }

            this->m_NextFrame = (gpGlobals->time + 0.9f);
        }
    }
}
