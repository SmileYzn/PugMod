#include "precompiled.h"

CPugMod gPugMod;

void CPugMod::ServerActivate()
{
    this->m_State = STATE_DEAD;

    this->m_Score = {};
    this->m_ScoreOT = {};
    this->m_Point = {};

    this->SetState(STATE_DEAD);

    gPugUtil.ServerCommand("exec %s/cfg/maplist.cfg", gPugUtil.GetPath());

    gPugEngine.RegisterHook("TeamScore", this->TeamScore);

    gPugEngine.RegisterHook("ScoreInfo", this->ScoreInfo);

    gPugTask.Create(E_TASK::SET_STATE, 5.0f, false, STATE_DEATHMATCH); 
}

void CPugMod::ServerDeactivate()
{
    if (gPugCvar.m_LastMap)
    {
        g_engfuncs.pfnCvar_DirectSet(gPugCvar.m_LastMap, STRING(gpGlobals->mapname));
    }

    if (this->m_State >= STATE_FIRST_HALF && this->m_State <= STATE_OVERTIME)
    {
        this->SetState(STATE_END);
    }
}

int CPugMod::GetState()
{
    return this->m_State;
}

int CPugMod::SetState(int State)
{
    this->m_State = State;

    switch(State)
    {
        case STATE_DEAD:
        {
            this->m_Score = {};
            this->m_ScoreOT = {};
            this->m_Point = {};

            gPugDM.Stop();
            gPugReady.Stop(true);
            gPugTimer.Stop(true);
            break;
        }
        case STATE_DEATHMATCH:
        {
            this->m_Score = {};
            this->m_ScoreOT = {};
            this->m_Point = {};

            gPugDM.Stop();
            gPugReady.Stop(true);
            gPugTimer.Stop(true);

            if (gPugCvar.m_DM_Enable->value)
            {
                gPugDM.Init();
            }

            if (gPugCvar.m_ReadyType->value == 1.0f)
            {
                gPugTimer.Init(gPugCvar.m_VoteMap->value ? STATE_VOTEMAP : STATE_VOTETEAM);
            }
            else if (gPugCvar.m_ReadyType->value == 2.0f)
            {
                gPugReady.Init(gPugCvar.m_VoteMap->value ? STATE_VOTEMAP : STATE_VOTETEAM);
            }

            g_engfuncs.pfnCvar_DirectSet(gPugCvar.m_PlayGameMode, "0");
            g_engfuncs.pfnCvar_DirectSet(gPugCvar.m_PlayKnifeRound, "0");
            
            break;
        }
        case STATE_VOTEMAP:
        {
            this->m_Score = {};
            this->m_ScoreOT = {};
            this->m_Point = {};

            gPugDM.Stop();
            gPugReady.Stop(true);
            gPugTimer.Stop(true);

            gPugVoteMap.Init();

            g_engfuncs.pfnCvar_DirectSet(gPugCvar.m_VoteMap, "0");

            g_engfuncs.pfnCvar_DirectSet(gPugCvar.m_PlayGameMode, "0");
            g_engfuncs.pfnCvar_DirectSet(gPugCvar.m_PlayKnifeRound, "0");
            break;
        }
        case STATE_VOTETEAM:
        {
            this->m_Score = {};
            this->m_ScoreOT = {};
            this->m_Point = {};

            gPugDM.Stop();
            gPugReady.Stop(true);
            gPugTimer.Stop(true);

            gPugVoteTeam.Init();

            g_engfuncs.pfnCvar_DirectSet(gPugCvar.m_VoteMap, "1");

            g_engfuncs.pfnCvar_DirectSet(gPugCvar.m_PlayGameMode, "0");
            g_engfuncs.pfnCvar_DirectSet(gPugCvar.m_PlayKnifeRound, "0");
            break;
        }
        case STATE_CAPTAIN:
        {
            this->m_Score = {};
            this->m_ScoreOT = {};
            this->m_Point = {};

            gPugDM.Stop();
            gPugReady.Stop(true);
            gPugTimer.Stop(true);

            gPugLeader.Init();

            if (g_pGameRules)
            {
                CSGameRules()->BalanceTeams();
            }

            g_engfuncs.pfnCvar_DirectSet(gPugCvar.m_PlayGameMode, "1");
            g_engfuncs.pfnCvar_DirectSet(gPugCvar.m_PlayKnifeRound, "0");
            break;
        }
        case STATE_KNIFE_ROUND:
        {
            this->m_Score = {};
            this->m_ScoreOT = {};
            this->m_Point = {};
            
            gPugDM.Stop();
            gPugReady.Stop(true);
            gPugTimer.Stop(true);

            if (g_pGameRules)
            {
                CSGameRules()->BalanceTeams();
            }

            g_engfuncs.pfnCvar_DirectSet(gPugCvar.m_PlayKnifeRound, "1");

            gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Knife Round: The ^3winner chooses the starting team."), gPugCvar.m_Tag->string);

            gPugLO3.Init(4);
            break;
        }
        case STATE_FIRST_HALF:
        {
            this->m_Score = {};
            this->m_ScoreOT = {};
            this->m_Point = {};

            gPugDM.Stop();
            gPugReady.Stop(true);
            gPugTimer.Stop(true);
            
            if (g_pGameRules)
            {
                CSGameRules()->BalanceTeams();
            }
            
            gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 ^3%s^1 started: Get ready!!"), gPugCvar.m_Tag->string, gPugMod.GetString(State));

            gPugLO3.Init(4);
            break;
        }
        case STATE_HALFTIME:
        {
            this->SwapTeams();

            auto Players = gPugUtil.GetPlayers(true, true);

            auto NextState = (this->GetRound() < static_cast<int>(gPugCvar.m_Rounds->value)) ? STATE_SECOND_HALF : STATE_OVERTIME;

            if (Players.size() < static_cast<size_t>(gPugCvar.m_PlayersMin->value * 2.0f))
            {
                if (gPugCvar.m_ReadyType->value == 1.0f)
                {
                    gPugTimer.Init(NextState);
                }
                else if (gPugCvar.m_ReadyType->value == 2.0f)
                {
                    gPugReady.Init(NextState);
                }
            }
            else
            {
                gPugTask.Create(E_TASK::SET_STATE, 3.0f, false, NextState);
            }
            
            gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 ^3%s^1 started: Get ready!!"), gPugCvar.m_Tag->string, gPugMod.GetString(State));
            break;
        }
        case STATE_SECOND_HALF:
        {
            this->SetScore(TERRORIST, State, 0);
            this->SetScore(CT, State, 0);
            this->ResetPoint(State);

            gPugDM.Stop();
            gPugReady.Stop(true);
            gPugTimer.Stop(true);

            if (g_pGameRules)
            {
                CSGameRules()->BalanceTeams();
            }

            gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 ^3%s^1 started: Get ready!!"), gPugCvar.m_Tag->string, gPugMod.GetString(State));

            gPugLO3.Init(4);
            break;
        }
        case STATE_OVERTIME:
        {
            gPugDM.Stop();
            gPugReady.Stop(true);
            gPugTimer.Stop(true);

            gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 ^3%s^1 started: Get ready!!"), gPugCvar.m_Tag->string, gPugMod.GetString(State));

            gPugLO3.Init(4);
            break;
        }
        case STATE_END:
        {
            this->Scores(nullptr);

            g_engfuncs.pfnCvar_DirectSet(gPugCvar.m_VoteMap, "1");

            g_engfuncs.pfnCvar_DirectSet(gPugCvar.m_LastMap, STRING(gpGlobals->mapname));

            auto Players = gPugUtil.GetPlayers(true, true);

            if (Players.size() >= static_cast<size_t>(gPugCvar.m_PlayersMin->value))
            {
                gPugTask.Create(E_TASK::SET_STATE, (gPugCvar.m_RoundRestartDelay->value + 1.0f), false, STATE_VOTEMAP);
            }
            else
            {
                gPugTask.Create(E_TASK::SET_STATE, (gPugCvar.m_RoundRestartDelay->value + 1.0f), false, STATE_DEATHMATCH);
            }

            break;
        }
    }

    gPugGameDesc.Update();

    gPugStats.SetState();

    g_engfuncs.pfnCvar_DirectSet(gPugCvar.m_State, std::to_string(State).c_str());

    gPugUtil.ServerCommand("exec %s/cfg/%s.cfg", gPugUtil.GetPath(), g_Pug_Config[State]);

    return this->m_State;
}

