#pragma once

constexpr std::array<hudtextparms_t, 2U> g_VoteOvertime_HudParam =
{{
	{0.23f, 0.02f, 2, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0.0f, 0.0f, 0.93f, 0.93f, 1},
	{0.23f, 0.05f, 2, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0.0f, 0.0f, 0.93f, 0.93f, 2},
}};

constexpr std::array<const char*, 3U> g_VoteOverimte_Sound =
{
	"spk gman/gman_Choose1",
	"spk gman/gman_Choose2",
	"spk scientist/letsgo",
};

typedef struct S_VOTE_OT_INFO
{
	int Index;
	int Votes;
	std::string Name;
} P_VOTE_OT_INFO, *LP_VOTE_OT_INFO;

class CPugVoteOvertime
{
public:
	void ServerActivate();
	void ServerDeactivate();
	void Init();
	void Stop();
	P_VOTE_OT_INFO GetWinner();
	void MenuHandle(CBasePlayer *Player, P_MENU_ITEM Item);
	void StartFrame();

private:
	bool m_Run = false;
	float m_NextFrame = 0.0f;
	time_t m_Time = 0;
	int m_VotesLeft = 0;
	std::vector<P_VOTE_OT_INFO> m_VoteList = {};
	std::array<bool, MAX_CLIENTS + 1U> m_Voter = {};
};

extern CPugVoteOvertime gPugVoteOvertime;