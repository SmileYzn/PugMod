#include "precompiled.h"

CPugtimer gPugTimer;

void CPugtimer::ServerActivate()
{
	this->m_Run = false;

	this->m_NextFrame = 0.0f;

	this->m_Time = 0;

	this->m_PlayersMin = 0;

	this->m_NextState = STATE_VOTEMAP;
}

void CPugtimer::ServerDeactivate()
{
	this->m_Run = false;

	this->m_NextFrame = 0.0f;

	this->m_Time = 0;

	this->m_PlayersMin = 0;

	this->m_NextState = STATE_VOTEMAP;
}

void CPugtimer::Init(int NextState)
{
	this->m_Run = true;

	this->m_NextFrame = 0.0f;

	this->m_Time = time(nullptr);

	this->m_PlayersMin = static_cast<int>(gPugCvar.m_PlayersMin->value * 2.0f);

	this->m_NextState = NextState;

	gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Starts when ^3%d^1 players join the game."), gPugCvar.m_Tag->string, this->m_PlayersMin);
}

void CPugtimer::Stop(bool Forced)
{
	if (this->m_Run)
	{
		this->m_Run = false;

		this->m_Time = 0;

		this->m_PlayersMin = 0;

		if (!Forced)
		{
			gPugTask.Create(E_TASK::SET_STATE, 2.0f, false, this->m_NextState);

			gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Time's up: ^3All players must be ready!"), gPugCvar.m_Tag->string);
		}
	}
}

void CPugtimer::RestartRound()
{
	if (this->m_Run)
	{
		if (g_pGameRules)
		{
			if (CSGameRules()->m_bCompleteReset)
			{
				this->Init(this->m_NextState);
			}
		}
	}
}

void CPugtimer::StartFrame()
{
	if (this->m_Run)
	{
		if (g_pGameRules)
		{
			if (gpGlobals->time >= this->m_NextFrame)
			{
				auto Needed = (this->m_PlayersMin - (CSGameRules()->m_iNumTerrorist + CSGameRules()->m_iNumCT));

				if (Needed > 0)
				{
					this->m_Time = time(nullptr);

					auto State = gPugMod.GetState();

					if (Needed > 1)
					{
						gPugUtil.SendHud(nullptr, g_Timer_HudParam, _T("%s^n%d Players Required"), gPugMod.GetString(State), Needed);
					}
					else
					{
						gPugUtil.SendHud(nullptr, g_Timer_HudParam, _T("%s^n%d Player Required"), gPugMod.GetString(State), Needed);
					}
				}
				else
				{
					time_t RemainTime = (time_t)((time_t)(gPugCvar.m_ReadyTimeLimit->value) - (time(nullptr) - this->m_Time));

					if (RemainTime > 0)
					{
						char szTime[32] = {0};

						strftime(szTime, sizeof(szTime), _T("MATCH STARTING IN^n%M:%S"), localtime(&RemainTime));

						gPugUtil.SendHud(nullptr, g_Timer_HudParam, szTime);
					}
					else
					{
						this->Stop(false);
					}
				}

				this->m_NextFrame = (gpGlobals->time + 0.9f);
			}
		}
	}
}