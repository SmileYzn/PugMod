#include <PugCore>
#include <PugMenus>
#include <PugStocks>
#include <PugCS>

new g_iEvent;
new g_iState;

new g_iPlayersMin;
new g_iPlayersMax;

new g_iPlayRounds;
new g_iPlayOvertimeRounds;
new g_iPlayOvertime;

new g_iPlayBestRounds;

new g_iAllowSpec;
new g_iAllowHLTV;

new Float:g_fHandleTime;
new g_iDropBanTime;

new g_iForceJoinTeam;
new g_iBlockPickTeam;

new g_szStates[PUG_MOD_STATES][MAX_NAME_LENGTH] =
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

new g_iRound[PUG_MOD_STATES];
new g_iScore[TeamName][PUG_MOD_STATES];

new g_iRoundReset;

new HookChain:g_hCBasePlayer_GetIntoGame;
new HookChain:g_hCSGameRules_OnRoundFreezeEnd;
new HookChain:g_hRoundEnd;

public plugin_init()
{
	register_plugin(PUG_MOD_PLUGIN,PUG_MOD_VERSION,PUG_MOD_AUTHOR);
	
	register_dictionary("common.txt");
	register_dictionary("PugCore.txt");
	
	g_iEvent = CreateMultiForward("PUG_Event",ET_IGNORE,FP_CELL,FP_STRING);
	
	bind_pcvar_num(create_cvar("pug_players_min","10",FCVAR_NONE,"Minimum of players to start a game"),g_iPlayersMin);
	bind_pcvar_num(create_cvar("pug_players_max","10",FCVAR_NONE,"Maximum of players allowed in game"),g_iPlayersMax);
	
	bind_pcvar_num(create_cvar("pug_play_rounds","30",FCVAR_NONE,"Rounds to play before start overtime"),g_iPlayRounds);
	bind_pcvar_num(create_cvar("pug_play_overtime_rounds","3",FCVAR_NONE,"Win difference to determine a winner in overtime"),g_iPlayOvertimeRounds);
	bind_pcvar_num(create_cvar("pug_play_overtime","1",FCVAR_NONE,"Play Overtime (0 Sudden Death, 1 Force Overtime, 2 End Tied)"),g_iPlayOvertime);
	
	bind_pcvar_num(create_cvar("pug_play_best_rounds","0",FCVAR_NONE,"Play Best of X rounds (0 disable, or round count to enable)"),g_iPlayBestRounds);
	
	bind_pcvar_num(create_cvar("pug_allow_spec","1",FCVAR_NONE,"Allow Spectators in game"),g_iAllowSpec);
	bind_pcvar_num(create_cvar("pug_allow_hltv","1",FCVAR_NONE,"Allow HLTV in game"),g_iAllowHLTV);
	
	bind_pcvar_float(create_cvar("pug_handle_time","10.0",FCVAR_NONE,"Time to PUG change states"),g_fHandleTime);
	
	bind_pcvar_num(create_cvar("pug_drop_ban_time","15",FCVAR_NONE,"Minutes of ban players that leave from game in live"),g_iDropBanTime);
	
	bind_pcvar_num(create_cvar("pug_force_join_team","1",FCVAR_NONE,"Force players join when connect to server"),g_iForceJoinTeam);
	bind_pcvar_num(create_cvar("pug_block_pick_team","1",FCVAR_NONE,"Block players from choosing teams"),g_iBlockPickTeam);
	
	register_clcmd("say","HOOK_SayText");
	register_clcmd("say_team","HOOK_SayText");
	
	register_clcmd("jointeam","HOOK_ChangeTeam");
	register_clcmd("chooseteam","HOOK_ChangeTeam");
	
	PUG_RegCommand("status","PUG_Status",ADMIN_ALL,"PUG_DESC_STATUS");
	PUG_RegCommand("score","PUG_ShowScore",ADMIN_ALL,"PUG_DESC_SCORE");
	
	PUG_RegCommand("startpug","PUG_StartPug",ADMIN_LEVEL_A,"PUG_DESC_START");
	PUG_RegCommand("stoppug","PUG_StopPug",ADMIN_LEVEL_A,"PUG_DESC_STOP");
	
	PUG_RegCommand("help","PUG_Help",ADMIN_ALL,"PUG_DESC_HELP");
	PUG_RegCommand("help","PUG_HelpAdmin",ADMIN_LEVEL_A,"PUG_DESC_HELP_ADMIN");
	
	DisableHookChain(g_hCBasePlayer_GetIntoGame = RegisterHookChain(RG_CBasePlayer_GetIntoGame,"HOOK_CBasePlayer_GetIntoGame",true));
	
	DisableHookChain(g_hCSGameRules_OnRoundFreezeEnd = RegisterHookChain(RG_CSGameRules_OnRoundFreezeEnd,"HOOK_CSGameRules_OnRoundFreezeEnd",true));
	
	DisableHookChain(g_hRoundEnd = RegisterHookChain(RG_RoundEnd,"HOOK_RoundEnd",true));
}

