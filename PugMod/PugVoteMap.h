#pragma once

constexpr std::array<hudtextparms_t, 2U> g_VoteMap_HudParam =
{{
	{0.23f, 0.02f, 2, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0.0f, 0.0f, 0.93f, 0.93f, 1},
	{0.23f, 0.05f, 2, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0.0f, 0.0f, 0.93f, 0.93f, 2},
}};

constexpr std::array<const char*, 3U> g_VoteMap_Sound =
{
	"spk gman/gman_Choose1",
	"spk gman/gman_Choose2",
	"spk scientist/letsgo"
};

typedef struct S_VOTE_MAP_INFO
{
	int Index;
	int Votes;
	std::string Name;
} P_VOTE_MAP_INFO, *LP_VOTE_MAP_INFO;

class CPugVoteMap
{
public:
	void ServerActivate();
	void ServerDeactivate();
	void Init();
	void Stop();
	void LoadMaps();
	P_VOTE_MAP_INFO GetWinner();
	P_VOTE_MAP_INFO GetRandom();
	void MenuHandle(CBasePlayer *Player, P_MENU_ITEM Item);
	void StartFrame();
private:
	bool m_Run = false;
	float m_NextFrame = 0.0f;
	time_t m_Time = 0;
	int m_VotesLeft = 0;
	std::vector<P_VOTE_MAP_INFO> m_MapList = {};
	std::array<bool, MAX_CLIENTS + 1U> m_Voter = {};
};

extern CPugVoteMap gPugVoteMap;