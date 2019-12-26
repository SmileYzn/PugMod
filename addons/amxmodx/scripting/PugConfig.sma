#include <PugCore>
#include <PugStocks>

new g_pConfig[PUG_MOD_STATES];

public plugin_init()
{
	register_plugin("Pug Mod (Configs)",PUG_MOD_VERSION,PUG_MOD_AUTHOR);
	
	g_pConfig[STATE_DEAD]		= create_cvar("pug_cfg_pugmod","pugmod.rc",FCVAR_NONE,"Config executed for pugmod cvars");
	g_pConfig[STATE_WARMUP]		= create_cvar("pug_cfg_warmup","warmup.rc",FCVAR_NONE,"Used at warmup session in pug mod");
	g_pConfig[STATE_START]		= create_cvar("pug_cfg_start","start.rc",FCVAR_NONE,"Executed when vote session starts");
	g_pConfig[STATE_FIRST_HALF]	= create_cvar("pug_cfg_1st","esl.rc",FCVAR_NONE,"Used when the match begin (Live config)");
	g_pConfig[STATE_HALFTIME]	= create_cvar("pug_cfg_halftime","halftime.rc",FCVAR_NONE,"Used at half-time session");
	g_pConfig[STATE_SECOND_HALF]	= create_cvar("pug_cfg_2nd","esl.rc",FCVAR_NONE,"Used when the match begin (Live config)");
	g_pConfig[STATE_OVERTIME]	= create_cvar("pug_cfg_overtime","esl-ot.rc",FCVAR_NONE,"Used at Overtime session");
	g_pConfig[STATE_END]		= create_cvar("pug_cfg_end","end.rc",FCVAR_NONE,"Executed when the match ends");
}

public plugin_cfg()
{
	PUG_Event(STATE_DEAD);
}

public PUG_Event(iState)
{
	new szFile[MAX_NAME_LENGTH];
	get_pcvar_string(g_pConfig[iState],szFile,charsmax(szFile));
	
	if(szFile[0])
	{
		new szPath[PLATFORM_MAX_PATH];
		PUG_GetFilePath(szFile,szPath,charsmax(szPath));
		
		server_cmd("exec %s",szPath);
	}
}