public plugin_natives()
{
	register_library("PugCore");
	
	register_native("PUG_RunState","PUG_Next");
	
	register_native("PUG_GetState","THIS_GetState");
	register_native("PUG_GetScore","THIS_GetScore");
	register_native("PUG_GetRound","THIS_GetRound");
	register_native("PUG_GetReset","THIS_GetReset");
	register_native("PUG_SetEnded","THIS_SetEnded");
}

public OnConfigsExecuted()
{
	PUG_BuildHelpFile(ADMIN_ALL);
	PUG_BuildHelpFile(ADMIN_LEVEL_A);
	
	PUG_BuildCvarsFile("pugmod.rc",false);
	
	formatex(g_szStates[STATE_DEAD],charsmax(g_szStates[]),"%L",LANG_SERVER,"PUG_STATE_DEAD");
	formatex(g_szStates[STATE_WARMUP],charsmax(g_szStates[]),"%L",LANG_SERVER,"PUG_STATE_WARMUP");
	formatex(g_szStates[STATE_START],charsmax(g_szStates[]),"%L",LANG_SERVER,"PUG_STATE_START");
	formatex(g_szStates[STATE_FIRST_HALF],charsmax(g_szStates[]),"%L",LANG_SERVER,"PUG_STATE_FIRST_HALF");
	formatex(g_szStates[STATE_HALFTIME],charsmax(g_szStates[]),"%L",LANG_SERVER,"PUG_STATE_HALFTIME");
	formatex(g_szStates[STATE_SECOND_HALF],charsmax(g_szStates[]),"%L",LANG_SERVER,"PUG_STATE_SECOND_HALF");
	formatex(g_szStates[STATE_OVERTIME],charsmax(g_szStates[]),"%L",LANG_SERVER,"PUG_STATE_OVERTIME");
	formatex(g_szStates[STATE_END],charsmax(g_szStates[]),"%L",LANG_SERVER,"PUG_STATE_END");

	PUG_Next();
}

public plugin_end()
{
	if(STATE_FIRST_HALF <= g_iState <= STATE_OVERTIME)
	{
		g_iState = STATE_END;
		
		ExecuteForward(g_iEvent,_,g_iState,g_szStates[g_iState]);
	}
}

public client_putinserver(id)
{
	if(g_iForceJoinTeam && !is_user_hltv(id))
	{
		RequestFrame("PUG_ForceTeamJoin",id);
	}
}

public PUG_ForceTeamJoin(id)
{
	if(is_user_connected(id))
	{
		new TeamName:iTeam = TEAM_SPECTATOR;
		
		if((g_iState != STATE_START) && (PUG_GetPlayersNum() < g_iPlayersMax))
		{
			iTeam = rg_get_join_team_priority();
		}
		
		rg_join_team(id,iTeam);
	}
}

