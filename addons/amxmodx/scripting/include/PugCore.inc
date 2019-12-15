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
#define PUG_MOD_VERSION		"3.0.8"
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

/**
 * Call next Pug Mod State
 *
 * @note This is useful for call Pug Mod states automatically.
 *
 * @noreturn
 */
native PUG_NextState();

/**
 * Called just after the Pug Mod change the state
 *
 * @note This is useful to do what you want :D
 *
 * @param State		The new state that was called
 *
 * @noreturn
 */
forward PUG_Event(iState);

/**
 * Get Pug Mod State
 *
 * @note This return current state of pug mod
 *
 * @noreturn
 */
stock PUG_GetState()
{
	if(xvar_exists("g_iState"))
	{
		return get_xvar_num(get_xvar_id("g_iState"));
	}
	
	return STATE_DEAD;
}
