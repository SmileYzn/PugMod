#include "precompiled.h"

CPugCvar gPugCvar;

void CPugCvar::ServerActivate()
{
    // allow_spectators
    this->m_AllowSpectators = CVAR_GET_POINTER("allow_spectators");

    // mp_maxmoney
    this->m_MpMaxMoney = CVAR_GET_POINTER("mp_maxmoney");

    // mp_freezetime
    this->m_MpFreezeTime = CVAR_GET_POINTER("mp_freezetime");

    // mp_auto_join_team
    this->m_AutoTeamJoin = CVAR_GET_POINTER("mp_auto_join_team");

    // humans_join_team
    this->m_HumansJoinTeam = CVAR_GET_POINTER("humans_join_team");

    // bot_join_team
    this->m_BotJoinTeam = CVAR_GET_POINTER("bot_join_team");

    // mp_round_restart_delay
    this->m_RoundRestartDelay = CVAR_GET_POINTER("mp_round_restart_delay");

    // sv_restart
	this->m_SvRestart = CVAR_GET_POINTER("sv_restart");

    // sv_restartround
	this->m_SvRestartRound = CVAR_GET_POINTER("sv_restartround");

    // mp_buytime
    this->m_MpBuyTime = CVAR_GET_POINTER("mp_buytime");

    // Log Tag
    //
    // Default "PUG"
    this->m_Tag = this->Register("pug_tag", "PUG");

    // Server language
    //
    // Default "en"
    this->m_Language = this->Register("pug_language", "en");

    // Player command prefix
    //
    // Default "."
    this->m_CmdPrefixPlayer = this->Register("pug_cmd_prefix_player", ".");

    // Admin command prefix
    //
    // Default "!"
    this->m_CmdPrefixAdmin = this->Register("pug_cmd_prefix_admin", "!");

    // Time between commands to consider a flood attempt
    //
    // Default "0.75"
    this->m_CmdFloodTime = this->Register("pug_cmd_flood_time", "0.75");

    // Maximum flood attempts before blocking
    //
    // Default "3.0"
    this->m_CmdFloodRepeat = this->Register("pug_cmd_flood_repeat", "3.0");

    // Anti-flood block time
    //
    // Default "4.0"
    this->m_CmdFloodNextTime = this->Register("pug_cmd_flood_next_time", "4.0");

    // The current pug mod state
    // WARNING: Do not use this cvar on server configurations
    //
    // Default "0"
    this->m_State = this->Register("pug_state", "0");

    // The current pug game mode
    // WARNING: Do not use this cvar on server configurations
    //
    // Default "0"
	this->m_PlayGameMode = this->Register("pug_game_mode", "0");

    // The current match have knife round
    // WARNING: Do not use this cvar on server configurations
    //
    // Default "0"
	this->m_PlayKnifeRound = this->Register("pug_knife_round_mode", "0");

    // The PUG game mode
    // The string used in server status
    //
    // Default "PUG"
    this->m_ModName = this->Register("pug_mod_name", "PUG");

    // Minimum players per team
    //
    // Default "5"
    this->m_PlayersMin = this->Register("pug_players_min", "5");

    // Maximum players per team in game
    //
    // Default "5"
    this->m_PlayersMax = this->Register("pug_players_max", "5");

    // Total rounds per map
    //
    // Default "30"
    this->m_Rounds = this->Register("pug_rounds", "30");

    // Total overtime rounds per match
    //
    // Default "6"
    this->m_RoundsOT = this->Register("pug_rounds_ot", "6");

    // Overtime type
    //
    // 1 Play overtime
    // 2 Allow game end tied
    // 3 Sudden death
    //
    // Default "0"
    this->m_OvertimeType = this->Register("pug_ot_type", "0");

    // Ready system mode
    //
    // 0 Disabled
    // 1 Automatic (Timer)
    // 2 Players use the ready system
    //
    // Default "1"
    this->m_ReadyType = this->Register("pug_ready_type", "1");

    // Ready system list
    //
    // 0 Show full list of Ready / Warming
    // 1 Number of players Ready / Warming
    //
    // Default "0"
    this->m_ReadyList = this->Register("pug_ready_list", "0");

    // Time in seconds to start the match in warmup mode
    // Counting from the moment all players are in the game
    //
    // Default "60.0"
    this->m_ReadyTimeLimit = this->Register("pug_ready_time_limit", "60.0");

    // Team system mode
    // 
    // 0 Vote
    // 1 Captains
    // 2 Shuffle Players
    // 3 None
    // 4 Balance Skill
    // 5 Switch Teams
    // 6 Knife Round
    //
    // Default "0"
    this->m_TeamType = this->Register("pug_team_type", "0");

    // Options in the team choice vote menu
    //
    // a Captains
    // b Shuffle Players
    // c None
    // d Balance Skill
    // e Switch Teams
    // f Knife Round
    //
    // Default "abcdef"
    this->m_TeamOption = this->Register("pug_team_option", "abcdef");

    // Restrict weapons at the indicated slot
    // Each slot is one item (See enum ItemID for slots)
    //
    // 0 Disabled
    // 1 Enabled
    //
    // Default "000000000000000000000000000000000000000"
    this->m_RestrictItem = this->Register("pug_restrict_item", "000000000000000000000000000000000000000");

    // The pug vote time limit
    //
    // Default "15.0"
    this->m_VoteDelay = this->Register("pug_vote_delay", "15.0");

    // The pug vote percentage for success in votes
    //
    // Default "0.70"
    this->m_VotePercent = this->Register("pug_vote_percent", "0.70");

    // The current map has a vote to change to
    // WARNING: Do not use this cvar on server configurations
    //
    // 0 Disabled
    // 1 Enabled
    //
    // Default "1"
    this->m_VoteMap = this->Register("pug_vote_map", "0");

    // The last map played on server will disable the vote map option
    // WARNING: Do not use this cvar on server configurations
    //
    // Default ""
    this->m_LastMap = this->Register("pug_last_map", STRING(gpGlobals->mapname));

    // Force knife round on the current map
    //
    // 0 Disabled
    // 1 Enabled
    //
    // Default "0"
    this->m_KnifeRound = this->Register("pug_knife_round", "0");

    // Prompt end of match when a player disconnects
    //
    // 0 Disabled
    // 1 Enabled
    //
    // Default "1"
    this->m_VoteEnd  = this->Register("pug_vote_end", "1");

    // Type of score text in chat
    //
    // 0 Written ([PUG] Terrorists are winning: 8-6)
    // 1 Both Scores (Terrorists (8) - (6) Counter-Terrorists)
    //
    // Default "0"
    this->m_ScoreText = this->Register("pug_score_text", "0");

    // Keep total team score in the scoreboard (TAB)
    //
    // 0 Reset score each period
    // 1 Keep total score across periods
    //
    // Default "1"
	this->m_ScoreTeams = this->Register("pug_score_teams", "1");

    // Keep player scores in the scoreboard (TAB)
    //
    // 0 Reset scores each period
    // 1 Keep scores across periods
    //
    // Default "1"
	this->m_ScorePlayers = this->Register("pug_score_players", "1");

    // Display match states and scores in game description
    //
    // 0 Disabled
    // 1 Enabled
    //
    // Default "1"
    this->m_GameDesc = this->Register("pug_game_desc", "1");

    // Help file for commands via MOTD
    // File name or URL of the help file
    //
    // Default "cstrike/addons/pugmod/motd.html"
    this->m_MotdFile = this->Register("pug_motd_file", "motd.html");

    // Admin help file for commands via MOTD
    // File name or URL of the help file
    //
    // Default "cstrike/addons/pugmod/motd_admin.html"
    this->m_MotdFileAdmin = this->Register("pug_motd_admin_file", "motd_admin.html");

    // Ask for record a client-side demo when the match starts
    //
    // 0 Disabled
    // 1 Enabled
    //
    // Default "1"
    this->m_DemoRecord = this->Register("pug_demo_record", "1");

    // Rage quit time limit to ban in seconds
    //
    // 0 Disabled
    //
    // Default "0"
	this->m_RageQuitLimit = this->Register("pug_rage_quit_time_limit", "0");

    // Rage quit ban time in minutes
    //
    // 0 Ban permanently
    //
    // Default "1440"
    this->m_RageQuitBanTime = this->Register("pug_rage_quit_ban_time", "1440");

    // Flags to enable stats commands
    //
    // a Enables .hp command
    // b Enables .dmg command
    // c Enables .rdmg command
    // d Enables .sum command
    // e Enables .stats command
    //
    // Default "abcde"
    this->m_RoundStats = this->Register("pug_round_stats", "abcde");

    // Display round-end statistics
    //
    // a Execute .hp
    // b Execute .dmg
    // c Execute .rdmg
    // d Execute .sum
    // e Execute .stats
    //
    // Default "e"
    this->m_RoundEndStats = this->Register("pug_round_end_stats", "e");

    // Allow players to use vote kick command
    //
    // 0 Disable
    // 1 Enable
    // 
    // Default "1"
    this->m_PlayerVoteKick = this->Register("pug_player_vote_kick", "1");

    // Allow players to use vote map command
    //
    // 0 Disable
    // 1 Enable
    // 
    // Default "1"
	this->m_PlayerVoteMap = this->Register("pug_player_vote_map", "1");

    // Allow players to use vote pause command
    //
    // 0 Disable
    // 1 Enable
    // 
    // Default "1"
	this->m_PlayerVotePause = this->Register("pug_player_vote_pause", "1");

    // Allow players to use vote restart command
    //
    // 0 Disable
    // 1 Enable
    // 
    // Default "1"
	this->m_PlayerVoteRestart = this->Register("pug_player_vote_restart", "1");

    // Allow players to use vote cancel command
    //
    // 0 Disable
    // 1 Enable
    // 
    // Default "1"
	this->m_PlayerVoteCancel = this->Register("pug_player_vote_cancel", "1");

    // Allow players to vote use surrender command
    //
    // 0 Disable
    // 1 Enable
    // 
    // Default "1"
	this->m_PlayerVoteSurrender = this->Register("pug_player_vote_surrender", "1");

    // Allowed time in seconds for match pause
    //
    // 0 Disable
    //
    // Default "60"
    this->m_PauseTimeout = this->Register("pug_pause_timeout", "60");

    // Fix smoke grenade bug (multiple smoke clouds)
    //
    // 0 Disable
    // 1 Enable
    //
    // Default "1"
    this->m_SmokeBugFix = this->Register("pug_smoke_bug_fix", "1");

    // Enable Deathmatch mode
    // If inactive, it will be regular warmup mode
    //
    // 0 Disabled
    // 1 Enabled
    //
    // Default "1"
    this->m_DM_Enable = this->Register("pug_dm_enable", "1");

    // Minimum distance between players for a player to respawn
    // WARNING: Minimum recomended is 250.0
    //
    // Default "250.0"
    this->m_DM_SpawnDistance = this->Register("pug_dm_spawn_distance", "250.0");

    // Hide kill feed from other players
    //
    // 0 Disabled
    // 1 Enabled
    //
    // Default "0"
    this->m_DM_HideKillFeed = this->Register("pug_dm_hide_kill_feed", "0");

    // Hit indicator on screen
    // 
    // 0 Disabled
    // 1 Enabled
    // 2 Active + Through Walls
    //
    // Default "1"
    this->m_DM_HitIndicator = this->Register("pug_dm_hit_indicator", "1");

    // Only accept headshots
    //
    // 0 Disabled
    // 1 Enabled
    //
    // Default "0"
    this->m_DM_HSOnlyMode = this->Register("pug_dm_hs_mode", "0");

    // Show kill/frag rate and HS rate on the HUD
    //
    // 0 Disabled
    // 1 Enabled
    //
    // Default "1"
    this->m_DM_HudKDRatio = this->Register("pug_dm_hud_kd_ratio", "1");

    // Flash the screen on killing another player (HS)
    //
    // 0 Disabled
    // 1 Enabled
    //
    // Default "1"
    this->m_DM_KillFade = this->Register("pug_dm_kill_fade", "2");

    // Recover health after a frag
    //
    // 0 Disabled
    //
    // Default "15"
    this->m_DM_KillHP = this->Register("pug_dm_kill_hp", "15");

    // Recover health after a frag (HS)
    //
    // 0 Disabled
    //
    // Default "40"
    this->m_DM_KillHPHS = this->Register("pug_dm_kill_hp_hs", "40");

    // Recover armor after a frag
    //
    // 0 Disabled
    // 1 Enabled
    // 2 Only on HS
    //
    // Default "1"
    this->m_DM_KillRepairArmor = this->Register("pug_dm_kill_repair_armor ", "1");

    // Show HP recovered after a frag
    //
    // 0 Disabled
    // 1 Enabled
    //
    // Default "1"
    this->m_DM_KillHealedMsg = this->Register("pug_dm_kill_hp_msg ", "1");

    // Enable sound on frag (HS)
    //
    // 0 Disabled
    // 1 Enabled
    //
    // Default "1"
    this->m_DM_KillSound = this->Register("pug_dm_kill_sound", "2");

    // Show frags instead of money
    //
    // 0 Disabled
    // 1 Enabled
    //
    // Default "1"
    this->m_DM_MoneyFrag = this->Register("pug_dm_money_frag", "0");

	// Round Win Share
    // Total of Round Win Share points to be divided by winner team of round
    //
    // Default "100"
	this->m_ST_RwsTotalPoints = gPugCvar.Register("pug_stats_total_points", "100");
	
	// Round Win Share
    // Extra amount added to player from winner team that planted the bomb and bomb explode
    //
    // Default "30"
	this->m_ST_RwsC4Explode = gPugCvar.Register("pug_stats_rws_c4_explode", "30");

	// Round Win Share
    // Extra amount added to player from winner team that defused the bomb
    //
    // Default "30"
	this->m_ST_RwsC4Defused = gPugCvar.Register("pug_stats_rws_c4_defused", "30");

	// Enable Stats Remote API
    //
    // Default "0"
	this->m_API_Enable = gPugCvar.Register("pug_api_enable", "0");

	// Stats Remote API Address (HTTP protocol API)
    //
    // Default ""
    this->m_API_Address = gPugCvar.Register("pug_api_address", "");

	// Stats Remote API Timeout
    // Timeout in seconds to wait for response from remote server
    //
    // Default "10"
    this->m_API_Timeout = gPugCvar.Register("pug_api_timeout", "10");

	// Stats Remote API Bearer Token send in HTTP Header
    // Leave empty to disable it
    //
    // Default ""
    this->m_API_Bearer = gPugCvar.Register("pug_api_bearer_token", "");

    // Time (in seconds) to send server status to API
    // The minimum value is 20.0, if is less that will disable update
    //
    // Default "0"
    this->m_API_Status = gPugCvar.Register("pug_api_status_update", "0");
}

cvar_t *CPugCvar::Register(const char *pszName, const char *pszValue)
{
    cvar_t *pPointer = CVAR_GET_POINTER(pszName);

    if (!pPointer)
    {
        if (pszName)
        {
            if (pszName[0u] != '\0')
            {
                static cvar_t Data;

                Data.name = pszName;

                if (pszValue)
                {
                    if (pszValue[0u] != '\0')
                    {
                        auto Temp = strdup(pszValue);

                        Data.string = Temp;

                        free(Temp);
                    }
                }
                
                Data.flags = (FCVAR_SERVER | FCVAR_PROTECTED | FCVAR_SPONLY | FCVAR_UNLOGGED);

                CVAR_REGISTER(&Data);

                pPointer = CVAR_GET_POINTER(Data.name);

                if (pPointer)
                {
                    g_engfuncs.pfnCvar_DirectSet(pPointer, pszValue);
                }
            }
        }
    }

    return pPointer;
}