const char *CPugMod::GetString(int State)
{
    if (State >= STATE_DEAD && State <= STATE_END)
    {
        return _T(g_Pug_String[State]);
    }

    return g_Pug_String[STATE_DEAD];
}

void CPugMod::SwapTeams()
{
	if (this->GetRound() >= static_cast<int>(gPugCvar.m_Rounds->value))
	{
		if (this->GetScore(TERRORIST) == this->GetScore(CT))
		{
			return;
		}
	}

    if (g_pGameRules)
    {
        CSGameRules()->SwapAllPlayers();

        CSGameRules()->m_bCompleteReset = true;

        CSGameRules()->RestartRound();
    }

    SWAP(this->m_Score[TERRORIST], this->m_Score[CT]);

    SWAP(this->m_ScoreOT[TERRORIST], this->m_ScoreOT[CT]);

    gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Automatically swapping teams."), gPugCvar.m_Tag->string);
}

void CPugMod::Surrender(TeamName Team)
{
    if (this->m_State >= STATE_FIRST_HALF && this->m_State <= STATE_OVERTIME)
    {
        auto HalfRounds = (int)(gPugCvar.m_Rounds->value / 2.0f);

        this->m_Score = {};
        this->m_ScoreOT = {};

        auto Winner = (Team == TERRORIST ? CT : TERRORIST);
        
		this->m_Score[Winner][STATE_FIRST_HALF]  = HalfRounds;
		this->m_Score[Winner][STATE_SECOND_HALF] = HalfRounds;

        gPugUtil.SendHud(nullptr, g_Pug_HudParam, _T("Game Over!^nThe ^3%s^1 team surrendered!"), g_Pug_TeamName[Team]);

        gPugUtil.PrintColor(nullptr, (Winner == TERRORIST) ? E_PRINT_TEAM::RED : E_PRINT_TEAM::BLUE, _T("^4[%s]^1 Game Over! The ^3%s^1 team surrendered!"), gPugCvar.m_Tag->string, g_Pug_TeamName[Team]);

        gPugTask.Create(E_TASK::SET_STATE, 1.0f, false, STATE_END);
    }
}

