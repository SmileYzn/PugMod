#include "precompiled.h"

CPugSpread gPugSpread;

float CPugSpread::GetSpread(CBaseEntity *pEntity, Vector &vecSrc, Vector &vecDirShooting, float vecSpread, float flDistance, int iPenetration, int iBulletType, int iDamage, float flRangeModifier, entvars_t *pevAttacker, bool bPistol, int shared_rand)
{
	auto Player = static_cast<CBasePlayer*>(pEntity);

	if (Player)
	{
		if (!(Player->pev->flags & FL_ONGROUND))
		{
			return vecSpread;
		}

		if (Player->pev->velocity.Length() > (Player->pev->maxspeed / 2.0f))
		{
			return vecSpread;
		}

		if (Player->m_pActiveItem)
		{
			if (Player->m_pActiveItem->m_iId == WEAPON_SCOUT || Player->m_pActiveItem->m_iId == WEAPON_SG550 || Player->m_pActiveItem->m_iId == WEAPON_AWP || Player->m_pActiveItem->m_iId == WEAPON_G3SG1)
			{
				if (!Player->m_bResumeZoom)
				{
					return vecSpread;
				}
			}

			auto Weapon = static_cast<CBasePlayerWeapon *>(Player->m_pActiveItem);

			if (Weapon)
			{
				if (Q_fabs(Weapon->m_flNextPrimaryAttack) > 0.0f)
				{
					vecSpread = 0.0f;
				}
			}
		}
	}

	return vecSpread;
}