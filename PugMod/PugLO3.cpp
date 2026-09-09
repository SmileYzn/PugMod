#include "precompiled.h"

CPugLO3 gPugLO3;

void CPugLO3::Init(int Restart)
{
    if (Restart >= 0)
    {
        if (g_pGameRules)
        {
            auto Players = gPugUtil.GetPlayers(false, false);

            for (auto const & Player : Players)
            {
                gPugUtil.ScreenShake(Player->edict(), 2.0f, 2.0f, 2.0f);

                gPugUtil.ScreenFade(Player->edict(), 2.0f, 2.0f, 0x0002, RANDOM_LONG(0, 255), RANDOM_LONG(0, 255), RANDOM_LONG(0, 255), 100);

                if (gPugCvar.m_SoundEnable->value != 0.0f)
                {
                    gPugUtil.ClientCommand(Player->edict(), g_LO3_Sound[Restart]);
                }

                gPugUtil.SendHud(Player->edict(), g_LO3_HudParam, _T(g_LO3_HudText[Restart]));

                gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T(g_LO3_Message[Restart]), gPugCvar.m_Tag->string, gPugMod.GetString(gPugMod.GetState()));
            }

            if (Restart > 0)
            {
                CSGameRules()->m_bGameStarted = false;
                CSGameRules()->m_bCompleteReset = true;

                g_engfuncs.pfnCvar_DirectSet(gPugCvar.m_SvRestart, "1");

                gPugTask.Create(E_TASK::LO3_RESTART, 3.0f, false, (Restart - 1));
            }
            else
            {
                CSGameRules()->m_bGameStarted = true;
                CSGameRules()->m_bCompleteReset = false;
            }
        }
    }
}