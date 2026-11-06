#pragma once

constexpr std::array<hudtextparms_t, 4U> g_ReadyList_HudParam =
	{{
		{0.23f, 0.02f, 2, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0.0f, 0.0f, 0.93f, 0.93f, 1},
		{0.58f, 0.02f, 2, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0.0f, 0.0f, 0.93f, 0.93f, 2},
		{0.23f, 0.02f, 2, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0.0f, 0.0f, 0.93f, 0.93f, 3},
		{0.58f, 0.02f, 2, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0.0f, 0.0f, 0.93f, 0.93f, 4},
	}};

constexpr hudtextparms_t g_ReadyNum_HudParam = {0.01f, 0.20f, 2, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0.0f, 0.0f, 0.93f, 0.93f, 1};

class CPugReady
{
public:
	void ServerActivate();
	void ServerDeactivate();

	void Init(int NextState);
	void Stop(bool Forced);

	void PutInServer(edict_t *pEntity);
	void GetIntoGame(CBasePlayer *Player);
	void PlayerSpawn(CBasePlayer *Player);
	void DropClient(edict_t *pEntity);
	void RestartRound();
	void StartFrame();

	bool Ready(CBasePlayer *Player);
	bool NotReady(CBasePlayer *Player);

private:
	bool m_Run = false;
	float m_NextFrame = 0.0f;
	std::array<bool, MAX_CLIENTS + 1U> m_Ready = {};
	int m_PlayersMin = 0;
	int m_NextState = STATE_VOTEMAP;
};

extern CPugReady gPugReady;
