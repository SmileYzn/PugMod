#include "precompiled.h"

IReGameApi *g_ReGameApi;
const ReGameFuncs_t *g_ReGameFuncs;
IReGameHookchains *g_ReGameHookchains;
CGameRules *g_pGameRules;

bool ReGameDLL_Init()
{
	auto szGameDLLModule = gpMetaUtilFuncs->pfnGetGameInfo(PLID, GINFO_DLL_FULLPATH);

	if (szGameDLLModule)
	{
		auto GameModule = Sys_GetModuleHandle(szGameDLLModule);

		if (GameModule)
		{
			auto ifaceFactory = Sys_GetFactory(GameModule);

			if (ifaceFactory)
			{
				int retCode = 0;

				g_ReGameApi = (IReGameApi *)ifaceFactory(VRE_GAMEDLL_API_VERSION, &retCode);

				if (g_ReGameApi)
				{
					if (g_ReGameApi->GetMajorVersion() == REGAMEDLL_API_VERSION_MAJOR)
					{
						if (g_ReGameApi->GetMinorVersion() >= REGAMEDLL_API_VERSION_MINOR)
						{
							if (g_ReGameApi->BGetICSEntity(CSENTITY_API_INTERFACE_VERSION))
							{
								if (g_ReGameApi->BGetIGameRules(GAMERULES_API_INTERFACE_VERSION))
								{
									g_ReGameFuncs = g_ReGameApi->GetFuncs();

									g_ReGameHookchains = g_ReGameApi->GetHookchains();

									if (g_ReGameHookchains)
									{
										g_ReGameHookchains->InstallGameRules()->registerHook(ReGameDLL_InstallGameRules);

										g_ReGameHookchains->CBasePlayer_UpdateClientData()->registerHook(ReGameDLL_CBasePlayer_UpdateClientData);

										g_ReGameHookchains->HandleMenu_ChooseTeam()->registerHook(ReGameDLL_HandleMenu_ChooseTeam);

										g_ReGameHookchains->CBasePlayer_GetIntoGame()->registerHook(ReGameDLL_CBasePlayer_GetIntoGame);

										g_ReGameHookchains->CBasePlayer_SwitchTeam()->registerHook(ReGameDLL_CBasePlayer_SwitchTeam);

										g_ReGameHookchains->CSGameRules_RestartRound()->registerHook(ReGameDLL_CSGameRules_RestartRound);

										g_ReGameHookchains->CSGameRules_GetPlayerSpawnSpot()->registerHook(ReGameDLL_CSGameRules_GetPlayerSpawnSpot);

										g_ReGameHookchains->CSGameRules_PlayerSpawn()->registerHook(ReGameDLL_CSGameRules_PlayerSpawn);

										g_ReGameHookchains->CBasePlayer_OnSpawnEquip()->registerHook(ReGameDLL_CBasePlayer_OnSpawnEquip);

										g_ReGameHookchains->ShowVGUIMenu()->registerHook(ReGameDLL_ShowVGUIMenu);

										g_ReGameHookchains->CSGameRules_FPlayerCanTakeDamage()->registerHook(ReGameDLL_CSGameRules_FPlayerCanTakeDamage);

										g_ReGameHookchains->CBasePlayer_TakeDamage()->registerHook(ReGameDLL_CBasePlayer_TakeDamage);

										g_ReGameHookchains->CSGameRules_SendDeathMessage()->registerHook(ReGameDLL_CSGameRules_SendDeathMessage);

										g_ReGameHookchains->CBasePlayer_StartDeathCam()->registerHook(ReGameDLL_CBasePlayer_StartDeathCam);

										g_ReGameHookchains->CBasePlayer_AddAccount()->registerHook(ReGameDLL_CBasePlayer_AddAccount);

										g_ReGameHookchains->CBasePlayer_SetAnimation()->registerHook(ReGameDLL_CBasePlayer_SetAnimation);

										g_ReGameHookchains->CBasePlayer_HasRestrictItem()->registerHook(ReGameDLL_CBasePlayer_HasRestrictItem);

										g_ReGameHookchains->CSGameRules_OnRoundFreezeEnd()->registerHook(ReGameDLL_CSGameRules_OnRoundFreezeEnd);

										g_ReGameHookchains->RoundEnd()->registerHook(ReGameDLL_RoundEnd);

										g_ReGameHookchains->CGrenade_ExplodeSmokeGrenade()->registerHook(ReGameDLL_CGrenade_ExplodeSmokeGrenade);

										g_ReGameHookchains->CBasePlayer_MakeBomber()->registerHook(ReGameDLL_CBasePlayer_MakeBomber);

										g_ReGameHookchains->CBasePlayer_DropPlayerItem()->registerHook(ReGameDLL_CBasePlayer_DropPlayerItem);

										g_ReGameHookchains->PlantBomb()->registerHook(ReGameDLL_PlantBomb);

										g_ReGameHookchains->CGrenade_DefuseBombStart()->registerHook(ReGameDLL_CGrenade_DefuseBombStart);

										g_ReGameHookchains->CGrenade_DefuseBombEnd()->registerHook(ReGameDLL_CGrenade_DefuseBombEnd);

										g_ReGameHookchains->CGrenade_ExplodeBomb()->registerHook(ReGameDLL_CGrenade_ExplodeBomb);

										g_ReGameHookchains->CBaseEntity_FireBullets3()->registerHook(ReGameDLL_CBaseEntity_FireBullets3);
									}

									gpMetaUtilFuncs->pfnLogConsole(PLID, "[%s] ReGameDLL API Loaded: %d.%d", Plugin_info.logtag, REGAMEDLL_API_VERSION_MAJOR, REGAMEDLL_API_VERSION_MINOR);

									return true;
								}
							}
						}
					}
				}
			}
		}
	}

	gpMetaUtilFuncs->pfnLogConsole(PLID, "[%s] ReGameDLL API Failed: %d.%d version is needed.", Plugin_info.logtag, REGAMEDLL_API_VERSION_MAJOR, REGAMEDLL_API_VERSION_MINOR);
	gpMetaUtilFuncs->pfnLogConsole(PLID, "[%s] Download at: https://github.com/rehlds/ReGameDLL_CS/actions", Plugin_info.logtag);

	return false;
}

