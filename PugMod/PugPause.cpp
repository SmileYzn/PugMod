#include "precompiled.h"

CPugPause gPugPause;

void CPugPause::ServerActivate()
{
    this->m_State = PUG_PAUSE_NONE;

    this->m_BuyTime = 0.0f;

    this->m_Team = UNASSIGNED;
}

void CPugPause::Init(CBasePlayer* Player)
{
    if (gPugAdmin.CheckAccess(Player, ADMIN_LEVEL_B))
    {
        if (gPugCvar.m_PauseTimeout->value > 0.0f)
        {
            auto State = gPugMod.GetState();

            if (State == STATE_FIRST_HALF || State == STATE_SECOND_HALF || State == STATE_OVERTIME)
            {
                this->m_State = PUG_PAUSE_PAUSE;

                this->m_Team = UNASSIGNED;

                gPugUtil.PrintColor(nullptr, Player->entindex(), _T("^4[%s]^1 ^3%s^1 paused match: Game will pause on next round start."), gPugCvar.m_Tag->string, STRING(Player->edict()->v.netname));
            }
            else
            {
                gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Unable to pause match while game is not running."), gPugCvar.m_Tag->string);
            }
        }
        else
        {
            gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Pause timeout is not set."), gPugCvar.m_Tag->string);
        }
    }
}

void CPugPause::Init(TeamName Team)
{
    if (Team == TERRORIST || Team == CT)
    {
        if (gPugCvar.m_PauseTimeout->value > 0.0f)
        {
            auto State = gPugMod.GetState();

            if (State == STATE_FIRST_HALF || State == STATE_SECOND_HALF || State == STATE_OVERTIME)
            {
                this->m_State = PUG_PAUSE_PAUSE;

                this->m_Team = Team;

                gPugUtil.PrintColor(nullptr, (Team == TERRORIST ? E_PRINT_TEAM::RED : E_PRINT_TEAM::BLUE), _T("^4[%s]^1 The ^3%s^1 team paused match: Game will pause on next round start."), gPugCvar.m_Tag->string, g_Pug_TeamName[Team]);
            }
        }
    }
}

void CPugPause::RestartRound()
{
    auto State = gPugMod.GetState();

    if (State == STATE_FIRST_HALF || State == STATE_SECOND_HALF || State == STATE_OVERTIME)
    {
        if (this->m_State == PUG_PAUSE_PAUSE)
        {
            this->m_State = PUG_PAUSE_PAUSED;

            this->m_BuyTime = gPugCvar.m_MpBuyTime->value;

            auto PauseTime = static_cast<int>(gPugCvar.m_PauseTimeout->value);

            g_engfuncs.pfnCvar_DirectSet(gPugCvar.m_MpBuyTime, std::to_string(PauseTime).c_str());

            this->SetRoundTime(PauseTime + 1, true);

            gPugTask.Create(E_TASK::PAUSE_MATCH, 0.5f, true, PauseTime);

            auto TimeUnit = CTimeFormat::GetTimeLength(PauseTime, CTimeFormat::TIMEUNIT_SECONDS);

            if (TimeUnit.length() > 0)
            {
                if (this->m_Team == UNASSIGNED)
                {
                    gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Match paused: Match will continue in ^3%s^1."), gPugCvar.m_Tag->string, TimeUnit.c_str());
                }
                else
                {
                    gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Match paused by ^3%s^1: Match will continue in ^3%s^1."), gPugCvar.m_Tag->string, g_Pug_TeamName[this->m_Team], TimeUnit.c_str());
                }
            }
        }
    }
    else
    {
        this->m_State = PUG_PAUSE_NONE;

        this->m_BuyTime = 0.0f;

        this->m_Team = UNASSIGNED;
    }
}

void CPugPause::Timer(int PauseTime)
{
    auto RemainTime = (static_cast<time_t>(PauseTime) - static_cast<time_t>(gpGlobals->time - CSGameRules()->m_fRoundStartTime));

    if (RemainTime > 0)
    {
        this->m_State = PUG_PAUSE_PAUSED;

        struct tm* tm_info = localtime(&RemainTime);

        if (tm_info)
        {
            char szTime[256] = { 0 };

            strftime(szTime, sizeof(szTime), _T("MATCH IS PAUSED %M:%S^nWAITING TO CONTINUE"), localtime(&RemainTime));
            
            gPugUtil.SendHud(nullptr, g_Pause_HudParam1, szTime);
        }
    }
    else
    {
        this->m_State = PUG_PAUSE_NONE;

        gPugTask.Remove(E_TASK::PAUSE_MATCH);

        g_engfuncs.pfnCvar_DirectSet(gPugCvar.m_MpBuyTime, std::to_string(this->m_BuyTime).c_str());

        this->SetRoundTime(static_cast<int>(gPugCvar.m_MpFreezeTime->value), true);

        edict_t *pEntity = nullptr;

        for (int i = 1; i <= gpGlobals->maxClients; ++i)
        {
            pEntity = INDEXENT(i);

            if (gPugUtil.IsNetClient(pEntity))
            {
                gPugUtil.ScreenShake(pEntity, 2.0f, 2.0f, 2.0f);

                gPugUtil.ScreenFade(pEntity, 2.0f, 2.0f, 0x0002, 0, 0, 200, 100);

                gPugUtil.ClientCommand(pEntity, "spk \"barney/letsgo\"");

                gPugUtil.SendHud(pEntity, g_Pause_HudParam2, _T("MATCH WILL CONTINUE AFTER FREEZETIME"));

                gPugUtil.PrintColor(pEntity, E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Match will continue after freezetime."), gPugCvar.m_Tag->string);
            }
        }
    }
}

void CPugPause::SetRoundTime(int Time, bool FreezePeriod)
{
	if (g_pGameRules)
	{
		if (!CSGameRules()->m_bCompleteReset)
		{
			CSGameRules()->m_bFreezePeriod = FreezePeriod;

			CSGameRules()->m_iRoundTimeSecs = Time;

			CSGameRules()->m_iIntroRoundTime = Time;

			CSGameRules()->m_fRoundStartTime = gpGlobals->time;
		}
	}

	static int iMsgRoundTime;

	if (iMsgRoundTime || (iMsgRoundTime = GET_USER_MSG_ID(PLID, "RoundTime", NULL)))
	{
        MESSAGE_BEGIN(MSG_ALL, iMsgRoundTime);
		WRITE_SHORT(Time);
		MESSAGE_END();
	}
}