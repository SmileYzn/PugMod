#if defined _PugCore_included
	#endinput
#endif
#define _PugCore_included

#include <amxmisc>
#include <reapi>

#pragma semicolon 1

#if AMXX_VERSION_LOCAL_REV_NUM < 5449
	#assert AMX Mod X version 1.10.0.5449 or later required!
#endif

#if REAPI_VERSION < 520236
	#assert ReAPI version 5.20.236 or later required!
#endif

#pragma reqlib "PugCore"
	#if !defined AMXMODX_NOAUTOLOAD
	#pragma loadlib "PugCore"
#endif

/**
 * Pug Mod version definition
 */
#define PUG_MOD_PLUGIN          "CS Pug Mod"
#define PUG_MOD_VERSION         "4.0.2"
#define PUG_MOD_AUTHOR		"SmileY"
#define PUG_MOD_HEADER		"^4[PUG]^1"

/**
 * Pug Mod states
 */
enum _:PUG_MOD_STATES
{
	STATE_DEAD,
	STATE_WARMUP,
	STATE_START,
	STATE_FIRST_HALF,
	STATE_HALFTIME,
	STATE_SECOND_HALF,
	STATE_OVERTIME,
	STATE_END
};

/**
 * Pug Mod Team names (Counter-Strike)
 */
stock const PUG_MOD_CS_TEAMS_STR[TeamName][] =
{
	"Unassigned",
	"Terrorists",
	"Counter-Terrorists",
	"Spectators"
};

/**
 * Call next Pug Mod State
 *
 * @noreturn
 */
native PUG_RunState();

/**
 * Called just after the Pug Mod change the state
 *
 * @param iState		The new state that was called
 * @param szState               The new state name was called
 *
 * @noreturn
 */
forward PUG_Event(iState,szState[]);

/**
 * Get Pug Mod State, and optionally retrieves the name of state
 *
 * @param szState       Buffer to copy state name to
 * @param iLen          Maximum size of buffer
 *
 * @note                This return current state of pug mod
 *
 * @return              Return the state enum index
 */
native PUG_GetState(szState[] = "",iLen = 0);

/**
 * Get a team score
 *
 * @param iTeam     	The team index to get score
 *
 * @note Will return -1 if a invalid team is passed
 *
 * @return      Return score on success, -1 on fail
 */
native PUG_GetScore(iTeam);

/**
 * Get current round count
 *
 * @note Will return 0 if match is not started
 *
 * @return      Return rounds played
 */
native PUG_GetRound();

/**
 * Get state of reset rounds
 *
 * @note        Return 0 if is not set, 1 otherwise
 *
 * @return      Return the state of reset rounds
 */
native PUG_GetReset();

/**
 * Stop the pug with a winner: TEAM_TERRORIST, TEAM_CT, TEAM_UNASSIGNED
 *
 * @param iTeam     	The team index to set as winner
 *
 * @note If team is TEAM_UNASSIGNED, game will end tied
 *
 * @noreturn
 */
native PUG_SetEnded(TeamName:iWinner);
