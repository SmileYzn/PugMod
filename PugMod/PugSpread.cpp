#include "precompiled.h"

CPugSpread gPugSpread;

float CPugSpread::GetSpread(CBaseEntity *pEntity, Vector &vecSrc, Vector &vecDirShooting, float vecSpread, float flDistance, int iPenetration, int iBulletType, int iDamage, float flRangeModifier, entvars_t *pevAttacker, bool bPistol, int shared_rand)
{
	if (gPugCvar.m_SpreadFix->value)
	{
		if (!FNullEnt(pEntity) && pEntity->IsPlayer())
		{
			auto Player = static_cast<CBasePlayer *>(pEntity);

			if ((Player->pev->flags & FL_ONGROUND) && Player->pev->velocity.Length() <= (Player->pev->maxspeed / 2.0f))
			{
				auto Weapon = static_cast<CBasePlayerWeapon *>(Player->m_pActiveItem);

				if (Weapon)
				{
					auto Sniper = (Weapon->m_iId == WEAPON_SCOUT || Weapon->m_iId == WEAPON_SG550 || Weapon->m_iId == WEAPON_AWP || Weapon->m_iId == WEAPON_G3SG1);

					// Snipers only when scoped; other weapons only on the first shot of a burst
					if ((Sniper && Player->m_bResumeZoom) || (!Sniper && Weapon->m_iShotsFired <= 1))
					{
						return 0.0f;
					}
				}
			}
		}
	}

	return vecSpread;
}
