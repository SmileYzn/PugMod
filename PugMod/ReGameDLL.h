#pragma once

extern IReGameApi *g_ReGameApi;
extern const ReGameFuncs_t *g_ReGameFuncs;
extern IReGameHookchains *g_ReGameHookchains;
extern CGameRules *g_pGameRules;

extern bool ReGameDLL_Init();
extern bool ReGameDLL_Stop();

CGameRules *ReGameDLL_InstallGameRules(IReGameHook_InstallGameRules *chain);
void ReGameDLL_CBasePlayer_UpdateClientData(IReGameHook_CBasePlayer_UpdateClientData *chain, CBasePlayer *Player);
BOOL ReGameDLL_HandleMenu_ChooseTeam(IReGameHook_HandleMenu_ChooseTeam *chain, CBasePlayer *Player, int Slot);
void ReGameDLL_CBasePlayer_SwitchTeam(IReGameHook_CBasePlayer_SwitchTeam *chain, CBasePlayer *Player);
bool ReGameDLL_CBasePlayer_GetIntoGame(IReGameHook_CBasePlayer_GetIntoGame *chain, CBasePlayer *Player);
void ReGameDLL_CSGameRules_RestartRound(IReGameHook_CSGameRules_RestartRound *chain);
edict_t *ReGameDLL_CSGameRules_GetPlayerSpawnSpot(IReGameHook_CSGameRules_GetPlayerSpawnSpot *chain, CBasePlayer *Player);
void ReGameDLL_CSGameRules_PlayerSpawn(IReGameHook_CSGameRules_PlayerSpawn *chain, CBasePlayer *Player);
void ReGameDLL_CBasePlayer_OnSpawnEquip(IReGameHook_CBasePlayer_OnSpawnEquip *chain, CBasePlayer *Player, bool addDefault, bool equipGame);
void ReGameDLL_ShowVGUIMenu(IReGameHook_ShowVGUIMenu *chain, CBasePlayer *Player, int MenuType, int BitMask, char *pszMenuText);
BOOL ReGameDLL_CSGameRules_FPlayerCanTakeDamage(IReGameHook_CSGameRules_FPlayerCanTakeDamage *chain, CBasePlayer *Player, CBaseEntity *Entity);
BOOL ReGameDLL_CBasePlayer_TakeDamage(IReGameHook_CBasePlayer_TakeDamage *chain, CBasePlayer *Player, entvars_t *pevInflictor, entvars_t *pevAttacker, float &flDamage, int bitsDamageType);
void ReGameDLL_CSGameRules_SendDeathMessage(IReGameHook_CSGameRules_SendDeathMessage *chain, CBaseEntity *KillerBaseEntity, CBasePlayer *Victim, CBasePlayer *Assister, entvars_t *pevInflictor, const char *killerWeaponName, int iDeathMessageFlags, int iRarityOfKill);
void ReGameDLL_CBasePlayer_StartDeathCam(IReGameHook_CBasePlayer_StartDeathCam *chain, CBasePlayer *Player);
void ReGameDLL_CBasePlayer_AddAccount(IReGameHook_CBasePlayer_AddAccount *chain, CBasePlayer *Player, int Amount, RewardType Type, bool TrackChange);
void ReGameDLL_CBasePlayer_SetAnimation(IReGameHook_CBasePlayer_SetAnimation *chain, CBasePlayer *Player, PLAYER_ANIM playerAnimation);
bool ReGameDLL_CBasePlayer_HasRestrictItem(IReGameHook_CBasePlayer_HasRestrictItem *chain, CBasePlayer *Player, ItemID ItemIndex, ItemRestType RestType);
void ReGameDLL_CSGameRules_RestartRound(IReGameHook_CSGameRules_RestartRound *chain);
void ReGameDLL_CSGameRules_OnRoundFreezeEnd(IReGameHook_CSGameRules_OnRoundFreezeEnd *chain);
bool ReGameDLL_RoundEnd(IReGameHook_RoundEnd *chain, int winStatus, ScenarioEventEndRound event, float tmDelay);
void ReGameDLL_CGrenade_ExplodeSmokeGrenade(IReGameHook_CGrenade_ExplodeSmokeGrenade *chain, CGrenade *pThis);
bool ReGameDLL_CBasePlayer_MakeBomber(IReGameHook_CBasePlayer_MakeBomber *chain, CBasePlayer *pthis);
CBaseEntity *ReGameDLL_CBasePlayer_DropPlayerItem(IReGameHook_CBasePlayer_DropPlayerItem *chain, CBasePlayer *pthis, const char *pszItemName);
CGrenade *ReGameDLL_PlantBomb(IReGameHook_PlantBomb *chain, entvars_t *pevOwner, Vector &vecStart, Vector &vecVelocity);
void ReGameDLL_CGrenade_DefuseBombStart(IReGameHook_CGrenade_DefuseBombStart *chain, CGrenade *pthis, CBasePlayer *pPlayer);
void ReGameDLL_CGrenade_DefuseBombEnd(IReGameHook_CGrenade_DefuseBombEnd *chain, CGrenade *pthis, CBasePlayer *pPlayer, bool bDefused);
void ReGameDLL_CGrenade_ExplodeBomb(IReGameHook_CGrenade_ExplodeBomb *chain, CGrenade *pthis, TraceResult *ptr, int bitsDamageType);
Vector &ReGameDLL_CBaseEntity_FireBullets3(IReGameHook_CBaseEntity_FireBullets3 *chain, CBaseEntity *pEntity, Vector &vecSrc, Vector &vecDirShooting, float vecSpread, float flDistance, int iPenetration, int iBulletType, int iDamage, float flRangeModifier, entvars_t *pevAttacker, bool bPistol, int shared_rand);