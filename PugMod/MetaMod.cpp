#include "precompiled.h"

#pragma region METAMOD
plugin_info_t Plugin_info =
{
	META_INTERFACE_VERSION,
	"Pickup Game Mod",
	"1.0.1",
	__DATE__,
	"SmileY",
	"https://pugbr.net",
	"PUG",
	PT_STARTUP,
	PT_ANYTIME,
};

enginefuncs_t g_engfuncs;
globalvars_t *gpGlobals;
meta_globals_t *gpMetaGlobals;
gamedll_funcs_t *gpGamedllFuncs;
mutil_funcs_t *gpMetaUtilFuncs;
META_FUNCTIONS gMetaFunctionTable;

C_DLLEXPORT void WINAPI GiveFnptrsToDll(enginefuncs_t *pengfuncsFromEngine, globalvars_t *pGlobals)
{
	memcpy(&g_engfuncs, pengfuncsFromEngine, sizeof(enginefuncs_t));
	
	gpGlobals = pGlobals;
}

C_DLLEXPORT int Meta_Attach(PLUG_LOADTIME now, META_FUNCTIONS *pFunctionTable, meta_globals_t *pMGlobals, gamedll_funcs_t *pGamedllFuncs)
{
	gpMetaGlobals = pMGlobals;

	gpGamedllFuncs = pGamedllFuncs;

	memset(&gMetaFunctionTable, 0, sizeof(META_FUNCTIONS));

	gMetaFunctionTable.pfnGetEntityAPI2 = GetEntityAPI2;

	gMetaFunctionTable.pfnGetEntityAPI2_Post = GetEntityAPI2_Post;

	gMetaFunctionTable.pfnGetEngineFunctions = GetEngineFunctions;

	gMetaFunctionTable.pfnGetEngineFunctions_Post = GetEngineFunctions_Post;

	gMetaFunctionTable.pfnGetNewDLLFunctions = GetNewDLLFunctions;

	gMetaFunctionTable.pfnGetNewDLLFunctions_Post = GetNewDLLFunctions_Post;

	memcpy(pFunctionTable, &gMetaFunctionTable, sizeof(META_FUNCTIONS));

	ReAPI_Init();

	ReGameDLL_Init();

	return TRUE;
}

C_DLLEXPORT int Meta_Detach(PLUG_LOADTIME now, PL_UNLOAD_REASON reason)
{
	ReAPI_Stop();

	ReGameDLL_Stop();

	return TRUE;
}

C_DLLEXPORT int Meta_Query(char *interfaceVersion, plugin_info_t **pPlugInfo, mutil_funcs_t *pMetaUtilFuncs)
{
	*pPlugInfo = PLID;

	gpMetaUtilFuncs = pMetaUtilFuncs;

	return TRUE;
}
#pragma endregion

#pragma region META_DLL_PRE
DLL_FUNCTIONS g_DLL_FunctionTable_Pre;

C_DLLEXPORT int GetEntityAPI2(DLL_FUNCTIONS *pFunctionTable, int *interfaceVersion)
{
	memset(&g_DLL_FunctionTable_Pre, 0, sizeof(DLL_FUNCTIONS));

	g_DLL_FunctionTable_Pre.pfnClientCommand = DLL_PRE_ClientCommand;

	memcpy(pFunctionTable, &g_DLL_FunctionTable_Pre, sizeof(DLL_FUNCTIONS));

	return 1;
}

void DLL_PRE_ClientCommand(edict_t *pEntity)
{
	if (gPugClientCmd.Command(pEntity))
	{
		RETURN_META(MRES_SUPERCEDE);
	}

	RETURN_META(MRES_IGNORED);
}
#pragma endregion

#pragma region META_DLL_POST
DLL_FUNCTIONS g_DLL_FunctionTable_Post;

C_DLLEXPORT int GetEntityAPI2_Post(DLL_FUNCTIONS *pFunctionTable, int *interfaceVersion)
{
	memset(&g_DLL_FunctionTable_Post, 0, sizeof(DLL_FUNCTIONS));

	g_DLL_FunctionTable_Post.pfnServerActivate = DLL_POST_ServerActivate;

	g_DLL_FunctionTable_Post.pfnServerDeactivate = DLL_POST_ServerDeactivate;

	g_DLL_FunctionTable_Post.pfnStartFrame = DLL_POST_StartFrame;

	g_DLL_FunctionTable_Post.pfnClientPutInServer = DLL_POST_ClientPutInServer;

	g_DLL_FunctionTable_Post.pfnClientUserInfoChanged = DLL_POST_ClientUserInfoChanged;

	memcpy(pFunctionTable, &g_DLL_FunctionTable_Post, sizeof(DLL_FUNCTIONS));

	return 1;
}

