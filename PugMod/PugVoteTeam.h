#pragma once

constexpr std::array<hudtextparms_t, 2U> g_VoteTeam_HudParam =
{{
    {0.23f, 0.02f, 2, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0.0f, 0.0f, 0.93f, 0.93f, 1},
    {0.23f, 0.05f, 2, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0.0f, 0.0f, 0.93f, 0.93f, 2},
}};

constexpr std::array<const char*, 4U> g_VoteTeam_Sound =
{
    "spk gman/gman_Choose1",
    "spk gman/gman_Choose2",
    "spk scientist/letsgo",
    "spk scientist/sci_pain8",
};

constexpr auto VOTE_TEAM_CAPTAIN = 1;
constexpr auto VOTE_TEAM_RANDOM = 2;
constexpr auto VOTE_TEAM_UNSORTED = 3;
constexpr auto VOTE_TEAM_SKILL_BALANCE = 4;
constexpr auto VOTE_TEAM_SWAP_SIDES = 5;
constexpr auto VOTE_TEAM_KNIFE_ROUND = 6;

typedef struct S_VOTE_TEAM_INFO
{
    int Index;
    int Votes;
    std::string Name;
} P_VOTE_TEAM_INFO, *LP_VOTE_TEAM_INFO;

class CPugVoteTeam
{
public:
    void ServerActivate();
    void ServerDeactivate();

    void Init();
    void Stop();

    void ChangeTeam(int Type);
    P_VOTE_TEAM_INFO GetWinner();

    void MenuHandle(CBasePlayer *Player, P_MENU_ITEM Item);
    void StartFrame();

    void TeamsRamdomize();
    void TeamsOptimize();

private:
    bool m_Run = false;
    float m_NextFrame = 0.0f;
    time_t m_Time = 0;
    int m_VotesLeft = 0;
    std::vector<P_VOTE_TEAM_INFO> m_VoteList = {};
    std::array<bool, MAX_CLIENTS + 1U> m_Voter = {};
};

extern CPugVoteTeam gPugVoteTeam;