public HOOK_SayText(id)
{
	new szArgs[192];
	read_args(szArgs,charsmax(szArgs));
	remove_quotes(szArgs);
	
	if((szArgs[0] == '.') || (szArgs[0] == '!'))
	{
		client_cmd(id,szArgs);
		return PLUGIN_HANDLED;
	}
	
	return PLUGIN_CONTINUE;
}

public HOOK_ChangeTeam(id)
{
	if(g_iBlockPickTeam)
	{
		client_print_color(0,print_team_red,"%s %L",PUG_MOD_HEADER,LANG_SERVER,"PUG_TEAM_CHANGE");
		return PLUGIN_HANDLED;
	}
	
	return PLUGIN_CONTINUE;
}

public client_connectex(id,const szName[],const szIP[],szReason[128])
{
	if(!g_iAllowSpec && !is_user_hltv(id) && (PUG_GetPlayersNum() >= g_iPlayersMax))
	{
		formatex(szReason,charsmax(szReason),"%L",LANG_SERVER,"PUG_KICK_MSG_FULL");
		return PLUGIN_HANDLED;
	}
	
	if(!g_iAllowHLTV && is_user_hltv(id))
	{
		formatex(szReason,charsmax(szReason),"%L",LANG_SERVER,"PUG_KICK_MSG_HLTV");
		return PLUGIN_HANDLED;
	}
	
	return PLUGIN_CONTINUE;
}

public HOOK_CBasePlayer_GetIntoGame(id)
{
	client_print_color(id,id,"%s %L",PUG_MOD_HEADER,LANG_SERVER,"PUG_BUILD_STRING1",PUG_MOD_PLUGIN,PUG_MOD_VERSION,PUG_MOD_AUTHOR);
	client_print_color(id,id,"%s %L",PUG_MOD_HEADER,LANG_SERVER,"PUG_BUILD_STRING2");
}

public client_disconnected(id,bool:bDropped,szReason[],iLen)
{
	if(bDropped && (STATE_FIRST_HALF <= g_iState <= STATE_OVERTIME) && !is_user_hltv(id))
	{
		if(get_playersnum() < g_iPlayersMin / 2)
		{
			g_iState = STATE_END;
			
			ExecuteForward(g_iEvent,_,g_iState,g_szStates[g_iState]);
		}
		else
		{
			if(1 <= get_user_team(id) <= 2)
			{
				if(g_iDropBanTime)
				{
					if(equali(szReason,"Timed out") || equali(szReason,"Client sent 'drop'"))
					{
						if(!access(id,ADMIN_LEVEL_A))
						{
							new szAuth[MAX_AUTHID_LENGTH];
							get_user_authid(id,szAuth,charsmax(szAuth));
							
							server_cmd("banid %i %s;wait;writeid",g_iDropBanTime,szAuth);
							
							client_print_color(0,print_team_red,"%s %L",PUG_MOD_HEADER,LANG_SERVER,"PUG_CLIENT_DROP_BAN",id,g_iDropBanTime,szReason);
						}
					}
				}	
			}
		}
	}
}

public PUG_Next()
{
	switch(g_iState)
	{
		case STATE_DEAD:
		{			
			g_iState = STATE_WARMUP;
		}
		case STATE_WARMUP:
		{
			g_iState = STATE_START;
		}
		case STATE_START:
		{
			g_iState = STATE_FIRST_HALF;
		}
		case STATE_FIRST_HALF:
		{
			g_iState = STATE_HALFTIME;
		}
		case STATE_HALFTIME:
		{
			g_iState = (PUG_GetTotalScore(TEAM_TERRORIST) + PUG_GetTotalScore(TEAM_CT) < g_iPlayRounds) ? STATE_SECOND_HALF : STATE_OVERTIME;
		}
		case STATE_SECOND_HALF:
		{
			if(PUG_GetTotalScore(TEAM_TERRORIST) + PUG_GetTotalScore(TEAM_CT) == g_iPlayRounds)
			{
				if(g_iPlayOvertime == 1)
				{
					g_iState = STATE_HALFTIME;
				}
				else if(g_iPlayOvertime == 2)
				{
					g_iState = STATE_END;
				}
			}
			else
			{
				g_iState = STATE_END;
			}
		}
		case STATE_OVERTIME:
		{
			if((g_iRound[STATE_OVERTIME] % (g_iPlayOvertimeRounds+1)) == 0)
			{
				g_iState = STATE_HALFTIME;
			}
			else
			{
				g_iState = STATE_END;
			}
		}
		case STATE_END:
		{
			g_iState = STATE_WARMUP;
		}
	}
	
	ExecuteForward(g_iEvent,_,g_iState,g_szStates[g_iState]);
}

