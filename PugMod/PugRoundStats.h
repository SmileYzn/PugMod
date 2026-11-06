#pragma once

constexpr auto R_STATS_ALL = 0;		   /* All Options */
constexpr auto R_STATS_HP = BIT(0);	   /* flag "a" */
constexpr auto R_STATS_DMG = BIT(1);   /* flag "b" */
constexpr auto R_STATS_RDMG = BIT(2);  /* flag "c" */
constexpr auto R_STATS_SUM = BIT(3);   /* flag "d" */
constexpr auto R_STATS_STATS = BIT(4); /* flag "e" */

constexpr hudtextparms_t g_RoundStats_HudParam = {0.70f, 0.28f, 2, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0.0f, 0.0f, 12.0f, 3.0f, 2};

class CPugRoundStats
{
public:
	void RoundStart();
	void RoundEnd(int winStatus, ScenarioEventEndRound eventScenario, float tmDelay);
	void GetIntoGame(CBasePlayer *Player);
	void DropClient(edict_t *pEntity);
	void TakeDamage(CBasePlayer *Victim, entvars_t *pevInflictor, entvars_t *pevAttacker, float& flDamage, int bitsDamageType);
	void StartDeathCam(CBasePlayer *Player);
	bool ShowHP(CBasePlayer *Player);
	bool ShowDamage(CBasePlayer *Player);
	bool ShowReceivedDamage(CBasePlayer *Player);
	bool ShowSummary(CBasePlayer *Player);
	bool ShowStats(CBasePlayer *Player);
private:
	int m_Flags = R_STATS_ALL;
	std::array<std::array<int, MAX_CLIENTS + 1U>, MAX_CLIENTS + 1U> m_RoundDmg;
	std::array<std::array<int, MAX_CLIENTS + 1U>, MAX_CLIENTS + 1U> m_RoundHit;
};

extern CPugRoundStats gPugRoundStats;