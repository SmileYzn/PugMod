#include <PugCore>
#include <PugStocks>

new g_szConfig[PUG_MOD_STATES][MAX_NAME_LENGTH];

public plugin_init()
{
	register_plugin("Pug Mod (Configs)",PUG_MOD_VERSION,PUG_MOD_AUTHOR);

	bind_pcvar_string(create_cvar("pug_cfg_pugmod","pugmod.rc",FCVAR_NONE,"Config executed for pugmod cvars"),g_szConfig[STATE_DEAD],sizeof(g_szConfig[]));
	bind_pcvar_string(create_cvar("pug_cfg_warmup","warmup.rc",FCVAR_NONE,"Used at warmup session in pug mod"),g_szConfig[STATE_WARMUP],sizeof(g_szConfig[]));
	bind_pcvar_string(create_cvar("pug_cfg_start","start.rc",FCVAR_NONE,"Executed when vote session starts"),g_szConfig[STATE_START],sizeof(g_szConfig[]));
	bind_pcvar_string(create_cvar("pug_cfg_1st","esl.rc",FCVAR_NONE,"Used when the match begin (Live config)"),g_szConfig[STATE_FIRST_HALF],sizeof(g_szConfig[]));
	bind_pcvar_string(create_cvar("pug_cfg_halftime","halftime.rc",FCVAR_NONE,"Used at half-time session"),g_szConfig[STATE_HALFTIME],sizeof(g_szConfig[]));
	bind_pcvar_string(create_cvar("pug_cfg_2nd","esl.rc",FCVAR_NONE,"Used when the match begin (Live config)"),g_szConfig[STATE_SECOND_HALF],sizeof(g_szConfig[]));
	bind_pcvar_string(create_cvar("pug_cfg_overtime","esl-ot.rc",FCVAR_NONE,"Used at Overtime session"),g_szConfig[STATE_OVERTIME],sizeof(g_szConfig[]));
	bind_pcvar_string(create_cvar("pug_cfg_end","end.rc",FCVAR_NONE,"Executed when the match ends"),g_szConfig[STATE_END],sizeof(g_szConfig[]));
}

public plugin_cfg()
{
	PUG_ExecConfig(STATE_DEAD);
}

public PUG_Event(iState)
{
	PUG_ExecConfig(iState);
}

PUG_ExecConfig(iState)
{
	new szPath[PLATFORM_MAX_PATH];
	PUG_GetFilePath(g_szConfig[iState],szPath,charsmax(szPath));
	
	server_cmd("exec %s",szPath);
}