int CPugMod::GetRound()
{
    return (this->GetScore(TERRORIST) + this->GetScore(CT));
}

int CPugMod::GetScore(TeamName Team)
{
    return (this->m_Score[Team][STATE_FIRST_HALF] + this->m_Score[Team][STATE_SECOND_HALF] + this->m_Score[Team][STATE_OVERTIME]);
}

void CPugMod::SetScore(TeamName Team, int State, int Score)
{
    if (State >= STATE_DEAD && State <= STATE_END)
    {
        this->m_Score[Team][State] = Score;
    }
}

int CPugMod::GetPoint(int EntityIndex, int Type)
{
    if (EntityIndex >= 1 && EntityIndex <= gpGlobals->maxClients)
    {
        if (Type == 0 || Type == 1)
        {
            return (this->m_Point[EntityIndex][STATE_FIRST_HALF][Type] + this->m_Point[EntityIndex][STATE_SECOND_HALF][Type] + this->m_Point[EntityIndex][STATE_OVERTIME][Type]);
        }
    }

    return 0;
}

void CPugMod::SetPoint(int EntityIndex, int State, int Type, int Point)
{
    if (State >= STATE_DEAD && State <= STATE_END)
    {
        if (EntityIndex >= 1 && EntityIndex <= gpGlobals->maxClients)
        {
            if (Type == 0 || Type == 1)
            {
                this->m_Point[EntityIndex][State][Type] = Point;
            }
        }
    }
}

void CPugMod::ResetPoint(int State)
{
    if (State >= STATE_DEAD && State <= STATE_END)
    {
        for (int i = 1; i <= gpGlobals->maxClients; ++i)
        {
            this->m_Point[i][State].fill(0);
        }
    }
}