bool ReGameDLL_Stop()
{
	if (g_ReGameHookchains)
	{
		g_ReGameHookchains->InstallGameRules()->unregisterHook(ReGameDLL_InstallGameRules);

		g_ReGameHookchains->CBasePlayer_UpdateClientData()->unregisterHook(ReGameDLL_CBasePlayer_UpdateClientData);

		g_ReGameHookchains->HandleMenu_ChooseTeam()->unregisterHook(ReGameDLL_HandleMenu_ChooseTeam);

		g_ReGameHookchains->CBasePlayer_GetIntoGame()->unregisterHook(ReGameDLL_CBasePlayer_GetIntoGame);

		g_ReGameHookchains->CBasePlayer_SwitchTeam()->unregisterHook(ReGameDLL_CBasePlayer_SwitchTeam);

		g_ReGameHookchains->CSGameRules_RestartRound()->unregisterHook(ReGameDLL_CSGameRules_RestartRound);

		g_ReGameHookchains->CSGameRules_GetPlayerSpawnSpot()->unregisterHook(ReGameDLL_CSGameRules_GetPlayerSpawnSpot);

		g_ReGameHookchains->CSGameRules_PlayerSpawn()->unregisterHook(ReGameDLL_CSGameRules_PlayerSpawn);

		g_ReGameHookchains->CBasePlayer_OnSpawnEquip()->unregisterHook(ReGameDLL_CBasePlayer_OnSpawnEquip);

		g_ReGameHookchains->ShowVGUIMenu()->unregisterHook(ReGameDLL_ShowVGUIMenu);

		g_ReGameHookchains->CSGameRules_FPlayerCanTakeDamage()->unregisterHook(ReGameDLL_CSGameRules_FPlayerCanTakeDamage);

		g_ReGameHookchains->CBasePlayer_TakeDamage()->unregisterHook(ReGameDLL_CBasePlayer_TakeDamage);

		g_ReGameHookchains->CSGameRules_SendDeathMessage()->unregisterHook(ReGameDLL_CSGameRules_SendDeathMessage);

		g_ReGameHookchains->CBasePlayer_StartDeathCam()->unregisterHook(ReGameDLL_CBasePlayer_StartDeathCam);

		g_ReGameHookchains->CBasePlayer_AddAccount()->unregisterHook(ReGameDLL_CBasePlayer_AddAccount);

		g_ReGameHookchains->CBasePlayer_SetAnimation()->unregisterHook(ReGameDLL_CBasePlayer_SetAnimation);

		g_ReGameHookchains->CBasePlayer_HasRestrictItem()->unregisterHook(ReGameDLL_CBasePlayer_HasRestrictItem);

		g_ReGameHookchains->CSGameRules_OnRoundFreezeEnd()->unregisterHook(ReGameDLL_CSGameRules_OnRoundFreezeEnd);

		g_ReGameHookchains->RoundEnd()->unregisterHook(ReGameDLL_RoundEnd);

		g_ReGameHookchains->CGrenade_ExplodeSmokeGrenade()->unregisterHook(ReGameDLL_CGrenade_ExplodeSmokeGrenade);

		g_ReGameHookchains->CBasePlayer_MakeBomber()->unregisterHook(ReGameDLL_CBasePlayer_MakeBomber);

		g_ReGameHookchains->CBasePlayer_DropPlayerItem()->unregisterHook(ReGameDLL_CBasePlayer_DropPlayerItem);

		g_ReGameHookchains->PlantBomb()->unregisterHook(ReGameDLL_PlantBomb);

		g_ReGameHookchains->CGrenade_DefuseBombStart()->unregisterHook(ReGameDLL_CGrenade_DefuseBombStart);

		g_ReGameHookchains->CGrenade_DefuseBombEnd()->unregisterHook(ReGameDLL_CGrenade_DefuseBombEnd);

		g_ReGameHookchains->CGrenade_ExplodeBomb()->unregisterHook(ReGameDLL_CGrenade_ExplodeBomb);

		g_ReGameHookchains->CBaseEntity_FireBullets3()->unregisterHook(ReGameDLL_CBaseEntity_FireBullets3);
	}

	return true;
}

