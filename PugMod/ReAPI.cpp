#include "precompiled.h"
#include "interface.cpp"

IRehldsApi *g_RehldsApi;
const RehldsFuncs_t *g_RehldsFuncs;
IRehldsServerData *g_RehldsData;
IRehldsHookchains *g_RehldsHookchains;
IRehldsServerStatic *g_RehldsSvs;

bool ReAPI_Init()
{
	if (IS_DEDICATED_SERVER())
	{
#ifdef WIN32
		auto engineModule = Sys_GetModuleHandle("swds.dll");
#else
		auto engineModule = Sys_GetModuleHandle("engine_i486.so");
#endif

		if (engineModule)
		{
			auto ifaceFactory = Sys_GetFactory(engineModule);

			if (ifaceFactory)
			{
				int retCode = 0;

				g_RehldsApi = (IRehldsApi *)ifaceFactory(VREHLDS_HLDS_API_VERSION, &retCode);

				if (g_RehldsApi)
				{
					if (g_RehldsApi->GetMajorVersion() == REHLDS_API_VERSION_MAJOR)
					{
						if (g_RehldsApi->GetMinorVersion() >= REHLDS_API_VERSION_MINOR)
						{
							g_RehldsFuncs = g_RehldsApi->GetFuncs();

							g_RehldsData = g_RehldsApi->GetServerData();

							g_RehldsHookchains = g_RehldsApi->GetHookchains();

							g_RehldsSvs = g_RehldsApi->GetServerStatic();

							if (g_RehldsHookchains)
							{
								g_RehldsHookchains->SV_DropClient()->registerHook(ReAPI_SV_DropClient);
							}

							gpMetaUtilFuncs->pfnLogConsole(PLID, "[%s] ReHLDS API Loaded: %d.%d", Plugin_info.logtag, REHLDS_API_VERSION_MAJOR, REHLDS_API_VERSION_MINOR);

							return true;
						}
					}
				}
			}
		}
	}

	gpMetaUtilFuncs->pfnLogConsole(PLID, "[%s] ReHLDS API Failed: %d.%d version is needed.", Plugin_info.logtag, REHLDS_API_VERSION_MAJOR, REHLDS_API_VERSION_MINOR);
	gpMetaUtilFuncs->pfnLogConsole(PLID, "[%s] Download at: https://github.com/rehlds/ReHLDS/actions", Plugin_info.logtag);

	return false;
}

bool ReAPI_Stop()
{
	if (g_RehldsHookchains)
	{
		g_RehldsHookchains->SV_DropClient()->unregisterHook(ReAPI_SV_DropClient);
	}

	return true;
}

void ReAPI_SV_DropClient(IRehldsHook_SV_DropClient *chain, IGameClient *client, bool crash, const char *Reason)
{
	if (client)
	{
		auto pEntity = client->GetEdict();

		if (!FNullEnt(pEntity))
		{
			gPugMod.DropClient(pEntity);

			gPugTeamManager.DropClient(pEntity);

			gPugReady.DropClient(pEntity);

			gPugLeader.DropClient(pEntity);

			gPugVoteMenu.DropClient(pEntity);

			gPugRoundStats.DropClient(pEntity);

			gPugRageBan.DropClient(pEntity, client);

			gPugStats.DropClient(pEntity);
		}
	}
	
	chain->callNext(client, crash, Reason);
}