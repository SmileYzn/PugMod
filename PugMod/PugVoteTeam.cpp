#include "precompiled.h"

CPugVoteTeam gPugVoteTeam;

void CPugVoteTeam::ServerActivate()
{
    this->m_Run = false;

    this->m_NextFrame = 0.0f;

    this->m_Time = 0;

    this->m_VotesLeft = 0;

    this->m_VoteList.push_back({VOTE_TEAM_CAPTAIN, 0, _T("Captains")});
    this->m_VoteList.push_back({VOTE_TEAM_RANDOM, 0, _T("Shuffle Teams")});
    this->m_VoteList.push_back({VOTE_TEAM_UNSORTED, 0, _T("No Draw")});
    this->m_VoteList.push_back({VOTE_TEAM_SKILL_BALANCE, 0, _T("Balance Skill")});
    this->m_VoteList.push_back({VOTE_TEAM_SWAP_SIDES, 0, _T("Swap Teams")});
    this->m_VoteList.push_back({VOTE_TEAM_KNIFE_ROUND, 0, _T("Knife Round")});
}

void CPugVoteTeam::ServerDeactivate()
{
    this->m_Run = false;

    this->m_NextFrame = 0.0f;

    this->m_Time = 0;

    this->m_VotesLeft = 0;

    this->m_VoteList.clear();
}

void CPugVoteTeam::Init()
{
    auto TeamType = static_cast<int>(gPugCvar.m_TeamType->value);

    if (TeamType > 0)
    {
        this->ChangeTeam(TeamType);
    }
    else
    {
        this->m_Run = true;

        this->m_NextFrame = 0.0f;

        this->m_Time = time(nullptr) + static_cast<time_t>(gPugCvar.m_VoteDelay->value);

        this->m_VotesLeft = 0;

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

        auto MenuFlags = gPugAdmin.ReadFlags(gPugCvar.m_TeamOption->string);

        this->m_VotesLeft = Players.size();

        for (auto const &Player : Players)
        {
            gPugMenu[Player->entindex()].Create(false, E_MENU::ME_VOTE_TEAM, _T("Game mode:"));

            for (size_t i = 0; i < this->m_VoteList.size(); ++i)
            {
                this->m_VoteList[i].Votes = 0;

                if (MenuFlags == 0 || (MenuFlags & BIT(i)))
                {
                    gPugMenu[Player->entindex()].AddItem(i, false, i, this->m_VoteList[i].Name.c_str());
                }
            }

            if (gPugCvar.m_SoundEnable->value != 0.0f)
            {
                gPugUtil.ClientCommand(Player->edict(), g_VoteTeam_Sound[RANDOM_LONG(0, 1)]);
            }

            gPugMenu[Player->entindex()].Show(Player);
        }

        gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Starting team selection."), gPugCvar.m_Tag->string);
    }
}

void CPugVoteTeam::Stop()
{
    if (this->m_Run)
    {
        this->m_Run = false;

        this->m_NextFrame = 0.0f;

        this->m_Time = 0;

        this->m_VotesLeft = 0;

        auto Winner = this->GetWinner();

        if (gPugCvar.m_SoundEnable->value != 0.0f)
        {
            gPugUtil.ClientCommand(nullptr, g_VoteTeam_Sound[2]);
        }

        if (Winner.Votes)
        {
            this->ChangeTeam(Winner.Index);
        }
        else
        {
            gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Selection failed: ^3No votes."), gPugCvar.m_Tag->string);

            this->ChangeTeam(VOTE_TEAM_RANDOM);
        }
    }
}

void CPugVoteTeam::ChangeTeam(int Type)
{
    auto NewState = (gPugCvar.m_KnifeRound->value > 0.0f) ? STATE_KNIFE_ROUND : STATE_FIRST_HALF;

    switch(Type)
    {
        case VOTE_TEAM_CAPTAIN:
        {
            NewState = STATE_CAPTAIN;
            break;
        }
        case VOTE_TEAM_RANDOM:
        {
            this->TeamsRamdomize();

            gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Organizing players automatically."), gPugCvar.m_Tag->string);
            break;
        }
        case VOTE_TEAM_UNSORTED:
        {
            gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Teams are already set."), gPugCvar.m_Tag->string);
            break;
        }
        case VOTE_TEAM_SKILL_BALANCE:
        {
            this->TeamsOptimize();

            gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Organizing players by skill."), gPugCvar.m_Tag->string);
            break;
        }
        case VOTE_TEAM_SWAP_SIDES:
        {
            if (g_pGameRules)
            {
                CSGameRules()->SwapAllPlayers();
            }
            
            gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Switching team sides."), gPugCvar.m_Tag->string);
            break;
        }
        case VOTE_TEAM_KNIFE_ROUND:
        {
            NewState = STATE_KNIFE_ROUND;
            break;
        }
    }

    g_engfuncs.pfnCvar_DirectSet(gPugCvar.m_PlayGameMode, std::to_string(Type).c_str());

    gPugTask.Create(E_TASK::SET_STATE, 2.0f, false, NewState);
}

