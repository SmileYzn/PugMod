#include "precompiled.h"

CPugSmokeGrenade gPugSmokeGrenade;

void CPugSmokeGrenade::ExplodeSmokeGrenade(CGrenade* Entity)
{
	if (gPugCvar.m_SmokeBugFix->value > 0.0f)
	{
		if (!FNullEnt(Entity))
		{
			Entity->edict()->v.origin.y -= 100.0f;

			PLAYBACK_EVENT_FULL(FEV_GLOBAL, 0, Entity->m_usEvent, 0.0f, Entity->edict()->v.origin, Entity->edict()->v.angles, 0.0f, 0.0f, 0, 1, 1, 0);
			PLAYBACK_EVENT_FULL(FEV_GLOBAL, 0, Entity->m_usEvent, 0.0f, Entity->edict()->v.origin, Entity->edict()->v.angles, 0.0f, 0.0f, 0, 1, 1, 0);
		}
	}
}