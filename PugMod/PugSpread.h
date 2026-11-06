#pragma once

class CPugSpread
{
public:
	float GetSpread(CBaseEntity *pEntity, Vector &vecSrc, Vector &vecDirShooting, float vecSpread, float flDistance, int iPenetration, int iBulletType, int iDamage, float flRangeModifier, entvars_t *pevAttacker, bool bPistol, int shared_rand);
};

extern CPugSpread gPugSpread;