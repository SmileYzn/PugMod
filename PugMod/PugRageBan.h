#pragma once

typedef struct S_RAGE_BAN_INFO
{
	time_t CheckTime;
	std::string Name;
	std::string Auth;
	std::string Address;
	bool GenericAuth;
} P_RAGE_BAN_INFO, *LP_RAGE_BAN_INFO;

class CPugRageBan
{
public:
	void ServerActivate();
	void DropClient(edict_t *pEntity, IGameClient *client);
	void CheckList();
private:
	bool IsGenericAuthId(const char *AuthId);
	std::string GetAddress(IGameClient *client);

	// Key: SteamID, or the IP address when the SteamID is generic
	std::map<std::string, P_RAGE_BAN_INFO> m_Data = {};
};

extern CPugRageBan gPugRageBan;
