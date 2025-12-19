#pragma once

class CPugSpawnEdit
{
public:
	void ServerActivate();
	void ServerDeactivate();
	void EditSpawns(CBasePlayer* Player);
	void Menu(CBasePlayer* Player, int Page);
	void MenuHandle(CBasePlayer *Player, P_MENU_ITEM Item);
	void AddSpawnMenu(CBasePlayer* Player);
	void AddSpawnMenuHandle(CBasePlayer* Player, P_MENU_ITEM Item);
	void ToggleNoClip(CBasePlayer* Player);
	void DiscardChanges(CBasePlayer* Player);
	void DiscardChangesHandle(CBasePlayer* Player, P_MENU_ITEM Item);
	edict_t* MakeEntity(int EntityIndex);
	void RemoveEntity(int EntityIndex);
	void GlowEnt(int EntityIndex, vec3_t Color);
	void UnGlowEnt(int EntityIndex);
	bool IsStuck(int EntityIndex);
	void SetPosition(int EntityIndex);
	void AddSpawn(CBasePlayer* Player, int Team);
	void EditSpawn(CBasePlayer *Player);
	void DeleteSpawn(int EntityIndex);
	int ClosestSpawn(edict_t* pEdict);
	void Refresh(CBasePlayer* Player);
	void ShowStats(CBasePlayer* Player);
	void Save(CBasePlayer* Player);
	void ShowStuckedSpawns(CBasePlayer* Player);
	void RespawnPlayer(CBasePlayer* Player, int SpawnIndex);
private:
	string_t m_InfoTarget = 0;
	std::map<int, P_SPAWN> m_Spawns = {};
	std::map<int, edict_t*> m_Entities = {};
	std::map<int, int> m_Marked = {};
};

extern CPugSpawnEdit gPugSpawnEdit;