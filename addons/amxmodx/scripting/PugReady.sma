#include <amxmodx>
#include <amxmisc>
#include <cstrike>
#include <fakemeta>

#include <PugCore>
#include <PugStocks>
#include <PugCS>

#define TASK_HUDLIST 1337

new bool:g_ReadySystem = false;
new bool:g_Ready[MAX_PLAYERS+1];
new g_ReadyTime[MAX_PLAYERS+1];

new g_PlayersMin;
new g_PlayersReadyTime;

public plugin_init()
{
	register_plugin("Pug Mod (Ready System)",PUG_VERSION,PUG_AUTHOR);
	
	register_dictionary("common.txt");
	register_dictionary("PugReady.txt");
	
	g_PlayersMin = get_cvar_pointer("pug_players_min");
	
	g_PlayersReadyTime = create_cvar("pug_ready_time","0",FCVAR_NONE,"Seconds to put a player in ready state (0 to disable)");
	
	PugRegCommand("ready","Ready",ADMIN_ALL,"PUG_DESC_READY");
	PugRegCommand("notready","NotReady",ADMIN_ALL,"PUG_DESC_NOTREADY");
	
	PugRegCommand("forceready","ForceReady",ADMIN_LEVEL_A,"PUG_DESC_FORCEREADY");
	
	register_clcmd("joinclass","JoinClass");
	register_clcmd("menuselect","JoinClass");
}

public client_putinserver(id)
{
	g_Ready[id] = false;
	g_ReadyTime[id] = 0;
}

public JoinClass(id)
{
	if(g_ReadySystem)
	{
		if(get_ent_data(id,"CBasePlayer","m_iMenu") == CS_Menu_ChooseAppearance)
		{
			if(get_pcvar_num(g_PlayersReadyTime))
			{
				g_ReadyTime[id] = get_systime();
			}
		}
	}
}

public PugEvent(State)
{
	switch(State)
	{
		case STATE_WARMUP:
		{
			ReadySystem(true);
		}
		case STATE_HALFTIME:
		{
			if(PugGetPlayersNum(false) < get_pcvar_num(g_PlayersMin))
			{
				ReadySystem(true);
			}
		}
		default:
		{
			ReadySystem(false);
		}
	}
}

ReadySystem(bool:Enable)
{
	arrayset(g_Ready,false,sizeof(g_Ready));
	arrayset(g_ReadyTime,0,sizeof(g_ReadyTime));
	
	if(Enable)
	{
		g_ReadySystem = true;
		
		if(get_pcvar_num(g_PlayersReadyTime))
		{
			arrayset(g_ReadyTime,get_systime(),sizeof(g_ReadyTime));
		}
		
		set_task(0.5,"HudList",TASK_HUDLIST, .flags="b");
		
		PugMsg(0,"PUG_READY_START");
	}
	else
	{
		g_ReadySystem = false;
		remove_task(TASK_HUDLIST);
	}
}

public Ready(id)
{
	if(g_ReadySystem)
	{
		if(!g_Ready[id])
		{
			if(JoinedTeam(id))
			{
				g_Ready[id] = true;
				g_ReadyTime[id] = 0;
				
				new Name[MAX_NAME_LENGTH];
				get_user_name(id,Name,charsmax(Name));
				
				client_print_color(0,print_team_red,"%s %L",PUG_HEADER,LANG_SERVER,"PUG_READY",Name);
				
				return CheckReady();
			}
		}
	}

	PugMsg(id,"PUG_CMD_ERROR");
	
	return PLUGIN_HANDLED;
}

public NotReady(id)
{
	if(g_ReadySystem)
	{
		if(g_Ready[id])
		{
			if(JoinedTeam(id))
			{
				g_Ready[id] = false;
				
				if(get_pcvar_num(g_PlayersReadyTime))
				{
					g_ReadyTime[id] = get_systime();
				}
				
				new Name[MAX_NAME_LENGTH];
				get_user_name(id,Name,charsmax(Name));
				
				client_print_color(0,print_team_red,"%s %L",PUG_HEADER,LANG_SERVER,"PUG_NOTREADY",Name);
				
				return PLUGIN_HANDLED;
			}
		}
	}
	
	PugMsg(id,"PUG_CMD_ERROR");
	
	return PLUGIN_HANDLED;
}

public ForceReady(id,Level)
{
	if(access(id,Level))
	{
		new Arg[MAX_NAME_LENGTH];
		read_argv(1,Arg,charsmax(Arg));
		
		new Player = cmd_target(id,Arg,CMDTARGET_NO_BOTS|CMDTARGET_OBEY_IMMUNITY);
		
		if(!Player)
		{
			return PLUGIN_HANDLED;
		}
		
		PugCommandClient(id,"!forceready","PUG_FORCE_READY",Player,Ready(Player));
	}
	
	return PLUGIN_HANDLED;
}

CheckReady()
{
	new Count;
	
	for(new i;i < sizeof(g_Ready);i++)
	{
		if(g_Ready[i])
		{
			Count++;
		}
	}
	
	if(Count >= get_pcvar_num(g_PlayersMin))
	{
		ReadySystem(false);
		PugMsg(0,"PUG_ALL_READY");
		
		PugNext();
	}
	
	return PLUGIN_HANDLED;
}

public HudList()
{	
	new List[2][512];
	
	new Time[16];
	new Name[MAX_NAME_LENGTH + sizeof(Time)];
	
	new Readys,PlayersCount;
	
	new Players[MAX_PLAYERS],Num,Player;
	get_players(Players,Num,"h");
	
	for(new i;i < Num;i++)
	{
		Player = Players[i];
		
		if(JoinedTeam(Player))
		{
			PlayersCount++;
			get_user_name(Player,Name,charsmax(Name));
			
			if(g_Ready[Player])
			{
				Readys++;
				formatex(List[0],charsmax(List[]),"%s%s^n",List[0],Name);
			}
			else
			{
				if(g_ReadyTime[Player] > 0)
				{
					new Remain = get_pcvar_num(g_PlayersReadyTime) - (get_systime() - g_ReadyTime[Player]);
					
					if(Remain > 0)
					{
						format_time(Time,charsmax(Time),"%M:%S",Remain);
						
						format(Name,charsmax(Name),"%s %s",Name,Time);
					}
					else
					{
						Ready(Player);
					}
				}
				
				formatex(List[1],charsmax(List[]),"%s%s^n",List[1],Name);
			}
		}
	}
	
	new MinPlayers = get_pcvar_num(g_PlayersMin);
	
	set_hudmessage(0,255,0,0.23,0.02,0,0.0,0.6,0.0,0.0,1);
	show_hudmessage(0,"%L",LANG_SERVER,"PUG_LIST_NOTREADY",(PlayersCount - Readys),MinPlayers);

	set_hudmessage(0,255,0,0.58,0.02,0,0.0,0.6,0.0,0.0,2);
	show_hudmessage(0,"%L",LANG_SERVER,"PUG_LIST_READY",Readys,MinPlayers);
	
	set_hudmessage(255,255,225,0.58,0.02,0,0.0,0.6,0.0,0.0,3);
	show_hudmessage(0,"^n%s",List[0]);

	set_hudmessage(255,255,225,0.23,0.02,0,0.0,0.6,0.0,0.0,4);
	show_hudmessage(0,"^n%s",List[1]);
}
