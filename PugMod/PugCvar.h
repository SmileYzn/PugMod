#pragma once

class CPugCvar
{
public:
	void ServerActivate();

	cvar_t* Register(const char *pszName, const char *pszValue);

	cvar_t* m_AllowSpectators;
	cvar_t* m_MpMaxMoney;
	cvar_t* m_MpFreezeTime;
	cvar_t* m_AutoTeamJoin;
	cvar_t* m_HumansJoinTeam;
	cvar_t* m_BotJoinTeam;
	cvar_t* m_RoundRestartDelay;
	cvar_t* m_SvRestart;
	cvar_t* m_SvRestartRound;
	cvar_t* m_MpBuyTime;
	cvar_t* m_MpFreeForAll;

	cvar_t* m_Tag;
	cvar_t* m_Language;
	cvar_t* m_CmdPrefixPlayer;
	cvar_t* m_CmdPrefixAdmin;
	cvar_t* m_CmdFloodTime;
	cvar_t* m_CmdFloodRepeat;
	cvar_t* m_CmdFloodNextTime;
	cvar_t* m_State;
	cvar_t* m_PlayGameMode;
	cvar_t* m_PlayKnifeRound;
	cvar_t* m_ModName;
	cvar_t* m_PlayersMin;
	cvar_t* m_PlayersMax;
	cvar_t* m_Rounds;
	cvar_t* m_RoundsOT;
	cvar_t* m_OvertimeType;
	cvar_t* m_ReadyType;
	cvar_t* m_ReadyList;
	cvar_t* m_ReadyTimeLimit;
	cvar_t* m_TeamType;
	cvar_t* m_TeamOption;
	cvar_t* m_RestrictItem;
	cvar_t* m_VoteDelay;
	cvar_t* m_VotePercent;
	cvar_t* m_VoteMap;
	cvar_t* m_LastMap;
	cvar_t* m_KnifeRound;
	cvar_t* m_VoteEnd;
	cvar_t* m_ScoreText;
	cvar_t* m_ScoreTeams;
	cvar_t* m_ScorePlayers;
	cvar_t* m_GameDesc;
	cvar_t* m_MotdFile;
	cvar_t* m_MotdFileAdmin;
	cvar_t* m_DemoRecord;
	cvar_t* m_RageQuitLimit;
	cvar_t* m_RageQuitBanTime;
	cvar_t* m_RoundStats;
	cvar_t* m_RoundEndStats;
	cvar_t* m_PlayerVoteKick;
	cvar_t* m_PlayerVoteMap;
	cvar_t* m_PlayerVotePause;
	cvar_t* m_PlayerVoteRestart;
	cvar_t* m_PlayerVoteCancel;
	cvar_t* m_PlayerVoteSurrender;
	cvar_t* m_PauseTimeout;
	cvar_t* m_SmokeBugFix;

	cvar_t* m_DM_Enable;
	cvar_t* m_DM_SpawnDistance;
	cvar_t* m_DM_HideKillFeed;
	cvar_t* m_DM_HitIndicator;
	cvar_t* m_DM_HSOnlyMode;
	cvar_t* m_DM_HudKDRatio;
	cvar_t* m_DM_KillFade;
	cvar_t* m_DM_KillHP;
	cvar_t* m_DM_KillHPHS;
	cvar_t* m_DM_KillRepairArmor;
	cvar_t* m_DM_KillHealedMsg;
	cvar_t* m_DM_KillSound;
	cvar_t* m_DM_MoneyFrag;

	cvar_t* m_ST_RwsTotalPoints;
	cvar_t* m_ST_RwsC4Explode;
	cvar_t* m_ST_RwsC4Defused;

	cvar_t* m_API_Enable;
	cvar_t* m_API_Address;
	cvar_t* m_API_Timeout;
	cvar_t* m_API_Bearer;
	cvar_t* m_API_Status;
};

extern CPugCvar gPugCvar;