void CPugMod::GetIntoGame(CBasePlayer *Player)
{
    if (this->m_State != STATE_DEAD)
    {
        if (!Player->IsBot())
        {
            if (Player->m_iTeam == TERRORIST || Player->m_iTeam == CT)
            {
                if (Player->m_iLastAccount < 1)
                {
                    gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 %s Build %s (^3%s^1)"), gPugCvar.m_Tag->string, Plugin_info.name, Plugin_info.date, Plugin_info.author);
                    gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Type ^3%shelp^1 for command list."), gPugCvar.m_Tag->string, gPugCvar.m_CmdPrefixPlayer->string);
                }
            }
        }
    }
}

void CPugMod::DropClient(edict_t *pEntity)
{
    if (this->m_State >= STATE_KNIFE_ROUND && this->m_State <= STATE_OVERTIME)
    {
        auto Players = gPugUtil.GetPlayers();

        if((Players[TERRORIST].size() < 1) && (Players[CT].size() < 1))
        {
            this->SetState(STATE_END);
        }
        else
        {
            auto Player = UTIL_PlayerByIndexSafe(ENTINDEX(pEntity));

            if (Player)
            {
                if (Player->m_iTeam == TERRORIST || Player->m_iTeam == CT)
                {
                    gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 ^3%s^1 left the match."), gPugCvar.m_Tag->string, STRING(pEntity->v.netname));

                    auto PlayersMin = static_cast<size_t>(gPugCvar.m_PlayersMin->value - 1.0f);

                    if((Players[TERRORIST].size() < PlayersMin) || (Players[CT].size() < PlayersMin))
                    {
                        gPugVoteEnd.Init();
                    }
                }
            }
        }
    }
}

void CPugMod::RestartRound()
{
    if (g_pGameRules)
    {
        if (!CSGameRules()->m_bCompleteReset)
        {
            if (this->m_State >= STATE_FIRST_HALF && this->m_State <= STATE_END)
            {
                if (CSGameRules()->m_iTotalRoundsPlayed > 0)
                {
                    gPugTask.Create(E_TASK::ROUND_MESSAGE, 1.0f, false, 0);
                }
            }

            gPugGameDesc.Update();
        }
        else
        {
            if (this->m_State == STATE_FIRST_HALF || this->m_State == STATE_SECOND_HALF)
            {
                this->ResetPoint(this->m_State);

                this->SetScore(TERRORIST, this->m_State, 0);

                this->SetScore(CT, this->m_State, 0);
            }
        }
    }
}

void CPugMod::RoundStart()
{
    if (this->m_State == STATE_FIRST_HALF || this->m_State == STATE_SECOND_HALF || this->m_State == STATE_OVERTIME)
    {
        if (g_pGameRules)
        {
            if (this->GetScore(TERRORIST) || this->GetScore(CT))
            {
                this->Scores(nullptr);
            }

            gPugUtil.ClientPrint(nullptr, E_PRINT::CONSOLE, _T("[%s] Round %d Started."), gPugCvar.m_Tag->string, this->GetRound() + 1);
        }
    }
}