public THIS_GetState(iPlugin,iParams)
{
	if(iParams == 2)
	{
		set_string(1,g_szStates[g_iState],get_param(2)); 
	}
	
	return g_iState;
}

public THIS_GetScore(iPlugin,iParams)
{
	if(iParams == 1)
	{
		new TeamName:iTeam = TeamName:get_param(1);
		
		if(TEAM_TERRORIST <= iTeam <= TEAM_CT)
		{
			return PUG_GetTotalScore(iTeam);
		}
	}
	
	return 0;
}

PUG_GetTotalScore(TeamName:iTeam)
{
	return (g_iScore[iTeam][STATE_FIRST_HALF]+g_iScore[iTeam][STATE_SECOND_HALF]+g_iScore[iTeam][STATE_OVERTIME]);
}

public THIS_GetRound()
{
	return (g_iRound[STATE_FIRST_HALF]+g_iRound[STATE_SECOND_HALF]+g_iRound[STATE_OVERTIME]);
}

public THIS_GetReset()
{
	return g_iRoundReset;
}

public THIS_SetEnded(iPlugin,iParams)
{
	if(iParams == 1)
	{
		new TeamName:iWinner = TeamName:get_param(1);
		
		if(TEAM_TERRORIST <= iWinner <= TEAM_CT)
		{
			new TeamName:iLosers = (iWinner == TEAM_TERRORIST) ? TEAM_CT : TEAM_TERRORIST;
			
			g_iRound[STATE_FIRST_HALF]  = (g_iPlayRounds/2);
			g_iRound[STATE_SECOND_HALF] = 1;
			g_iRound[STATE_OVERTIME]    = 0;
			
			g_iScore[iWinner][STATE_FIRST_HALF]  = g_iRound[STATE_FIRST_HALF];
			g_iScore[iWinner][STATE_SECOND_HALF] = g_iRound[STATE_SECOND_HALF];
			g_iScore[iWinner][STATE_OVERTIME]    = g_iRound[STATE_OVERTIME];
			
			g_iScore[iLosers][STATE_FIRST_HALF]  = 0;
			g_iScore[iLosers][STATE_SECOND_HALF] = 0;
			g_iScore[iLosers][STATE_OVERTIME]    = 0;
		}
	}
	
	g_iState = STATE_END;
	
	ExecuteForward(g_iEvent,_,g_iState,g_szStates[g_iState]);
}

