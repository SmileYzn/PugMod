#pragma once

#define BIT_LEADER_TYPE BIT(24)
#define BIT_LEADER_MENU BIT(25)

constexpr std::array<hudtextparms_t, 6U> g_Leader_HudParam =
{{
	{0.70f, 0.02f, 2, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0.0f, 0.0f, 0.93f, 0.93f, 1}, // Terroristas (%d / %d):
	{0.70f, 0.02f, 2, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0.0f, 0.0f, 0.93f, 0.93f, 2}, // \n%s

	{0.70f, 0.28f, 2, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0.0f, 0.0f, 0.93f, 0.93f, 3}, // CTs (%d / %d):
	{0.70f, 0.28f, 2, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0.0f, 0.0f, 0.93f, 0.93f, 4}, // \n%s

	{0.70f, 0.28f, 2, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0.0f, 0.0f, 0.93f, 0.93f, 3}, // CTs (%d / %d):\n\n\n\n\n\nEspectadores (%d):
	{0.70f, 0.28f, 2, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0.0f, 0.0f, 0.93f, 0.93f, 4}, // \n%s\n%s
}};

class CPugLeader
{
public:
	void ServerActivate();
	void ServerDeactivate();

	void Init();
	void Stop();

	void PutInServer(edict_t *pEntity);
	void DropClient(edict_t *pEntity);

	void SetLeader(CBasePlayer *Player, int Team);
	void GetPlayer(CBasePlayer *Leader, CBasePlayer *Target);
	void Next(CBasePlayer *Leader);

	void Menu(CBasePlayer *Leader);
	void MenuHandle(CBasePlayer *Player, P_MENU_ITEM Item);

	void GetRandomPlayer(int EntityIndex);
	void StartFrame();

private:
	bool m_Run = false;
	float m_NextFrame = 0.0f;
	unsigned int m_PlayersMin = 0;
	std::map<int, int> m_Team = {};
};

extern CPugLeader gPugLeader;