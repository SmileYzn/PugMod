#include "precompiled.h"

CPugVoteEnd gPugVoteEnd;

void CPugVoteEnd::ServerActivate()
{
    this->m_Run = false;

    this->m_NextFrame = 0.0f;

    this->m_Time = 0;

    this->m_VotesLeft = 0;

    this->m_VoteList.clear();
}

void CPugVoteEnd::ServerDeactivate()
{
    this->m_Run = false;

    this->m_NextFrame = 0.0f;

    this->m_Time = 0;

    this->m_VotesLeft = 0;

    this->m_VoteList.clear();
}

void CPugVoteEnd::Init()
{
    if (!this->m_Run)
    {        
        auto Players = gPugUtil.GetPlayers(true, false);

        if (Players.size() > 0)
        {
            this->m_Run = true;

            this->m_NextFrame = 0.0f;

            this->m_Time = time(nullptr) + static_cast<time_t>(gPugCvar.m_VoteDelay->value);

            this->m_VotesLeft = 0;

            this->m_VoteList.clear();
            
            this->m_VoteList.push_back({1, 0, _T("Continue Match")});
            this->m_VoteList.push_back({2, 0, _T("Cancel Match")});

            if (gPugMod.GetState() == STATE_FIRST_HALF)
            {
                this->m_VoteList.push_back({3, 0, _T("Restart Match")});
            }

            this->m_VotesLeft = Players.size();

            for (auto const &Player : Players)
            {
                gPugMenu[Player->entindex()].Create(false, E_MENU::ME_VOTE_END, _T("A player left the match, what do you want to do?"));

                for (size_t i = 0; i < this->m_VoteList.size(); ++i)
                {
                    this->m_VoteList[i].Votes = 0;

                    gPugMenu[Player->entindex()].AddItem(i, false, i, this->m_VoteList[i].Name.c_str());
                }

                gPugMenu[Player->entindex()].Show(Player);

                if (gPugCvar.m_SoundEnable->value != 0.0f)
                {
                    gPugUtil.ClientCommand(Player->edict(), g_VoteEnd_Sound[RANDOM_LONG(0, 1)]);
                }

                gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 A player left the match, what do you want to do?"), gPugCvar.m_Tag->string);
            }
        }
    }
}

void CPugVoteEnd::Stop()
{
    if (this->m_Run)
    {
        this->m_Run = false;

        this->m_NextFrame = 0.0f;

        this->m_Time = 0;

        this->m_VotesLeft = 0;

        auto Winner = this->GetWinner();

        if (Winner.Votes)
        {
            if (Winner.Index == 2)
            {
                if (gPugCvar.m_SoundEnable->value != 0.0f)
                {
                    gPugUtil.ClientCommand(nullptr, g_VoteEnd_Sound[2]);
                }

                gPugTask.Create(E_TASK::SET_STATE, 1.0f, false, STATE_DEATHMATCH);
            }
            else if (Winner.Index == 3)
            {
                if (gPugCvar.m_SoundEnable->value != 0.0f)
                {
                    gPugUtil.ClientCommand(nullptr, g_VoteEnd_Sound[2]);
                }

                gPugTask.Create(E_TASK::SET_STATE, 1.0f, false, STATE_FIRST_HALF);
            }
        }
    }
}

P_VOTE_END_INFO CPugVoteEnd::GetWinner()
{
    P_VOTE_END_INFO Winner = {};

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

void CPugVoteEnd::MenuHandle(CBasePlayer *Player, P_MENU_ITEM Item)
{
    if (this->m_Run)
    {
        if (Player)
        {
            this->m_VotesLeft -= 1;

            this->m_VoteList[Item.Info].Votes += 1;

            gPugUtil.PrintColor(nullptr, (Item.Info == TERRORIST) ? E_PRINT_TEAM::RED : E_PRINT_TEAM::BLUE, "^4[%s]^1 ^3%s^1 escolheu ^3%s^1.", gPugCvar.m_Tag->string, STRING(Player->edict()->v.netname), this->m_VoteList[Item.Info].Name.c_str());

            if (this->m_VotesLeft < 1)
            {
                this->Stop();
            }
        }
    }
}

void CPugVoteEnd::StartFrame()
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
                        
                        gPugUtil.SendHud(nullptr, g_VoteEnd_HudParam[0], _T("Finalizar Partida: %s"), szTime);
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
                        gPugUtil.SendHud(nullptr, g_VoteEnd_HudParam[1], VoteList.c_str());
                    }
                    else
                    {
                        gPugUtil.SendHud(nullptr, g_VoteEnd_HudParam[1], _T("No Votes."));
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
