#include "precompiled.h"

CPugVoteOvertime gPugVoteOvertime;

void CPugVoteOvertime::ServerActivate()
{
    this->m_Run = false;

    this->m_NextFrame = 0.0f;

    this->m_Time = 0;

    this->m_VotesLeft = 0;

    this->m_VoteList.clear();
}

void CPugVoteOvertime::CPugVoteOvertime::ServerDeactivate()
{
    this->m_Run = false;

    this->m_NextFrame = 0.0f;

    this->m_Time = 0;

    this->m_VotesLeft = 0;

    this->m_VoteList.clear();
}

void CPugVoteOvertime::Init()
{
    if (!this->m_Run)
    {
        auto Players = gPugUtil.GetPlayers(true, false);

        if (Players.size() > 0)
        {
            this->m_Run = true;

            if (g_pGameRules)
            {
                if (CSGameRules()->m_bRoundTerminating)
                {
                    CSGameRules()->m_flRestartRoundTime = (gpGlobals->time + gPugCvar.m_VoteDelay->value);
                }
            }

            this->m_NextFrame = 0.0f;

            this->m_Time = time(nullptr) + static_cast<time_t>(gPugCvar.m_VoteDelay->value);

            this->m_VotesLeft = 0;

            this->m_VoteList.clear();
            
            this->m_VoteList.push_back({1, 0, _T("Sudden Death")});
            this->m_VoteList.push_back({2, 0, _T("Play Overtime")});
            this->m_VoteList.push_back({3, 0, _T("End Tied")});
            
            this->m_VotesLeft = Players.size();

            for (auto const &Player : Players)
            {
                gPugMenu[Player->entindex()].Create(false, E_MENU::ME_VOTE_SWAP_TEAM, _T("Match tied, what do you want to do?"));

                for (size_t i = 0; i < this->m_VoteList.size(); ++i)
                {
                    this->m_VoteList[i].Votes = 0;

                    gPugMenu[Player->entindex()].AddItem(i, false, i, this->m_VoteList[i].Name.c_str());
                }

                gPugMenu[Player->entindex()].Show(Player);

                if (gPugCvar.m_SoundEnable->value != 0.0f)
                {
                    gPugUtil.ClientCommand(Player->edict(), g_VoteOverimte_Sound[RANDOM_LONG(0, 1)]);
                }
            }

            gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 The match is tied, choose what to do:"), gPugCvar.m_Tag->string);
            gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 1. Sudden Death: ^3The winning team will be the match winner."), gPugCvar.m_Tag->string);
            gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 2. Play Overtime: ^3The winner will be decided via Overtime."), gPugCvar.m_Tag->string);
            gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 3. End Tied: ^3The match ends without a winner."), gPugCvar.m_Tag->string);
        }
        else
        {
            gPugTask.Create(E_TASK::SET_STATE, 2.0f, false, STATE_END);
        }
    }
}

void CPugVoteOvertime::Stop()
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
            gPugUtil.ClientCommand(nullptr, g_VoteOverimte_Sound[2]);
        }

        if (Winner.Votes)
        {
            switch (Winner.Index)
            {
                case 1: // Sudden Death Round
                {
                    if (g_pGameRules)
                    {
                        CSGameRules()->m_bGameStarted = true;
                    }
                    
                    gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Continuing match in ^3Sudden Death^1 mode after freezetime."), gPugCvar.m_Tag->string);

                    break;
                }
                case 2: // Play Overtime
                {
                    gPugTask.Create(E_TASK::SET_STATE, 1.0f, false, STATE_OVERTIME);
                    break;
                }
                case 3: // End Tied
                {
                    gPugTask.Create(E_TASK::SET_STATE, 1.0f, false, STATE_END);
                    break;
                }
            }
        }
        else
        {
            gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Selection failed: ^3No votes."), gPugCvar.m_Tag->string);
            
            gPugTask.Create(E_TASK::SET_STATE, 1.0f, false, STATE_END);
        }
    }
}

P_VOTE_OT_INFO CPugVoteOvertime::GetWinner()
{
    P_VOTE_OT_INFO Winner = {};

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

void CPugVoteOvertime::MenuHandle(CBasePlayer *Player, P_MENU_ITEM Item)
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

void CPugVoteOvertime::StartFrame()
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
                        
                        gPugUtil.SendHud(nullptr, g_VoteSwapTeam_HudParam[0], _T("Overtime Mode: %s"), szTime);
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
