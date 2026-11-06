#pragma once

constexpr hudtextparms_t g_Timer_HudParam = {-1.0f, 0.20f, 2, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0.0f, 0.0f, 0.92f, 0.92f, 1};

class CPugtimer
{
public:
	void ServerActivate();
	void ServerDeactivate();

	void Init(int NextState);
	void Stop(bool Forced);
	void RestartRound();
	void StartFrame();

private:
	bool m_Run = false;
	float m_NextFrame = 0.0f;
	time_t m_Time = 0;
	int m_PlayersMin = 0;
	int m_NextState = STATE_VOTEMAP;
};

extern CPugtimer gPugTimer;