void DLL_POST_ServerActivate(edict_t *pEdictList, int edictCount, int clientMax)
{
	gPugCvar.ServerActivate();

	gPugCurl.ServerActivate();

	gPugTask.ServerActivate();

	gPugRageBan.ServerActivate();

	gPugLang.ServerActivate();

	gPugAdmin.ServerActivate();

	gPugMapList.ServerActivate();

	gPugServerCmd.ServerActivate();

	gPugClientCmd.ServerActivate();

	gPugDM.ServerActivate();

	gPugSpawnEdit.ServerActivate();

	gPugMod.ServerActivate();

	gPugTeamManager.ServerActivate();

	gPugReady.ServerActivate();

	gPugTimer.ServerActivate();

	gPugVoteMap.ServerActivate();

	gPugVoteTeam.ServerActivate();

	gPugLeader.ServerActivate();

	gPugVoteSwapTeam.ServerActivate();

	gPugVoteOvertime.ServerActivate();

	gPugVoteEnd.ServerActivate();

	gPugDemoRecord.ServerActivate();

	gPugCvarControl.ServerActivate();

	gPugVoteMenu.ServerActivate();

	gPugPause.ServerActivate();

	gPugStats.ServerActivate();

	gPugStatus.ServerActivate();

	RETURN_META(MRES_IGNORED);
}

void DLL_POST_ServerDeactivate()
{
	gPugTask.ServerDeactivate();

	gPugMod.ServerDeactivate();

	gPugTeamManager.ServerDeactivate();

	gPugReady.ServerDeactivate();

	gPugTimer.ServerDeactivate();

	gPugDM.ServerDeactivate();

	gPugSpawnEdit.ServerDeactivate();

	gPugVoteMap.ServerDeactivate();

	gPugVoteTeam.ServerDeactivate();

	gPugLeader.ServerDeactivate();

	gPugVoteSwapTeam.ServerDeactivate();

	gPugVoteOvertime.ServerDeactivate();

	gPugVoteEnd.ServerDeactivate();

	gPugDemoRecord.ServerDeactivate();

	RETURN_META(MRES_IGNORED);
}

void DLL_POST_StartFrame()
{
	gPugCurl.StartFrame();

	gPugTask.StartFrame();

	gPugReady.StartFrame();

	gPugTimer.StartFrame();

	gPugVoteMap.StartFrame();

	gPugVoteTeam.StartFrame();

	gPugLeader.StartFrame();

	gPugVoteSwapTeam.StartFrame();

	gPugVoteOvertime.StartFrame();

	gPugVoteEnd.StartFrame();

	gPugStatus.StartFrame();

	RETURN_META(MRES_IGNORED);
}

void DLL_POST_ClientPutInServer(edict_t *pEntity)
{
	gPugMenu[ENTINDEX(pEntity)].Clear();
	
	gPugAdmin.PutInServer(pEntity);

	gPugReady.PutInServer(pEntity);

	gPugLeader.PutInServer(pEntity);

	RETURN_META(MRES_IGNORED);
}

void DLL_POST_ClientUserInfoChanged(edict_t *pEntity, char *infobuffer)
{
	RETURN_META(MRES_IGNORED);
}
#pragma endregion

#pragma region NEW_DLL_PRE
NEW_DLL_FUNCTIONS g_DLL_NewFunctionTable_Pre;

C_DLLEXPORT int GetNewDLLFunctions(NEW_DLL_FUNCTIONS *pNewFunctionTable, int *interfaceVersion)
{
	memset(&g_DLL_NewFunctionTable_Pre, 0, sizeof(NEW_DLL_FUNCTIONS));

	// Functions here

	memcpy(pNewFunctionTable, &g_DLL_NewFunctionTable_Pre, sizeof(NEW_DLL_FUNCTIONS));

	return 1;
}
#pragma endregion

#pragma region NEW_DLL_POST
NEW_DLL_FUNCTIONS g_DLL_NewFunctionTable_Post;

C_DLLEXPORT int GetNewDLLFunctions_Post(NEW_DLL_FUNCTIONS *pNewFunctionTable, int *interfaceVersion)
{
	memset(&g_DLL_NewFunctionTable_Post, 0, sizeof(NEW_DLL_FUNCTIONS));

	// Functions here

	memcpy(pNewFunctionTable, &g_DLL_NewFunctionTable_Post, sizeof(NEW_DLL_FUNCTIONS));

	return 1;
}
#pragma endregion

#pragma region ENGINE_PRE
enginefuncs_t g_ENGINE_FunctionTable_Pre;

