#pragma once

class CPugTeamManager
{
public:
	void ServerActivate();
	void ServerDeactivate();

	bool ChooseTeam(CBasePlayer *Player, int Slot);
	void DropClient(edict_t *pEntity);

private:
	std::map<std::string, int> m_Data = {};
};

extern CPugTeamManager gPugTeamManager;