CGameRules *ReGameDLL_InstallGameRules(IReGameHook_InstallGameRules *chain)
{
	auto Result = chain->callNext();

	if (g_ReGameApi)
	{
		g_pGameRules = Result;
	}

	return Result;
}

void ReGameDLL_CBasePlayer_UpdateClientData(IReGameHook_CBasePlayer_UpdateClientData *chain, CBasePlayer *Player)
{
	chain->callNext(Player);

	gPugDM.UpdateClientData(Player);
}

BOOL ReGameDLL_HandleMenu_ChooseTeam(IReGameHook_HandleMenu_ChooseTeam *chain, CBasePlayer *Player, int Slot)
{
	if (gPugTeamManager.ChooseTeam(Player, Slot))
	{
		Slot = 0;
		return FALSE;
	}

	auto ret = chain->callNext(Player, Slot);

	gPugStats.ChooseTeam(Player);

	return ret;
}

void ReGameDLL_CBasePlayer_SwitchTeam(IReGameHook_CBasePlayer_SwitchTeam *chain, CBasePlayer *Player)
{
	chain->callNext(Player);

	gPugStats.SwitchTeam(Player);
}

bool ReGameDLL_CBasePlayer_GetIntoGame(IReGameHook_CBasePlayer_GetIntoGame *chain, CBasePlayer *Player)
{
	auto Result = chain->callNext(Player);

	gPugMod.GetIntoGame(Player);

	gPugReady.GetIntoGame(Player);

	gPugDM.GetIntoGame(Player);

	gPugDemoRecord.GetIntoGame(Player);

	gPugRoundStats.GetIntoGame(Player);

	gPugStats.GetIntoGame(Player);

	return Result;
}

void ReGameDLL_CSGameRules_RestartRound(IReGameHook_CSGameRules_RestartRound *chain)
{
	gPugReady.RestartRound();

	gPugTimer.RestartRound();

	gPugMod.RestartRound();

	gPugVoteMenu.RestartRound();

	gPugStats.RestartRound();

	chain->callNext();

	gPugMod.RestartRound();

	gPugPause.RestartRound();
}

edict_t *ReGameDLL_CSGameRules_GetPlayerSpawnSpot(IReGameHook_CSGameRules_GetPlayerSpawnSpot *chain, CBasePlayer *Player)
{
	auto Result = chain->callNext(Player);

	if (gPugDM.GetPlayerSpawnSpot(Player))
	{
		return nullptr;
	}

	return Result;
}

