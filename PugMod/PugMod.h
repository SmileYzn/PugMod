#pragma once

constexpr auto STATE_DEAD = 0;
constexpr auto STATE_DEATHMATCH = 1;
constexpr auto STATE_VOTEMAP = 2;
constexpr auto STATE_VOTETEAM = 3;
constexpr auto STATE_CAPTAIN = 4;
constexpr auto STATE_KNIFE_ROUND = 5;
constexpr auto STATE_FIRST_HALF = 6;
constexpr auto STATE_HALFTIME = 7;
constexpr auto STATE_SECOND_HALF = 8;
constexpr auto STATE_OVERTIME = 9;
constexpr auto STATE_END = 10;

constexpr std::array<const char *, STATE_END + 1U> g_Pug_Config = {"pugmod", "deathmatch", "votemap", "voteteam", "captain", "kniferound", "esl", "halftime", "esl", "esl-ot", "end"};
constexpr std::array<const char *, STATE_END + 1U> g_Pug_String = {"Dead", "Deathmatch", "Vote Map", "Vote Team", "Captains", "Knife Round", "First Half", "Halftime", "Second Half", "Overtime", "Finished"};
constexpr std::array<const char *, SPECTATOR + 1U> g_Pug_TeamName = {"None", "Terrorist", "Counter-Terrorist", "Spectactor"};
constexpr std::array<const char *, SPECTATOR + 1U> g_Pug_TeamNameShort = {"NONE", "TR", "CT", "SPEC"};

constexpr hudtextparms_t g_Pug_HudParam = {-1.0f, 0.20f, 2, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0.0f, 0.0f, 12.0f, 3.0f, 1};

class CPugMod
{
public:
    void ServerActivate();
    void ServerDeactivate();

    int GetState();
    int SetState(int State);

    const char *GetString(int State);

    int GetRound();

    int GetScore(TeamName Team);
    void SetScore(TeamName Team, int State, int Score);

    int GetPoint(int EntityIndex, int Type);
    void SetPoint(int EntityIndex, int State, int Type, int Point);
    void ResetPoint(int State);

    void SwapTeams();
    void Surrender(TeamName Team);

    void GetIntoGame(CBasePlayer *Player);
    void DropClient(edict_t *pEntity);

    void RestartRound();
    void RoundStart();
    void RoundEnd(int winStatus, ScenarioEventEndRound event, float tmDelay);
    void RoundMessage();

    void Status(CBasePlayer *Player);
    void Scores(CBasePlayer *Player);

    static bool TeamScore(int msg_dest, int msg_type, const float *pOrigin, edict_t *pEntity);
    static bool ScoreInfo(int msg_dest, int msg_type, const float *pOrigin, edict_t *pEntity);

private:
    int m_State = STATE_DEAD;
    std::array<std::array<int, STATE_END + 1U>, SPECTATOR + 1U> m_Score = {};
    std::array<int, SPECTATOR + 1U> m_ScoreOT = {};
    std::array<std::array<std::array<int, 2U>, STATE_END + 1U>, MAX_CLIENTS + 1U> m_Point = {};
};

extern CPugMod gPugMod;