public PUG_Event(iState)
{
	switch(iState)
	{
		case STATE_WARMUP:
		{
			EnableHookChain(g_hCBasePlayer_GetIntoGame);
		
			client_print_color(0,print_team_red,"%s %L",PUG_MOD_HEADER,LANG_SERVER,"PUG_WARMUP_MSG");
		}
		case STATE_FIRST_HALF:
		{
			PUG_LO3(1);
			
			g_iRoundReset = g_iPlayBestRounds;
			
			arrayset(g_iRound,0,sizeof(g_iRound));
			
			arrayset(g_iScore[TEAM_CT],0,sizeof(g_iScore[]));
			
			arrayset(g_iScore[TEAM_TERRORIST],0,sizeof(g_iScore[]));
			
			client_print_color(0,print_team_red,"%s %L",PUG_MOD_HEADER,LANG_SERVER,"PUG_LIVE_1ST");
		}
		case STATE_HALFTIME:
		{
			client_print_color(0,print_team_red,"%s %L",PUG_MOD_HEADER,LANG_SERVER,"PUG_HALFTIME");
			
			set_task(g_fHandleTime,"PUG_SwapTeams");
		}
		case STATE_SECOND_HALF:
		{
			PUG_LO3(1);
			
			if(abs(PUG_GetTotalScore(TEAM_TERRORIST)-PUG_GetTotalScore(TEAM_CT)) < (g_iPlayRounds/3))
			{
				g_iRoundReset = g_iPlayBestRounds;
			}
			
			client_print_color(0,print_team_red,"%s %L",PUG_MOD_HEADER,LANG_SERVER,"PUG_LIVE_2ND");
		}
		case STATE_OVERTIME:
		{
			PUG_LO3(1);
			
			client_print_color(0,print_team_red,"%s %L",PUG_MOD_HEADER,LANG_SERVER,"PUG_LIVE_OT");
		}
		case STATE_END:
		{
			PUG_ViewScore(0);
			
			set_task(g_fHandleTime,"PUG_Next");
		}
	}
	
	if(iState == STATE_FIRST_HALF || iState == STATE_SECOND_HALF || iState == STATE_OVERTIME)
	{
		EnableHookChain(g_hCSGameRules_OnRoundFreezeEnd);
		
		EnableHookChain(g_hRoundEnd);
	}
	else
	{
		DisableHookChain(g_hCSGameRules_OnRoundFreezeEnd);
		
		DisableHookChain(g_hRoundEnd);
	}
}

public PUG_Status(id)
{
	client_print_color
	(
		id,
		id,
		"%s %L",
		PUG_MOD_HEADER,
		LANG_SERVER,
		"PUG_STATUS",
		g_szStates[g_iState],
		get_playersnum(),
		g_iPlayersMin,
		g_iPlayersMax
	);	
	
	return PLUGIN_HANDLED;
}

public PUG_ShowScore(id)
{
	if(STATE_FIRST_HALF <= g_iState <= STATE_OVERTIME)
	{
		PUG_ViewScore(id);
	}
	else
	{
		client_print_color(id,id,"%s %L",PUG_MOD_HEADER,LANG_SERVER,"PUG_CMD_ERROR");
	}
	
	return PLUGIN_HANDLED;
}

public PUG_StartPug(id,iLevel)
{
	if(access(id,iLevel))
	{
		new bool:bCheck = (g_iState == STATE_WARMUP || g_iState == STATE_HALFTIME);
		
		PUG_ExecuteCommand(id,"PUG_START_PUG",bCheck);
		
		if(bCheck)
		{
			PUG_Next();		
		}
	}
	
	return PLUGIN_HANDLED;
}

public PUG_StopPug(id,Level)
{
	if(access(id,Level) || (id == 0))
	{
		new bool:bCheck = (STATE_FIRST_HALF <= g_iState <= STATE_OVERTIME);
		
		PUG_ExecuteCommand(id,"PUG_STOP_PUG",bCheck);
		
		if(bCheck)
		{
			g_iState = STATE_END;
			
			ExecuteForward(g_iEvent,_,g_iState,g_szStates[g_iState]);
			
		}
	}
	
	return PLUGIN_HANDLED;
}

public PUG_Help(id)
{
	new szPath[64];
	PUG_GetFilePath("help.htm",szPath,charsmax(szPath));
	
	new szTitle[MAX_NAME_LENGTH];
	format(szTitle,charsmax(szTitle),"%L",LANG_SERVER,"PUG_HELP_TITLE");

	show_motd(id,szPath,szTitle);
	
	return PLUGIN_HANDLED;
}