void CPugMod::RoundEnd(int winStatus, ScenarioEventEndRound event, float tmDelay)
{
    if (this->m_State == STATE_KNIFE_ROUND || this->m_State == STATE_FIRST_HALF || this->m_State == STATE_SECOND_HALF || this->m_State == STATE_OVERTIME)
    {
        if (winStatus == WINSTATUS_CTS || winStatus == WINSTATUS_TERRORISTS)
        {
            if (g_pGameRules)
            {
                if (!CSGameRules()->m_bCompleteReset && CSGameRules()->m_bGameStarted)
                {
                    TeamName Winner = (winStatus == WINSTATUS_TERRORISTS) ? TERRORIST : CT;

                    this->m_Score[Winner][this->m_State]++;

                    if (this->m_State == STATE_OVERTIME)
                    {
                        this->m_ScoreOT[Winner]++;
                    }

                    gPugGameDesc.Update();
                    
                    gPugUtil.ClientPrint(nullptr, E_PRINT::CONSOLE, _T("[%s] Round %d Won By: %s"), gPugCvar.m_Tag->string, (this->GetRound() + 1), g_Pug_TeamName[Winner]);

                    switch (this->m_State)
                    {
                        case STATE_KNIFE_ROUND:
                        {
                            gPugTask.Create(E_TASK::VOTE_SWAP_TEAM, 1.0f, false, Winner);
                            break;
                        }
                        case STATE_FIRST_HALF:
                        {
                            if ((this->m_Score[TERRORIST][this->m_State] + this->m_Score[CT][this->m_State]) == static_cast<int>(gPugCvar.m_Rounds->value / 2.0f))
                            {
                                gPugTask.Create(E_TASK::SET_STATE, (tmDelay + 1.0f), false, STATE_HALFTIME);
                            }
                            break;
                        }
                        case STATE_SECOND_HALF:
                        {
                            auto MaxRounds = static_cast<int>(gPugCvar.m_Rounds->value / 2.0f);

                            auto ScoreTR = (this->m_Score[TERRORIST][STATE_FIRST_HALF] + this->m_Score[TERRORIST][STATE_SECOND_HALF]);
                            auto ScoreCT = (this->m_Score[CT][STATE_FIRST_HALF] + this->m_Score[CT][STATE_SECOND_HALF]);

                            if ((ScoreTR > MaxRounds) || (ScoreCT > MaxRounds))
                            {
                                gPugTask.Create(E_TASK::SET_STATE, (tmDelay + 1.0f), false, STATE_END);
                            }
                            else if ((ScoreTR == MaxRounds) && (ScoreCT == MaxRounds))
                            {
                                switch (static_cast<int>(gPugCvar.m_OvertimeType->value))
                                {
                                    case 0: // Vote OT
                                    {
                                        gPugTask.Create(E_TASK::VOTE_OVERTIME, (tmDelay + 1.0f), false, 0);
                                        break;
                                    }
                                    case 1: // OT
                                    {
                                        gPugTask.Create(E_TASK::SET_STATE, (tmDelay + 1.0f), false, STATE_HALFTIME);
                                        break;
                                    }
                                    case 2: // End Tied
                                    {
                                        gPugTask.Create(E_TASK::SET_STATE, (tmDelay + 1.0f), false, STATE_END);
                                        break;
                                    }
                                }
                            }
                            break;
                        }
                        case STATE_OVERTIME:
                        {
                            auto MaxRounds = static_cast<int>(gPugCvar.m_RoundsOT->value / 2.0f);

                            if ((this->m_ScoreOT[TERRORIST] + this->m_ScoreOT[CT]) == MaxRounds)
                            {
                                gPugTask.Create(E_TASK::SET_STATE, (tmDelay + 1.0f), false, STATE_HALFTIME);
                            }
                            else if ((this->m_ScoreOT[TERRORIST] > MaxRounds) || (this->m_ScoreOT[CT] > MaxRounds))
                            {
                                gPugTask.Create(E_TASK::SET_STATE, (tmDelay + 1.0f), false, STATE_END);
                            }
                            else if ((this->m_ScoreOT[TERRORIST] == MaxRounds) && (this->m_ScoreOT[CT] == MaxRounds))
                            {
                                switch (static_cast<int>(gPugCvar.m_OvertimeType->value))
                                {
                                    case 0: // Vote OT
                                    {
                                        gPugTask.Create(E_TASK::VOTE_OVERTIME, (tmDelay + 1.0f), false, 0);
                                        break;
                                    }
                                    case 1: // OT
                                    {
                                        gPugTask.Create(E_TASK::SET_STATE, (tmDelay + 1.0f), false, STATE_HALFTIME);
                                        break;
                                    }
                                    case 2: // End Tied
                                    {
                                        gPugTask.Create(E_TASK::SET_STATE, (tmDelay + 1.0f), false, STATE_END);
                                        break;
                                    }
                                }
                            }
                            break;
                        }
                    }   
                }
            }
        }
    }
}

