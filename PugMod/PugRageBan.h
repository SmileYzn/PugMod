#pragma once

typedef struct S_RAGE_BAN_INFO
{
	float CheckTime;
	std::string Name;
} P_RAGE_BAN_INFO, *LP_RAGE_BAN_INFO; 

class CPugRageBan
{
public:
	void DropClient(edict_t *pEntity);
	void CheckList();
private:
	std::map<std::string, P_RAGE_BAN_INFO> m_Data = {};
};

extern CPugRageBan gPugRageBan;