void ReGameDLL_CSGameRules_PlayerSpawn(IReGameHook_CSGameRules_PlayerSpawn *chain, CBasePlayer *Player)
{
	chain->callNext(Player);

	gPugDM.PlayerSpawn(Player);

	gPugReady.PlayerSpawn(Player);

	gPugDemoRecord.PlayerSpawn(Player);
}

void ReGameDLL_CBasePlayer_OnSpawnEquip(IReGameHook_CBasePlayer_OnSpawnEquip *chain, CBasePlayer *Player, bool addDefault, bool equipGame)
{
	chain->callNext(Player, addDefault, equipGame);

	gPugDM.OnSpawnEquip(Player, addDefault, equipGame);
}

void ReGameDLL_ShowVGUIMenu(IReGameHook_ShowVGUIMenu *chain, CBasePlayer *Player, int MenuType, int BitMask, char *pszMenuText)
{
	if (gPugDM.ShowVGUIMenu(Player, MenuType, BitMask, pszMenuText))
	{
		return;
	}

	chain->callNext(Player, MenuType, BitMask, pszMenuText);
}

BOOL ReGameDLL_CSGameRules_FPlayerCanTakeDamage(IReGameHook_CSGameRules_FPlayerCanTakeDamage *chain, CBasePlayer *Player, CBaseEntity *Entity)
{
	if (gPugDM.FPlayerCanTakeDamage(Player, Entity))
	{
		return FALSE;
	}

	return chain->callNext(Player, Entity);
}

BOOL ReGameDLL_CBasePlayer_TakeDamage(IReGameHook_CBasePlayer_TakeDamage *chain, CBasePlayer *Player, entvars_t *pevInflictor, entvars_t *pevAttacker, float &flDamage, int bitsDamageType)
{
	auto Result = chain->callNext(Player, pevInflictor, pevAttacker, flDamage, bitsDamageType);

	gPugDM.TakeDamage(Player, pevInflictor, pevAttacker, flDamage, bitsDamageType);

	gPugRoundStats.TakeDamage(Player, pevInflictor, pevAttacker, flDamage, bitsDamageType);

	gPugStats.TakeDamage(Player, pevInflictor, pevAttacker, flDamage, bitsDamageType);

	return Result;
}

void ReGameDLL_CSGameRules_SendDeathMessage(IReGameHook_CSGameRules_SendDeathMessage *chain, CBaseEntity *KillerBaseEntity, CBasePlayer *Victim, CBasePlayer *Assister, entvars_t *pevInflictor, const char *killerWeaponName, int iDeathMessageFlags, int iRarityOfKill)
{
	if (gPugDM.SendDeathMessage(KillerBaseEntity, Victim, Assister, pevInflictor, killerWeaponName, iDeathMessageFlags, iRarityOfKill))
	{
		return;
	}

	chain->callNext(KillerBaseEntity, Victim, Assister, pevInflictor, killerWeaponName, iDeathMessageFlags, iRarityOfKill);

	gPugStats.SendDeathMessage(KillerBaseEntity, Victim, Assister, pevInflictor, killerWeaponName, iDeathMessageFlags, iRarityOfKill);
}

void ReGameDLL_CBasePlayer_StartDeathCam(IReGameHook_CBasePlayer_StartDeathCam *chain, CBasePlayer *Player)
{
	chain->callNext(Player);

	gPugRoundStats.StartDeathCam(Player);
}

void ReGameDLL_CBasePlayer_AddAccount(IReGameHook_CBasePlayer_AddAccount *chain, CBasePlayer *Player, int Amount, RewardType Type, bool TrackChange)
{
	if (gPugDM.AddAccount(Player, Amount, Type, TrackChange))
	{
		Amount = 0;
	}

	chain->callNext(Player, Amount, Type, TrackChange);

	gPugStats.AddAccount(Player, Amount, Type, TrackChange);
}

void ReGameDLL_CBasePlayer_SetAnimation(IReGameHook_CBasePlayer_SetAnimation *chain, CBasePlayer *Player, PLAYER_ANIM playerAnimation)
{
	chain->callNext(Player, playerAnimation);

	gPugDM.SetAnimation(Player, playerAnimation);

	gPugStats.SetAnimation(Player, playerAnimation);
}