void CPugMod::RoundMessage()
{
    if (this->m_State >= STATE_FIRST_HALF && this->m_State <= STATE_END)
    {
        if (g_pGameRules)
        {
            if (CSGameRules()->m_iTotalRoundsPlayed > 0)
            {
                auto ScoreTR = this->GetScore(TERRORIST);
                auto ScoreCT = this->GetScore(CT);

                if (this->m_State == STATE_FIRST_HALF || this->m_State == STATE_SECOND_HALF || this->m_State == STATE_OVERTIME)
                {
                    auto MaxRound = static_cast<int>(gPugCvar.m_Rounds->value / 2.0f);
                    auto MaxRoundOT = static_cast<int>(gPugCvar.m_RoundsOT->value / 2.0f);

                    if ((ScoreTR + ScoreCT) == (MaxRound - 1) || (this->m_ScoreOT[TERRORIST] + this->m_ScoreOT[CT]) == (MaxRoundOT - 1))
                    {
                        if (gPugCvar.m_SoundEnable->value != 0.0f)
                        {
                            gPugUtil.ClientCommand(nullptr, "spk \"fvox/blip, warning\"");
                        }

                        gPugUtil.SendHud(nullptr, g_Pug_HudParam, _T("%s^n%s %d : %d %s^nLast Round"), gPugMod.GetString(this->m_State), g_Pug_TeamNameShort[TERRORIST], ScoreTR, ScoreCT, g_Pug_TeamNameShort[CT]);
                    }
                    else if ((ScoreTR == MaxRound) || (ScoreCT == MaxRound) || (this->m_ScoreOT[TERRORIST] == MaxRoundOT) || (this->m_ScoreOT[CT] == MaxRoundOT))
                    {
                        if (gPugCvar.m_SoundEnable->value != 0.0f)
                        {
                            gPugUtil.ClientCommand(nullptr, "spk \"fvox/blip, warning\"");
                        }

                        gPugUtil.SendHud(nullptr, g_Pug_HudParam, _T("%s^n%s %d : %d %s^nMatch End Danger"), gPugMod.GetString(this->m_State), g_Pug_TeamNameShort[TERRORIST], ScoreTR, ScoreCT, g_Pug_TeamNameShort[CT]);
                    }
                    else
                    {
                        gPugUtil.SendHud(nullptr, g_Pug_HudParam, _T("%s\n%s %d : %d %s"), gPugMod.GetString(this->m_State), g_Pug_TeamNameShort[TERRORIST], ScoreTR, ScoreCT, g_Pug_TeamNameShort[CT]);
                    }
                }
                else if (this->m_State == STATE_END)
                {
                    if (gPugCvar.m_SoundEnable->value != 0.0f)
                    {
                        gPugUtil.ClientCommand(nullptr, "spk \"misc/sheep\"");
                    }

                    if (ScoreTR == ScoreCT)
                    {
                        gPugUtil.SendHud(nullptr, g_Pug_HudParam, _T("End of Game!^n%s %d : %d %s^nThe match ended tied"), g_Pug_TeamNameShort[TERRORIST], ScoreTR, ScoreCT, g_Pug_TeamNameShort[CT]);
                    }
                    else
                    {
                        auto Winner = (ScoreTR > ScoreCT) ? TERRORIST : CT;

                        gPugUtil.SendHud(nullptr, g_Pug_HudParam, _T("End of Game!^n%s %d : %d %s^nThe %s won the match"), g_Pug_TeamNameShort[TERRORIST], ScoreTR, ScoreCT, g_Pug_TeamNameShort[CT], g_Pug_TeamName[Winner]);
                    }
                }
            }
        }
    }
}

void CPugMod::Status(CBasePlayer *Player)
{
    auto Players = gPugUtil.GetPlayers();

    gPugUtil.PrintColor(Player->edict(), Player->entindex(), _T("^4[%s]^1 Status: ^3%s^1"), gPugCvar.m_Tag->string, gPugMod.GetString(this->m_State));
    gPugUtil.PrintColor(Player->edict(), Player->entindex(), _T("^4[%s]^1 TRs: ^3%d^1, CTs: ^3%d^1, SPECs: ^3%d^1"), gPugCvar.m_Tag->string, Players[TERRORIST].size(), Players[CT].size(), Players[SPECTATOR].size());
}