C_DLLEXPORT int GetEngineFunctions(enginefuncs_t *pengfuncsFromEngine, int *interfaceVersion)
{
	memset(&g_ENGINE_FunctionTable_Pre, 0, sizeof(enginefuncs_t));

	g_ENGINE_FunctionTable_Pre.pfnMessageBegin = ENGINE_PRE_MessageBegin;
	g_ENGINE_FunctionTable_Pre.pfnMessageEnd = ENGINE_PRE_MessageEnd;
	g_ENGINE_FunctionTable_Pre.pfnWriteByte = ENGINE_PRE_WriteByte;
	g_ENGINE_FunctionTable_Pre.pfnWriteChar = ENGINE_PRE_WriteChar;
	g_ENGINE_FunctionTable_Pre.pfnWriteShort = ENGINE_PRE_WriteShort;
	g_ENGINE_FunctionTable_Pre.pfnWriteLong = ENGINE_PRE_WriteLong;
	g_ENGINE_FunctionTable_Pre.pfnWriteAngle = ENGINE_PRE_WriteAngle;
	g_ENGINE_FunctionTable_Pre.pfnWriteCoord = ENGINE_PRE_WriteCoord;
	g_ENGINE_FunctionTable_Pre.pfnWriteString = ENGINE_PRE_WriteString;
	g_ENGINE_FunctionTable_Pre.pfnWriteEntity = ENGINE_PRE_WriteEntity;

	memcpy(pengfuncsFromEngine, &g_ENGINE_FunctionTable_Pre, sizeof(enginefuncs_t));

	return 1;
}

void ENGINE_PRE_MessageBegin(int msg_dest, int msg_type, const float* pOrigin, edict_t* ed)
{
	if (gPugEngine.MessageBegin(msg_dest, msg_type, pOrigin, ed))
	{
		RETURN_META(MRES_SUPERCEDE);
	}
	else
	{
		RETURN_META(MRES_IGNORED);
	}
}

void ENGINE_PRE_MessageEnd(void)
{
	if (gPugEngine.MessageEnd())
	{
		RETURN_META(MRES_SUPERCEDE);
	}
	else
	{
		RETURN_META(MRES_IGNORED);
	}
}

void ENGINE_PRE_WriteByte(int iValue)
{
	if (gPugEngine.WriteByte(iValue))
	{
		RETURN_META(MRES_SUPERCEDE);
	}
	else
	{
		RETURN_META(MRES_IGNORED);
	}
}

void ENGINE_PRE_WriteChar(int iValue)
{
	if (gPugEngine.WriteChar(iValue))
	{
		RETURN_META(MRES_SUPERCEDE);
	}
	else
	{
		RETURN_META(MRES_IGNORED);
	}
}

void ENGINE_PRE_WriteShort(int iValue)
{
	if (gPugEngine.WriteShort(iValue))
	{
		RETURN_META(MRES_SUPERCEDE);
	}
	else
	{
		RETURN_META(MRES_IGNORED);
	}
}

void ENGINE_PRE_WriteLong(int iValue)
{
	if (gPugEngine.WriteLong(iValue))
	{
		RETURN_META(MRES_SUPERCEDE);
	}
	else
	{
		RETURN_META(MRES_IGNORED);
	}
}

void ENGINE_PRE_WriteAngle(float flValue)
{
	if (gPugEngine.WriteAngle(flValue))
	{
		RETURN_META(MRES_SUPERCEDE);
	}
	else
	{
		RETURN_META(MRES_IGNORED);
	}
}

void ENGINE_PRE_WriteCoord(float flValue)
{
	if (gPugEngine.WriteCoord(flValue))
	{
		RETURN_META(MRES_SUPERCEDE);
	}
	else
	{
		RETURN_META(MRES_IGNORED);
	}
}

void ENGINE_PRE_WriteString(const char* sz)
{
	if (gPugEngine.WriteString(sz))
	{
		RETURN_META(MRES_SUPERCEDE);
	}
	else
	{
		RETURN_META(MRES_IGNORED);
	}
}

void ENGINE_PRE_WriteEntity(int iValue)
{
	if (gPugEngine.WriteEntity(iValue))
	{
		RETURN_META(MRES_SUPERCEDE);
	}
	else
	{
		RETURN_META(MRES_IGNORED);
	}
}
#pragma endregion

#pragma region ENGINE_POST
enginefuncs_t g_ENGINE_FunctionTable_Post;

C_DLLEXPORT int GetEngineFunctions_Post(enginefuncs_t *pengfuncsFromEngine, int *interfaceVersion)
{
	memset(&g_ENGINE_FunctionTable_Post, 0, sizeof(enginefuncs_t));

	// Functions here

	memcpy(pengfuncsFromEngine, &g_ENGINE_FunctionTable_Post, sizeof(enginefuncs_t));

	return 1;
}
#pragma endregion