public PUG_HelpAdmin(id,Level)
{
	if(access(id,Level))
	{
		new szPath[64];
		PUG_GetFilePath("admin.htm",szPath,charsmax(szPath));
		
		new szTitle[MAX_NAME_LENGTH];
		format(szTitle,charsmax(szTitle),"%L",LANG_SERVER,"PUG_HELP_TITLE_ADMIN");
	
		show_motd(id,szPath,szTitle);
	}
	
	return PLUGIN_HANDLED;
}

public PUG_LO3(iDelay)
{
	if(1 <= iDelay <= 3)
	{
		set_task(float(iDelay+1),"PUG_LO3",iDelay+1);
		
		set_cvar_num("sv_restart",iDelay);
	}
	else
	{
		set_hudmessage(0,255,0,-1.0,0.3,0,6.0,6.0);
		
		if(!g_iRoundReset)
		{
			show_hudmessage(0,"%L",LANG_SERVER,"PUG_LIVE_HUD_1");
		}
		else
		{
			show_hudmessage(0,"%L",LANG_SERVER,"PUG_LIVE_HUD_2",g_iRoundReset);
		}
	}
}

PUG_ViewScore(id)
{
	new TeamName:iWinner = PUG_GetWinner();
	
	if(iWinner)
	{
		client_print_color
		(
			id,
			(iWinner == TEAM_TERRORIST) ? print_team_red : print_team_blue,
			"%s %L",
			PUG_MOD_HEADER,
			LANG_SERVER,
			(g_iState == STATE_END) ? "PUG_END_WONALL" : "PUG_SCORE",
			PUG_MOD_CS_TEAMS_STR[iWinner],
			PUG_GetTotalScore(iWinner),
			PUG_GetTotalScore(iWinner == TEAM_TERRORIST ? TEAM_CT : TEAM_TERRORIST)
		);
	}
	else
	{
		client_print_color
		(
			id,
			id,
			"%s %L",
			PUG_MOD_HEADER,
			LANG_SERVER,
			(g_iState == STATE_END) ? "PUG_END_TIED" : "PUG_SCORE_TIED",
			PUG_GetTotalScore(TEAM_TERRORIST),
			PUG_GetTotalScore(TEAM_CT)
		);
	}
}

TeamName:PUG_GetWinner()
{
	if(PUG_GetTotalScore(TEAM_TERRORIST) != PUG_GetTotalScore(TEAM_CT))
	{
		return (PUG_GetTotalScore(TEAM_TERRORIST) > PUG_GetTotalScore(TEAM_CT)) ? TEAM_TERRORIST : TEAM_CT;
	}
	
	return TEAM_UNASSIGNED;
}

PUG_ResetRound()
{
	if(g_iRoundReset)
	{
		if(g_iRound[g_iState] > (g_iRoundReset/2))
		{
			g_iRoundReset = 0;
			
			g_iRound[g_iState] = 1;
			
			new TeamName:iWinner = PUG_GetWinner();
			
			if(iWinner == TEAM_TERRORIST || iWinner == TEAM_CT)
			{
				g_iScore[iWinner][g_iState] = 1;
				
				g_iScore[iWinner == TEAM_TERRORIST ? TEAM_CT : TEAM_TERRORIST][g_iState] = 0;
			}
		}
		
		PUG_LO3(1);
		
		return 1;
	}
	
	return 0;
}

public PUG_SwapTeams()
{
	new iTemp;
	
	for(new iState = STATE_FIRST_HALF;iState <= STATE_OVERTIME;iState++)
	{
		iTemp = g_iScore[TEAM_TERRORIST][iState];
		
		g_iScore[TEAM_TERRORIST][iState] = g_iScore[TEAM_CT][iState];
		
		g_iScore[TEAM_CT][iState] = iTemp;
	}
	
	rg_swap_all_players();
	
	rg_restart_round();
	
	client_print_color(0,print_team_red,"%s %L",PUG_MOD_HEADER,LANG_SERVER,"PUG_SWAP_TEAMS");
	
	if((g_iState == STATE_HALFTIME) && (PUG_GetPlayersNum(true) >= g_iPlayersMin))
	{
		PUG_Next();
	}
}

