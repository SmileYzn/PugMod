#pragma once

constexpr auto PUG_PAUSE_NONE = 0;
constexpr auto PUG_PAUSE_PAUSE = 1;
constexpr auto PUG_PAUSE_PAUSED = 2;

constexpr hudtextparms_t g_Pause_HudParam1 = {-1.0f, 0.30f, 0, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0.0f, 0.0f, 0.53f, 0.53f, 3};
constexpr hudtextparms_t g_Pause_HudParam2 = {-1.0f, 0.20f, 2, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0.0f, 0.0f, 12.0f, 2.0f, 3};

class CPugPause
{
public:
	void ServerActivate();
	void Init(CBasePlayer* Player);
	void Init(TeamName Team);
	void RestartRound();
	void Timer(int PauseTime);
	void SetRoundTime(int Time, bool FreezePeriod);

private:
	int m_State = PUG_PAUSE_NONE;
	float m_BuyTime = 0.0f;
	TeamName m_Team = UNASSIGNED;
};

extern CPugPause gPugPause;