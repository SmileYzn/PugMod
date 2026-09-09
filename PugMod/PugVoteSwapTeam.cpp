#include "precompiled.h"

CPugVoteSwapTeam gPugVoteSwapTeam;

void CPugVoteSwapTeam::ServerActivate()
{
    this->m_Run = false;

    this->m_NextFrame = 0.0f;

    this->m_Time = 0;

    this->m_Team = UNASSIGNED;

    this->m_VotesLeft = 0;

    this->m_VoteList.clear();
}

void CPugVoteSwapTeam::CPugVoteSwapTeam::ServerDeactivate()
{
    this->m_Run = false;

    this->m_NextFrame = 0.0f;

    this->m_Time = 0;

    this->m_Team = UNASSIGNED;

    this->m_VotesLeft = 0;

    this->m_VoteList.clear();
}

void CPugVoteSwapTeam::Init(int Team)
{
    if (!this->m_Run)
    {
        auto Players = gPugUtil.GetPlayers();

        if (Players[Team].size() > 0)
        {
            this->m_Run = true;

            this->m_NextFrame = 0.0f;

            this->m_Time = time(nullptr) + static_cast<time_t>(gPugCvar.m_VoteDelay->value);

            this->m_Team = Team;

            this->m_VotesLeft = 0;

            this->m_VoteList.clear();
            
            this->m_VoteList.push_back({1, 0, _T("Terrorists")});
            this->m_VoteList.push_back({2, 0, _T("Counter-Terrorists")});
            
            this->m_VotesLeft = Players[Team].size();

            for (auto const &Player : Players[Team])
            {
                gPugMenu[Player->entindex()].Create(false, E_MENU::ME_VOTE_SWAP_TEAM, _T("Choose the Team:"));

                for (size_t i = 0; i < this->m_VoteList.size(); ++i)
                {
                    this->m_VoteList[i].Votes = 0;

                    gPugMenu[Player->entindex()].AddItem(i, false, 0, this->m_VoteList[i].Name.c_str());
                }

                gPugMenu[Player->entindex()].Show(Player);

                if (gPugCvar.m_SoundEnable->value != 0.0f)
                {
                    gPugUtil.ClientCommand(Player->edict(), g_VoteSwapTeam_Sound[RANDOM_LONG(0, 1)]);
                }
            }

            gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 The ^3%s^1 won: Starting team pick."), gPugCvar.m_Tag->string, g_Pug_TeamName[Team]);
        }
        else
        {
            gPugTask.Create(E_TASK::SET_STATE, 2.0f, false, STATE_FIRST_HALF);
        }
    }
}

void CPugVoteSwapTeam::Stop()
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
            gPugUtil.ClientCommand(nullptr, g_VoteSwapTeam_Sound[2]);
        }

        if (Winner.Votes)
        {
            if (Winner.Team != this->m_Team)
            {
                if (g_pGameRules)
                {
                    CSGameRules()->SwapAllPlayers();
                }
            }

            gPugUtil.PrintColor(nullptr, (Winner.Team == TERRORIST ? E_PRINT_TEAM::RED : E_PRINT_TEAM::BLUE), _T("^4[%s]^1 Swapping teams to: ^3%s^1."), gPugCvar.m_Tag->string, Winner.Name.c_str());
        }
        else
        {
            gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Selection failed: ^3No votes."), gPugCvar.m_Tag->string);
        }

        gPugTask.Create(E_TASK::SET_STATE, 2.0f, false, STATE_FIRST_HALF);
    }
}

P_VOTE_SWAP_INFO CPugVoteSwapTeam::GetWinner()
{
    P_VOTE_SWAP_INFO Winner = {};

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

void CPugVoteSwapTeam::MenuHandle(CBasePlayer *Player, P_MENU_ITEM Item)
{
    if (this->m_Run)
    {
        if (Player)
        {
            this->m_VotesLeft -= 1;

            this->m_VoteList[Item.Info].Votes += 1;

            gPugUtil.PrintColor(nullptr, (Item.Info == TERRORIST) ? E_PRINT_TEAM::RED : E_PRINT_TEAM::BLUE, _T("^4[%s]^1 ^3%s^1 chose ^3%s^1."), gPugCvar.m_Tag->string, STRING(Player->edict()->v.netname), this->m_VoteList[Item.Info].Name.c_str());

            if (this->m_VotesLeft < 1)
            {
                this->Stop();
            }
        }
    }
}

void CPugVoteSwapTeam::StartFrame()
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
                        
                        gPugUtil.SendHud(nullptr, g_VoteSwapTeam_HudParam[0], _T("Team Choice: %s"), szTime);
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
                        gPugUtil.SendHud(nullptr, g_VoteSwapTeam_HudParam[1], VoteList.c_str());
                    }
                    else
                    {
                        gPugUtil.SendHud(nullptr, g_VoteSwapTeam_HudParam[1], _T("No votes registered."));
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