bool:PUG_CheckRound()
{
	if(g_iState == STATE_FIRST_HALF)
	{
		if(!PUG_ResetRound())
		{
			if(g_iRound[g_iState] == (g_iPlayRounds / 2))
			{
				return true;
			}
		}
	}
	else if(g_iState == STATE_SECOND_HALF)
	{
		if(!PUG_ResetRound())
		{
			new iScoreTR = PUG_GetTotalScore(TEAM_TERRORIST);
			new iScoreCT = PUG_GetTotalScore(TEAM_CT);
			
			if((iScoreTR > (g_iPlayRounds / 2)) || (iScoreCT > (g_iPlayRounds / 2)))
			{
				return true;
			}
			else if(g_iPlayOvertime && (iScoreTR == (g_iPlayRounds / 2)) && (iScoreCT == (g_iPlayRounds / 2)))
			{
				return true;
			}	
		}
	}
	else if(g_iState == STATE_OVERTIME)
	{
		if((THIS_GetRound() % g_iPlayOvertimeRounds) == 0)
		{
			return true;
		}
		else if((g_iScore[TEAM_TERRORIST][STATE_OVERTIME] - g_iScore[TEAM_CT][STATE_OVERTIME]) > g_iPlayOvertimeRounds)
		{
			return true;
		}
		else if((g_iScore[TEAM_CT][STATE_OVERTIME] - g_iScore[TEAM_TERRORIST][STATE_OVERTIME]) > g_iPlayOvertimeRounds)
		{
			return true;
		}
	}
	
	return false;
}

public HOOK_CSGameRules_OnRoundFreezeEnd()
{
	if(THIS_GetRound())
	{
		PUG_ViewScore(0);
		
		client_print(0,print_console,"%s %L",PUG_MOD_HEADER,LANG_SERVER,"PUG_ROUND_START",THIS_GetRound());
	}
}

public HOOK_RoundEnd(WinStatus:Status,ScenarioEventEndRound:Event,Float:tmDelay)
{
	if(Status != WINSTATUS_NONE)
	{
		if(Status == WINSTATUS_TERRORISTS)
		{
			g_iRound[g_iState]++;
			
			g_iScore[TEAM_TERRORIST][g_iState]++;
			
			client_print(0,print_console,"%L",LANG_SERVER,"PUG_ROUND_WON",THIS_GetRound(),PUG_MOD_CS_TEAMS_STR[TEAM_TERRORIST]);
			
			if(g_iRoundReset)
			{
				client_print_color(0,print_team_red,"%s %L",PUG_MOD_HEADER,LANG_SERVER,"PUG_ROUND_WON",g_iRound[g_iState],PUG_MOD_CS_TEAMS_STR[TEAM_TERRORIST]);
			}
		}
		else if(Status == WINSTATUS_CTS)
		{
			g_iRound[g_iState]++;
			
			g_iScore[TEAM_CT][g_iState]++;
			
			client_print(0,print_console,"%L",LANG_SERVER,"PUG_ROUND_WON",THIS_GetRound(),PUG_MOD_CS_TEAMS_STR[TEAM_CT]);
			
			if(g_iRoundReset)
			{
				client_print_color(0,print_team_blue,"%s %L",PUG_MOD_HEADER,LANG_SERVER,"PUG_ROUND_WON",g_iRound[g_iState],PUG_MOD_CS_TEAMS_STR[TEAM_CT]);
			}
		}
		else if(Status == WINSTATUS_DRAW)
		{
			client_print(0,print_console,"%L",LANG_SERVER,"PUG_ROUND_DRAW",THIS_GetRound());
			
			if(g_iRoundReset)
			{
				client_print_color(0,print_team_default,"%s %L",PUG_MOD_HEADER,LANG_SERVER,"PUG_ROUND_DRAW",g_iRound[g_iState]);
			}
		}
		
		if(PUG_CheckRound())
		{
			set_task(tmDelay,"PUG_Next");
		}
	}
}