void CPugMod::Scores(CBasePlayer *Player)
{
    if (g_pGameRules)
    {
        auto pEntity = Player ? Player->edict() : nullptr;

        if (this->m_State >= STATE_FIRST_HALF && this->m_State <= STATE_END)
        {
            auto ScoreTR = this->GetScore(TERRORIST);
            auto ScoreCT = this->GetScore(CT);

            auto Sender = E_PRINT_TEAM::GREY;

            if (ScoreTR != ScoreCT)
            {
                Sender = (ScoreTR > ScoreCT) ? E_PRINT_TEAM::RED : E_PRINT_TEAM::BLUE;
            }

            if (gPugCvar.m_ScoreText->value > 0)
            {
                if (this->m_State != STATE_END)
                {
                    gPugUtil.PrintColor(pEntity, Sender, _T("^4[%s]^1 ^3%s^1 (^4%d^1) - (^4%d^1) ^3%s^1"), gPugCvar.m_Tag->string, g_Pug_TeamName[TERRORIST], ScoreTR, ScoreCT, g_Pug_TeamName[CT]);
                }
                else
                {
                    gPugUtil.PrintColor(pEntity, Sender, _T("^4[%s]^1 End of game: ^3%s^1 (^4%d^1) - (^4%d^1) ^3%s^1"), gPugCvar.m_Tag->string, g_Pug_TeamName[TERRORIST], ScoreTR, ScoreCT, g_Pug_TeamName[CT]);
                }
            }
            else
            {
                if (ScoreTR != ScoreCT)
                {
                    auto Winner = (ScoreTR > ScoreCT) ? TERRORIST : CT;
                    auto Losers = (ScoreTR < ScoreCT) ? TERRORIST : CT;

                    if (this->m_State != STATE_END)
                    {
                        gPugUtil.PrintColor(pEntity, Sender, _T("^4[%s]^1 The ^3%s^1 are winning: %d-%d"), gPugCvar.m_Tag->string, g_Pug_TeamName[Winner], this->GetScore(Winner), this->GetScore(Losers));
                    }
                    else
                    {
                        gPugUtil.PrintColor(pEntity, Sender, _T("^4[%s]^1 End of game! The ^3%s^1 won: %d-%d"), gPugCvar.m_Tag->string, g_Pug_TeamName[Winner], this->GetScore(Winner), this->GetScore(Losers));
                    }
                }
                else
                {
                    if (this->m_State != STATE_END)
                    {
                        gPugUtil.PrintColor(pEntity, Sender, _T("^4[%s]^1 Score tied: %d-%d"), gPugCvar.m_Tag->string, ScoreTR, ScoreCT);
                    }
                    else
                    {
                        gPugUtil.PrintColor(pEntity, Sender, _T("^4[%s]^1 End of game! Score tied: %d-%d"), gPugCvar.m_Tag->string, ScoreTR, ScoreCT);
                    }
                }
            }
        }
        else
        {
            gPugUtil.PrintColor(pEntity, E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Command unavailable."), gPugCvar.m_Tag->string);
        }
    }
}

bool CPugMod::TeamScore(int msg_dest, int msg_type, const float* pOrigin, edict_t* pEntity)
{
	if (gPugMod.GetState() >= STATE_FIRST_HALF)
	{
        if (gPugCvar.m_ScoreTeams->value > 0.0f)
        {
            auto Team = gPugEngine.GetString(0);

            if (Team)
            {
                if (Team[0u] == 'T')
                {
                    gPugEngine.SetArgInt(1, gPugMod.GetScore(TERRORIST));
                }
                else if (Team[0u] == 'C')
                {
                    gPugEngine.SetArgInt(1, gPugMod.GetScore(CT));
                }
            }
        }
	}

	return false;
}

bool CPugMod::ScoreInfo(int msg_dest, int msg_type, const float *pOrigin, edict_t *pEntity)
{
    auto State = gPugMod.GetState();

    if (State >= STATE_FIRST_HALF && State <= STATE_OVERTIME)
    {
        if (gPugCvar.m_ScorePlayers->value)
        {
            auto EntityIndex = gPugEngine.GetByte(0);

            if (EntityIndex > 0 && EntityIndex <= gpGlobals->maxClients)
            {
                gPugMod.SetPoint(EntityIndex, State, 0, gPugEngine.GetShort(1));
                gPugMod.SetPoint(EntityIndex, State, 1, gPugEngine.GetShort(2));

                gPugEngine.SetArgInt(1, gPugMod.GetPoint(EntityIndex, 0));
                gPugEngine.SetArgInt(2, gPugMod.GetPoint(EntityIndex, 1));
            }
        }
    }

    return false;
}
