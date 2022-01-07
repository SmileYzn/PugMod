#include <PugCore>
#include <PugCS>
#include <PugStocks>

#define TASK_READY_SYSTEM 3000

new g_iPlayersMin;

new g_iReadyType;
new Float:g_fReadyTimer;

new g_iSystemTime;

new bool:g_bReady[MAX_PLAYERS+1];
new bool:g_bReadySystem;

public plugin_init()
{
	register_plugin("Pug Mod (Ready System)",PUG_MOD_VERSION,PUG_MOD_AUTHOR,PUG_MOD_LINK,"Ready System And Match Controller");
	
	register_dictionary("PugReady.txt");
	
	bind_pcvar_num(get_cvar_pointer("pug_players_min"),g_iPlayersMin);
	
	bind_pcvar_num(create_cvar("pug_ready_type","1",FCVAR_NONE,"Ready system type (1 Ready System, 0 Timer Counter)"),g_iReadyType);
	
	bind_pcvar_float(create_cvar("pug_ready_timer","60.0",FCVAR_NONE,"Time limit to start match"),g_fReadyTimer);
	
	PUG_RegCommand("ready","PUG_Ready",ADMIN_ALL,"PUG_DESC_READY");
	PUG_RegCommand("notready","PUG_NotReady",ADMIN_ALL,"PUG_DESC_NOTREADY");
	
	PUG_RegCommand("forceready","PUG_ForceReady",ADMIN_LEVEL_A,"PUG_DESC_FORCEREADY");
}

public client_putinserver(id)
{
	g_bReady[id] = false;
}

public PUG_Event(iState)
{
	if(iState == STATE_WARMUP || (iState == STATE_HALFTIME && (PUG_GetPlayersNum(true) < g_iPlayersMin)))
	{
		g_iSystemTime = get_systime();

		if(g_iReadyType)
		{
			g_bReadySystem = true;
			
			arrayset(g_bReady,false,sizeof(g_bReady));
			
			client_print_color(0,print_team_red,"%s %L",PUG_MOD_HEADER,LANG_SERVER,"PUG_READY_START");
		}
		
		set_task(0.5,"PUG_ReadyList",TASK_READY_SYSTEM, .flags="b");
	}
	else
	{
		remove_task(TASK_READY_SYSTEM);
	}
}

public PUG_ReadyList()
{	
	if(g_iReadyType)
	{
		new szList[2][400];

		new iReadyNum,iPlayersNum;
		
		new iPlayers[MAX_PLAYERS],iNum;
		get_players(iPlayers,iNum,"h");
		
		new iPlayer;
		
		for(new i;i < iNum;i++)
		{
			iPlayer = iPlayers[i];
			
			if(PUG_CHECK_TEAM(iPlayer))
			{
				iPlayersNum++;

				if(g_bReady[iPlayer])
				{
					iReadyNum++;
					
					format(szList[0],charsmax(szList[]),"%s%n^n",szList[0],iPlayer);
				}
				else
				{
					format(szList[1],charsmax(szList[]),"%s%n^n",szList[1],iPlayer);
				}
			}
		}
		
		if(iReadyNum < g_iPlayersMin)
		{
			set_hudmessage(0,255,0,0.23,0.02,0,0.0,0.53,0.0,0.0,1);
			show_hudmessage(0,"%L",LANG_SERVER,"PUG_LIST_NOTREADY",(iPlayersNum - iReadyNum),g_iPlayersMin);
		
			set_hudmessage(0,255,0,0.58,0.02,0,0.0,0.53,0.0,0.0,2);
			show_hudmessage(0,"%L",LANG_SERVER,"PUG_LIST_READY",iReadyNum,g_iPlayersMin);
			
			set_hudmessage(255,255,225,0.58,0.02,0,0.0,0.53,0.0,0.0,3);
			show_hudmessage(0,"^n%s",szList[0]);
		
			set_hudmessage(255,255,225,0.23,0.02,0,0.0,0.53,0.0,0.0,4);
			show_hudmessage(0,"^n%s",szList[1]);	
		}
		else
		{
			client_print_color(0,print_team_red,"%s %L",PUG_MOD_HEADER,LANG_SERVER,"PUG_ALL_READY");
			
			PUG_RunState();
		}
	}
	else
	{
		set_hudmessage(0,255,0,-1.0,0.3,0,0.0,0.53,0.0,0.0,1);
		
		new iRemainPlayers = (g_iPlayersMin - PUG_GetPlayersNum(true));
		
		if(iRemainPlayers > 0)
		{
			g_iSystemTime = get_systime();
			
			show_hudmessage(0,"%L",LANG_SERVER,"PUG_WAIT_HUD_MSG_1",iRemainPlayers);
		}
		else
		{
			new iRemainTime = floatround(g_fReadyTimer - (get_systime() - g_iSystemTime));
			
			if(iRemainTime > 0)
			{
				new szTime[16];
				format_time(szTime,charsmax(szTime),"%M:%S",iRemainTime);
				
				show_hudmessage(0,"%L",LANG_SERVER,"PUG_WAIT_HUD_MSG_2",szTime);
			}
			else
			{
				PUG_RunState();
			}
		}
	}
}

public PUG_Ready(id)
{
	if(g_bReadySystem && !g_bReady[id])
	{
		if(PUG_CHECK_TEAM(id))
		{
			g_bReady[id] = true;

			client_print_color(0,id,"%s %L",PUG_MOD_HEADER,LANG_SERVER,"PUG_READY",id);	
			
			return PLUGIN_HANDLED;
		}
	}

	client_print_color(id,id,"%s %L",PUG_MOD_HEADER,LANG_SERVER,"PUG_CMD_ERROR");
	
	return PLUGIN_HANDLED;
}

public PUG_NotReady(id)
{
	if(g_bReadySystem && g_bReady[id])
	{
		if(PUG_CHECK_TEAM(id))
		{
			g_bReady[id] = false;
	
			client_print_color(0,id,"%s %L",PUG_MOD_HEADER,LANG_SERVER,"PUG_NOTREADY",id);
			
			return PLUGIN_HANDLED;
		}
	}

	client_print_color(id,id,"%s %L",PUG_MOD_HEADER,LANG_SERVER,"PUG_CMD_ERROR");
	
	return PLUGIN_HANDLED;
}

public PUG_ForceReady(id,iLevel)
{
	if(g_bReadySystem && access(id,iLevel))
	{
		new szName[MAX_NAME_LENGTH];
		read_argv(1,szName,charsmax(szName));
		
		new iPlayer = cmd_target(id,szName,CMDTARGET_OBEY_IMMUNITY);
		
		if(iPlayer)
		{
			PUG_CommandClient(id,"PUG_FORCE_READY",iPlayer,PUG_Ready(iPlayer));
		}
	}
	
	return PLUGIN_HANDLED;
}