P_VOTE_TEAM_INFO CPugVoteTeam::GetWinner()
{
    P_VOTE_TEAM_INFO Winner = {};

    if (!this->m_VoteList.empty())
    {
        for (auto const& Item : this->m_VoteList)
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

void CPugVoteTeam::MenuHandle(CBasePlayer *Player, P_MENU_ITEM Item)
{
    if (this->m_Run)
    {
        if (Player)
        {
            this->m_VotesLeft -= 1;

            this->m_VoteList[Item.Info].Votes += 1;

            gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 ^3%s^1 chose ^3%s^1."), gPugCvar.m_Tag->string, STRING(Player->edict()->v.netname), this->m_VoteList[Item.Info].Name.c_str());

            if (this->m_VotesLeft < 1)
            {
                this->Stop();
            }
        }
    }
}

void CPugVoteTeam::StartFrame()
{
    if (this->m_Run)
    {
        if (gpGlobals->time >= this->m_NextFrame)
        {
            if (!this->m_VoteList.empty())
            {
                auto RemainTime = (this->m_Time - time(nullptr));

                if (RemainTime > 0)
                {
                    struct tm *tm_info = localtime(&RemainTime);

                    if (tm_info)
                    {
                        char szTime[32] = { 0 };

                        strftime(szTime, sizeof(szTime), "%M:%S", tm_info);
                        
                        gPugUtil.SendHud(nullptr, g_VoteTeam_HudParam[0], _T("Team Choice: %s"), szTime);
                    }

                    std::string VoteList = "";

                    for (const auto &Item : this->m_VoteList)
                    {
                        if (Item.Votes)
                        {
                            VoteList += "[" + std::to_string(Item.Votes) + "] " + Item.Name + "\n";
                        }
                    }

                    if (!VoteList.empty())
                    {
                        gPugUtil.SendHud(nullptr, g_VoteTeam_HudParam[1], VoteList.c_str());
                    }
                    else
                    {
                        gPugUtil.SendHud(nullptr, g_VoteTeam_HudParam[1], _T("No votes registered."));
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

void CPugVoteTeam::TeamsRamdomize()
{
    auto Players = gPugUtil.GetPlayers(true, true);

    if (Players.size())
    {
        TeamName Team = static_cast<TeamName>(RANDOM_LONG(TERRORIST, CT));

        do
        {
            auto i = RANDOM_LONG(0, Players.size() - 1);

			Players[i]->edict()->v.deadflag = DEAD_DEAD;

			Players[i]->CSPlayer()->JoinTeam(Team);

			Players[i]->edict()->v.deadflag = DEAD_NO;

            Players.erase(Players.begin() + i);

            Team = static_cast<TeamName>(Team % CT + TERRORIST);
        }
        while (Players.size());
    }
}

void CPugVoteTeam::TeamsOptimize()
{
	auto Players = gPugUtil.GetPlayers(true, true);

    if (Players.size())
    {
        std::array<float, MAX_CLIENTS + 1U> Skills = {};

        std::array<float, MAX_CLIENTS + 1U> Sorted = {};

        for (auto const & Player : Players)
        {
            Sorted[Player->entindex()] = Skills[Player->entindex()] = (Player->edict()->v.frags ? (100.0f * Player->edict()->v.frags / (Player->edict()->v.frags + static_cast<float>(Player->m_iDeaths))) : 0.0f);
        }

        std::sort(Sorted.begin(), Sorted.end(), std::greater<float>());

        TeamName Team = static_cast<TeamName>(RANDOM_LONG(TERRORIST, CT));

        for (size_t i = 0; i < Sorted.size(); i++)
        {
            for (size_t j = 0;j < Players.size(); ++j)
            {
                if (Skills[j] == Sorted[i])
                {
                    Players[j]->edict()->v.deadflag = DEAD_DEAD;

                    Players[j]->CSPlayer()->JoinTeam(Team);

                    Players[j]->edict()->v.deadflag = DEAD_NO;

                    Players.erase(Players.begin() + j);

                    Team = static_cast<TeamName>(Team % CT + TERRORIST);
                }
            }
        }
    }
}