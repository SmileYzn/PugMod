#if defined _PugCore_included
	#endinput
#endif
#define _PugCore_included

#include <amxmodx>
#include <amxmisc>
#include <cstrike>
#include <fakemeta>
#include <hamsandwich>

#if AMXX_VERSION_NUM < 200
	#assert AMX Mod X v1.10 or later library required!
#endif

#pragma reqlib "PugCore"
	#if !defined AMXMODX_NOAUTOLOAD
	#pragma loadlib "PugCore"
#endif

#define PUG_MOD_PLUGIN 		"CS Pug Mod"
#define PUG_MOD_VERSION		"3.1.1"
#define PUG_MOD_AUTHOR		"SmileY"
#define PUG_MOD_HEADER		"^4[PUG]^1"

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

stock const PUG_MOD_STATES_STR[PUG_MOD_STATES][] =
{
	"Dead",
	"Warmup",
	"Starting",
	"First Half",
	"Half Time",
	"Second Half",
	"Overtime",
	"Finished"
};

stock const PUG_MOD_CS_TEAMS_STR[CsTeams][] =
{
	"Unassigned",
	"Terrorists",
	"Counter-Terrorists",
	"Spectators"
};

/**
 * Call next Pug Mod State
 *
 * @note This is useful for call Pug Mod states automatically.
 *
 * @noreturn
 */
native PUG_RunState();

/**
 * Called just after the Pug Mod change the state
 *
 * @note This is useful to do what you want :D
 *
 * @param iState		The new state that was called
 *
 * @noreturn
 */
forward PUG_Event(iState);

/**
 * Get Pug Mod State
 *
 * @note This return current state of pug mod
 *
 * @return	Return the state enum
 */
native PUG_GetState();

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
 * Stop the pug with a winner: 1 Team TR, 2 Team CT
 *
 * @param iTeam     	The team index to set as winner
 *
 * @note If an invalid team is passed, game will end as tied
 *
 * @noreturn
 */
native PUG_SetEnded(iTeam);