bool ReGameDLL_CBasePlayer_HasRestrictItem(IReGameHook_CBasePlayer_HasRestrictItem *chain, CBasePlayer *Player, ItemID ItemIndex, ItemRestType RestType)
{
	if (gPugRestrictItem.HasRestrictItem(Player, ItemIndex, RestType))
	{
		return true;
	}

	return chain->callNext(Player, ItemIndex, RestType);
}

void ReGameDLL_CSGameRules_OnRoundFreezeEnd(IReGameHook_CSGameRules_OnRoundFreezeEnd *chain)
{
	chain->callNext();

	gPugMod.RoundStart();

	gPugRoundStats.RoundStart();

	gPugStats.RoundStart();
}

bool ReGameDLL_RoundEnd(IReGameHook_RoundEnd *chain, int winStatus, ScenarioEventEndRound event, float tmDelay)
{
	auto Result = chain->callNext(winStatus, event, tmDelay);

	gPugMod.RoundEnd(winStatus, event, tmDelay);

	gPugStats.RoundEnd(winStatus, event, tmDelay);

	gPugRoundStats.RoundEnd(winStatus, event, tmDelay);

	return Result;
}

void ReGameDLL_CGrenade_ExplodeSmokeGrenade(IReGameHook_CGrenade_ExplodeSmokeGrenade *chain, CGrenade *pThis)
{
	chain->callNext(pThis);

	gPugSmokeGrenade.ExplodeSmokeGrenade(pThis);
}

bool ReGameDLL_CBasePlayer_MakeBomber(IReGameHook_CBasePlayer_MakeBomber *chain, CBasePlayer *pthis)
{
	auto ret = chain->callNext(pthis);

	gPugStats.MakeBomber(pthis);

	return ret;
}

CBaseEntity *ReGameDLL_CBasePlayer_DropPlayerItem(IReGameHook_CBasePlayer_DropPlayerItem *chain, CBasePlayer *pthis, const char *pszItemName)
{
	auto ret = chain->callNext(pthis, pszItemName);

	gPugStats.DropPlayerItem(pthis, pszItemName);

	return ret;
}

CGrenade *ReGameDLL_PlantBomb(IReGameHook_PlantBomb *chain, entvars_t *pevOwner, Vector &vecStart, Vector &vecVelocity)
{
	gPugStats.PlantBomb(pevOwner, false);

	auto ret = chain->callNext(pevOwner, vecStart, vecVelocity);

	gPugStats.PlantBomb(pevOwner, true);

	return ret;
}

void ReGameDLL_CGrenade_DefuseBombStart(IReGameHook_CGrenade_DefuseBombStart *chain, CGrenade *pthis, CBasePlayer *pPlayer)
{
	chain->callNext(pthis, pPlayer);

	gPugStats.DefuseBombStart(pPlayer);
}

void ReGameDLL_CGrenade_DefuseBombEnd(IReGameHook_CGrenade_DefuseBombEnd *chain, CGrenade *pthis, CBasePlayer *pPlayer, bool bDefused)
{
	gPugStats.DefuseBombEnd(pPlayer, bDefused);

	chain->callNext(pthis, pPlayer, bDefused);
}

void ReGameDLL_CGrenade_ExplodeBomb(IReGameHook_CGrenade_ExplodeBomb *chain, CGrenade *pthis, TraceResult *ptr, int bitsDamageType)
{
	gPugStats.ExplodeBomb(pthis, ptr, bitsDamageType);

	chain->callNext(pthis, ptr, bitsDamageType);
}

Vector &ReGameDLL_CBaseEntity_FireBullets3(IReGameHook_CBaseEntity_FireBullets3 *chain, CBaseEntity *pEntity, Vector &vecSrc, Vector &vecDirShooting, float vecSpread, float flDistance, int iPenetration, int iBulletType, int iDamage, float flRangeModifier, entvars_t *pevAttacker, bool bPistol, int shared_rand)
{
	vecSpread = gPugSpread.GetSpread(pEntity, vecSrc, vecDirShooting, vecSpread, flDistance, iPenetration, iBulletType, iDamage, flRangeModifier, pevAttacker, bPistol, shared_rand);

	return chain->callNext(pEntity, vecSrc, vecDirShooting, vecSpread, flDistance, iPenetration, iBulletType, iDamage, flRangeModifier, pevAttacker, bPistol, shared_rand);
}