#include <PugCore>
#include <PugStocks>
#include <PugMenus>
#include <PugCS>

enum _:VOTE_TYPE
{
	VOTE_MAPS = 2021,
	VOTE_TEAM,
	VOTE_LEAD,
	VOTE_SWAP,
	VOTE_STOP
};

#define VOTE_LIST(%0) (%0+VOTE_STOP)

new g_iPlayersMax;
new g_iPlayersMin;

new g_iMapVote;
new g_iMapVoteType;
new g_iTeamEnforcement;

new g_iVoteCount;

new g_iMenuMap;
new g_iMenuTeams;
new g_iMenuSwap;

new g_iMapCount;
new g_szMapList[PUG_MENU_MAPS][MAX_NAME_LENGTH];
new g_iMapVotes[PUG_MENU_MAPS];

new g_szTeamTypes[PUG_MENU_TEAM][MAX_NAME_LENGTH];
new g_iTeamVotes[PUG_MENU_TEAM];

new g_iLeader[TeamName];

new g_szSwapTeamType[2][8];
new g_iSwapTeamVotes[2];

new HookChain:g_hRoundEnd;
new g_hCurWeapon;

public plugin_init()
{
	register_plugin("Pug Mod (Menus)",PUG_MOD_VERSION,PUG_MOD_AUTHOR);
	
	register_dictionary("PugMenus.txt");
	
	bind_pcvar_num(get_cvar_pointer("pug_players_max"),g_iPlayersMax);
	bind_pcvar_num(get_cvar_pointer("pug_players_min"),g_iPlayersMin);
	
	bind_pcvar_num(create_cvar("pug_vote_map_enabled","1",FCVAR_NONE,"Active vote map in pug (0 Disable, 1 Enable, 2 Random map)"),g_iMapVoteType);
	bind_pcvar_num(create_cvar("pug_teams_enforcement","-1",FCVAR_NONE,"The teams method for assign teams (-1 Vote, 0 Leaders, 1 Random, 2 None, 3 Skill Balanced, 4 Swap Teams, 5 Knife Round)"),g_iTeamEnforcement);
	
	DisableHookChain(g_hRoundEnd = RegisterHookChain(RG_RoundEnd,"HOOK_RoundEnd",true));
	
	disable_event(g_hCurWeapon = register_event("CurWeapon","HOOK_CurWeapon","be","1=1","2!29"));
}

public OnConfigsExecuted()
{
	g_iMenuMap  = menu_create("PUG_HUD_MAP","HANDLER_MenuVote",true);
	g_iMapCount = PUG_GetMapList(g_szMapList,sizeof(g_szMapList[]));

	for(new i = 0;i < g_iMapCount;i++)
	{
		menu_additem(g_iMenuMap,g_szMapList[i],fmt("%i",i));
	}
	
	menu_setprop(g_iMenuMap,MPROP_EXIT,MEXIT_NEVER);
	
	g_iMenuTeams = menu_create("PUG_HUD_TEAM","HANDLER_MenuVote",true);
	
	for(new i = 0;i < PUG_MENU_TEAM;i++)
	{
		formatex(g_szTeamTypes[i],charsmax(g_szTeamTypes[]),"%L",LANG_SERVER,fmt("PUG_TEAM_TYPE_%i",i));
		
		menu_additem(g_iMenuTeams,g_szTeamTypes[i],fmt("%i",i));
	}
	
	menu_setprop(g_iMenuTeams,MPROP_EXIT,MEXIT_NEVER);
	
	g_iMenuSwap = menu_create("PUG_HUD_SWAP","HANDLER_MenuVote",true);
	
	formatex(g_szSwapTeamType[0],sizeof(g_szSwapTeamType[]),"%L",LANG_SERVER,"YES");
	formatex(g_szSwapTeamType[1],sizeof(g_szSwapTeamType[]),"%L",LANG_SERVER,"NO");
	
	menu_additem(g_iMenuSwap,g_szSwapTeamType[0],"0");
	menu_additem(g_iMenuSwap,g_szSwapTeamType[1],"1");
	
	menu_setprop(g_iMenuSwap,MPROP_EXIT,MEXIT_NEVER);
}

public PUG_Event(iState)
{
	if(iState == STATE_START)
	{
		if(g_iMapVote && g_iMapVoteType != 0)
		{
			if(g_iMapVoteType == 1)
			{
				PUG_VoteStart(VOTE_MAPS);
			}
			else if(g_iMapVoteType == 2)
			{
				new iMap = random(g_iMapCount);
				
				set_task(5.0,"PUG_ChangeLevel",iMap);
				
				client_print_color(0,print_team_red,"%s %L",PUG_MOD_HEADER,LANG_SERVER,"PUG_VOTEMAP_NEXTMAP",g_szMapList[iMap]);
			}
		}
		else
		{
			if(g_iTeamEnforcement == -1)
			{
				PUG_VoteStart(VOTE_TEAM);
			}
			else
			{
				set_task(1.0,"PUG_ChangeTeams",g_iTeamEnforcement);
			}
		}
	}
	else if(iState == STATE_END)
	{
		g_iMapVote = 1;
	}
}

public PUG_VoteStart(iVote)
{
	if(iVote == VOTE_MAPS)
	{
		arrayset(g_iMapVotes,0,sizeof(g_iMapVotes));
		
		PUG_DisplayMenuAll(g_iMenuMap);
		
		client_print_color(0,print_team_red,"%s %L",PUG_MOD_HEADER,LANG_SERVER,"PUG_VOTEMAP_START");
	}
	else if(iVote == VOTE_TEAM)
	{
		arrayset(g_iTeamVotes,0,sizeof(g_iTeamVotes));
		
		PUG_DisplayMenuAll(g_iMenuTeams);
		
		client_print_color(0,print_team_red,"%s %L",PUG_MOD_HEADER,LANG_SERVER,"PUG_TEAMVOTE_START");
	}
	else if(iVote == VOTE_SWAP)
	{
		new iRoundWinStatus = get_member_game(m_iRoundWinStatus);
	
		if(1 <= iRoundWinStatus <= 2)
		{
			arrayset(g_iSwapTeamVotes,0,sizeof(g_iSwapTeamVotes));
		
			PUG_DisplayMenuTeam(g_iMenuSwap,(iRoundWinStatus == 1) ? "CT" : "TERRORIST");
			
			client_print_color(0,print_team_red,"%s %L",PUG_MOD_HEADER,LANG_SERVER,"PUG_TEAMVOTE_START");
		}
		else
		{
			PUG_ChangeTeams(4);
		}
	}
	
	set_task(15.0,"PUG_VoteEnd",iVote);
	
	set_task(0.5,"PUG_VoteList",VOTE_LIST(iVote), .flags="b");
}

public HANDLER_MenuVote(id,iMenu,iKey)
{
	if(iKey != MENU_EXIT)
	{
		new szNum[3],szOption[MAX_NAME_LENGTH];
		menu_item_getinfo(iMenu,iKey,_,szNum,charsmax(szNum),szOption,charsmax(szOption));

		new iMode;
		
		if(iMenu == g_iMenuMap)
		{
			iMode = VOTE_MAPS;
			
			g_iMapVotes[str_to_num(szNum)]++;
		}
		else if(iMenu == g_iMenuTeams)
		{	
			iMode = VOTE_TEAM;
		
			g_iTeamVotes[str_to_num(szNum)]++;
		}
		else if(iMenu == g_iMenuSwap)
		{
			iMode = VOTE_SWAP;
			
			g_iSwapTeamVotes[str_to_num(szNum)]++;
		}
		
		client_print_color(0,id,"%s %L",PUG_MOD_HEADER,LANG_SERVER,"PUG_VOTE_CHOOSED",id,szOption);
		
		if(++g_iVoteCount >= PUG_GetPlayersNum())
		{
			PUG_VoteEnd(iMode);
		}
	}
	
	return PLUGIN_HANDLED;
}

public PUG_VoteList(iVote)
{
	set_hudmessage(0,255,0,0.23,0.02,0,0.0,0.7,0.0,0.0,3);
			
	new szResult[512] = {0};
	
	if(iVote == VOTE_LIST(VOTE_MAPS))
	{
		show_hudmessage(0,"%L",LANG_SERVER,"PUG_HUD_MAP");
		
		for(new i;i < g_iMapCount;i++)
		{
			if(g_iMapVotes[i])
			{
				format(szResult,charsmax(szResult),"%s[%i] %s^n",szResult,g_iMapVotes[i],g_szMapList[i]);
			}
		}
	}
	else if(iVote == VOTE_LIST(VOTE_TEAM))
	{
		show_hudmessage(0,"%L",LANG_SERVER,"PUG_HUD_TEAM");
		
		for(new i;i < PUG_MENU_TEAM;i++)
		{
			if(g_iTeamVotes[i])
			{
				format(szResult,charsmax(szResult),"%s[%i] %s^n",szResult,g_iTeamVotes[i],g_szTeamTypes[i]);
			}
		}
	}
	else if(iVote == VOTE_LIST(VOTE_SWAP))
	{
		show_hudmessage(0,"%L",LANG_SERVER,"PUG_HUD_SWAP");
		
		for(new i;i < 2;i++)
		{
			if(g_iSwapTeamVotes[i])
			{
				format(szResult,charsmax(szResult),"%s[%i] %s^n",szResult,g_iSwapTeamVotes[i],g_szSwapTeamType[i]);
			}
		}
	}
	
	set_hudmessage(255,255,255,0.23,0.05,0,0.0,0.7,0.0,0.0,4);
	
	if(szResult[0])
	{
		show_hudmessage(0,szResult);
	}
	else
	{
		show_hudmessage(0,"%L",LANG_SERVER,"PUG_NO_VOTES");
	}
}

public PUG_VoteEnd(iVote)
{
	PUG_CancelMenu(0);
	
	remove_task(iVote);
	
	remove_task(VOTE_LIST(iVote));
	
	if(!PUG_GetVoteCount(iVote))
	{
		set_task(5.0,"PUG_VoteStart",iVote);
	}
}

PUG_GetVoteCount(iVote)
{
	if(iVote == VOTE_MAPS)
	{
		new iWinner,iWinnerVotes,iVotes; // Create function to do this!
	
		for(new i;i < g_iMapCount;i++)
		{
			iVotes = g_iMapVotes[i];
			
			if(iVotes > iWinnerVotes)
			{
				iWinner = i;
				iWinnerVotes = iVotes;
			}
			else if(iVotes == iWinnerVotes)
			{
				if(random_num(0,1))
				{
					iWinner = i;
					iWinnerVotes = iVotes;
				}
			}
		}
	
		if(!g_iMapVotes[iWinner])
		{
			client_print_color(0,print_team_red,"%s %L %L",PUG_MOD_HEADER,LANG_SERVER,"PUG_VOTEMAP_FAIL",LANG_SERVER,"PUG_NO_VOTES");
			return 0;
		}
		
		set_task(5.0,"PUG_ChangeLevel",iWinner);
		
		client_print_color(0,print_team_red,"%s %L",PUG_MOD_HEADER,LANG_SERVER,"PUG_VOTEMAP_NEXTMAP",g_szMapList[iWinner]);
		
		return g_iMapVotes[iWinner];
	}
	else if(iVote == VOTE_TEAM)
	{
		new iWinner,iWinnerVotes,iVotes;
	
		for(new i;i < sizeof(g_iTeamVotes);i++)
		{
			iVotes = g_iTeamVotes[i];
			
			if(iVotes > iWinnerVotes)
			{
				iWinner = i;
				iWinnerVotes = iVotes;
			}
			else if(iVotes == iWinnerVotes)
			{
				if(random_num(0,1))
				{
					iWinner = i;
					iWinnerVotes = iVotes;
				}
			}
		}

		if(!g_iTeamVotes[iWinner])
		{
			client_print_color(0,print_team_red,"%s %L %L",PUG_MOD_HEADER,LANG_SERVER,"PUG_TEAMVOTE_FAIL",LANG_SERVER,"PUG_NO_VOTES");
			return 0;
		}
		
		set_task(3.0,"PUG_ChangeTeams",iWinner);
		
		return g_iTeamVotes[iWinner];
	}
	else if(iVote == VOTE_SWAP)
	{
		if(g_iSwapTeamVotes[0] > g_iSwapTeamVotes[1])
		{
			rg_swap_all_players();
			
			client_print_color(0,print_team_red,"%s %L",PUG_MOD_HEADER,LANG_SERVER,"PUG_TEAMS_SWAP");
		}
		else
		{
			client_print_color(0,print_team_red,"%s %L",PUG_MOD_HEADER,LANG_SERVER,"PUG_TEAMS_SAME"); 
		}
		
		PUG_RunState();
		
		return 1;
	}
	
	return 0;
}

public PUG_ChangeLevel(iMapIndex)
{
	engine_changelevel(g_szMapList[iMapIndex]);
}

public PUG_ChangeTeams(iType)
{
	switch(iType)
	{
		case 0:
		{
			PUG_TeamsLeaders();
			
			client_print_color(0,print_team_red,"%s %L",PUG_MOD_HEADER,LANG_SERVER,"PUG_TEAMS_LEADER");
		}
		case 1:
		{
			PUG_TeamsRandomize();
			
			client_print_color(0,print_team_red,"%s %L",PUG_MOD_HEADER,LANG_SERVER,"PUG_TEAMS_RANDOM");
			
			PUG_RunState();
		}
		case 2:
		{
			client_print_color(0,print_team_red,"%s %L",PUG_MOD_HEADER,LANG_SERVER,"PUG_TEAMS_SAME");
			
			PUG_RunState();
		}
		case 3:
		{
			PUG_TeamsOptimize();
			
			client_print_color(0,print_team_red,"%s %L",PUG_MOD_HEADER,LANG_SERVER,"PUG_TEAMS_SKILL");
			
			PUG_RunState();
		}
		case 4:
		{
			rg_swap_all_players();
			
			client_print_color(0,print_team_red,"%s %L",PUG_MOD_HEADER,LANG_SERVER,"PUG_TEAMS_SWAP");
			
			PUG_RunState();
		}
		case 5:
		{
			PUG_LO3(1);
			
			enable_event(g_hCurWeapon);
			
			EnableHookChain(g_hRoundEnd);
			
			client_print_color(0,print_team_red,"%s %L",PUG_MOD_HEADER,LANG_SERVER,"PUG_TEAMS_KNIFE");
		}
	}
}

PUG_TeamsLeaders()
{
	if(get_playersnum() > (g_iPlayersMin / 2))
	{
		PUG_SetLeader(find_player("ej","TERRORIST"),TEAM_TERRORIST);
		
		PUG_SetLeader(find_player("ej","CT"),TEAM_CT);
		
		new iPlayers[MAX_PLAYERS],iNum;
		get_players(iPlayers,iNum,"h");
		
		new iPlayer;
		
		for(new i;i < iNum;i++)
		{
			iPlayer = iPlayers[i];
			
			if((g_iLeader[TEAM_TERRORIST] != iPlayer) && (g_iLeader[TEAM_CT] != iPlayer))
			{
				user_silentkill(iPlayer);
				
				rg_set_user_team(iPlayer,TEAM_SPECTATOR);	
			}
		}
		
		set_task(0.5,"PUG_LeaderList",VOTE_LEAD, .flags="b");
		
		set_task(2.0,"PUG_LeaderMenu",g_iLeader[TEAM_TERRORIST]);
	}
	else
	{
		PUG_RunState();
		
		client_print_color(0,print_team_red,"%s %L %L",PUG_MOD_HEADER,LANG_SERVER,"PUG_TEAMVOTE_FAIL",LANG_SERVER,"PUG_NO_PLAYERS");
	}
}

public PUG_LeaderList()
{
	new iPlayers[MAX_PLAYERS],iNum,iPlayer;
	get_players(iPlayers,iNum,"h");
	
	new szList[TeamName][320];
	
	new TeamName:iTeam;
	
	new iCount[TeamName] = {0};
	
	for(new i;i < iNum;i++)
	{
		iPlayer = iPlayers[i];

		iTeam = get_member_s(iPlayer,m_iTeam);
		
		if(iPlayer == g_iLeader[iTeam])
		{
			format(szList[iTeam],charsmax(szList[]),"%s%n (C)^n",szList[iTeam],iPlayer);
		}
		else
		{
			format(szList[iTeam],charsmax(szList[]),"%s%n^n",szList[iTeam],iPlayer);
		}
		
		iCount[iTeam]++;
	}
	
	for(new i = 0;i < 5 - iCount[TEAM_CT];i++)
	{
		add(szList[TEAM_CT],charsmax(szList[]),"^n");
	}
	
	set_hudmessage(0,255,0,0.75,0.02,0,0.0,99.0,0.0,0.0,1);
	show_hudmessage(0,PUG_MOD_CS_TEAMS_STR[TEAM_TERRORIST]);
	
	set_hudmessage(255,255,255,0.75,0.02,0,0.0,99.0,0.0,0.0,2);
	show_hudmessage(0,"^n%s",szList[TEAM_TERRORIST]);
	
	if(iCount[TEAM_SPECTATOR])
	{
		set_hudmessage(0,255,0,0.75,0.28,0,0.0,99.0,0.0,0.0,3);
		show_hudmessage(0,"%s^n^n^n^n^n^n%s",PUG_MOD_CS_TEAMS_STR[TEAM_CT],PUG_MOD_CS_TEAMS_STR[TEAM_SPECTATOR]);
		
		set_hudmessage(255,255,255,0.75,0.28,0,0.0,99.0,0.0,0.0,4);
		show_hudmessage(0,"^n%s^n%s",szList[TEAM_CT],szList[TEAM_SPECTATOR]);
	}
	else
	{
		set_hudmessage(0,255,0,0.75,0.28,0,0.0,99.0,0.0,0.0,3);
		show_hudmessage(0,PUG_MOD_CS_TEAMS_STR[TEAM_CT]);
		
		set_hudmessage(255,255,255,0.75,0.28,0,0.0,99.0,0.0,0.0,4);
		show_hudmessage(0,"^n%s",szList[TEAM_CT]);
	}
}

public PUG_SetLeader(id,TeamName:iTeam)
{
	g_iLeader[iTeam] = id;
	
	rg_set_user_team(id,iTeam);
	
	if(!is_user_alive(id))
	{
		rg_round_respawn(id);
	}
	
	client_print_color(0,id,"%s %L",PUG_MOD_HEADER,LANG_SERVER,"PUG_LEADER_NEW",id,PUG_MOD_CS_TEAMS_STR[iTeam]);
}

public PUG_LeaderMenu(id)
{
	new iPlayers[MAX_PLAYERS],iNum;
	get_players(iPlayers,iNum,"eh","SPECTATOR");
		
	if(iNum)
	{
		if(is_user_connected(id) && PUG_CHECK_TEAM(id))
		{
			if((iNum == 1) || is_user_bot(id))
			{
				PUG_LeaderGetRandom(id);
			}
			else
			{
				new iMenu = menu_create(PUG_MOD_CS_TEAMS_STR[TEAM_SPECTATOR],"HANDLER_MenuLeader");
				
				new iPlayer;
				
				for(new i;i < iNum;i++)
				{
					iPlayer = iPlayers[i];
					
					menu_additem(iMenu,fmt("%n",iPlayer),fmt("%i",iPlayer));
				}
				
				menu_setprop(iMenu,MPROP_EXIT,MEXIT_NEVER);
				
				PUG_CancelMenu(id);
				
				PUG_DisplayMenuSingle(id,iMenu);
				
				set_task(10.0,"PUG_LeaderGetRandom",id);
			}
		}
		else
		{
			new iPlayer = iPlayers[random(iNum)];
			
			if(g_iLeader[TEAM_TERRORIST] == id)
			{
				PUG_SetLeader(iPlayer,TEAM_TERRORIST);
			}
			else if(g_iLeader[TEAM_CT] == id)
			{
				PUG_SetLeader(iPlayer,TEAM_CT);
			}
			
			set_task(2.0,"PUG_LeaderMenu",iPlayer);
		}
	}
	else
	{
		PUG_RunState();
		
		remove_task(VOTE_LEAD);
	}
}

public HANDLER_MenuLeader(id,iMenu,iKey)
{
	if(iKey != MENU_EXIT)
	{
		new szNum[3];
		menu_item_getinfo(iMenu,iKey,_,szNum,charsmax(szNum));
		
		PUG_LeaderGetPlayer(id,str_to_num(szNum));
	}
	
	menu_destroy(iMenu);
	
	return PLUGIN_HANDLED;
}

public PUG_LeaderGetRandom(id)
{
	if(is_user_connected(id) && PUG_CHECK_TEAM(id))
	{
		PUG_LeaderGetPlayer(id,find_player("ej","SPECTATOR"));
	}
}

public PUG_LeaderGetPlayer(id,iPlayer)
{
	if(is_user_connected(id))
	{
		if(is_user_connected(iPlayer))
		{
			remove_task(id);
			
			PUG_CancelMenu(id);
		
			rg_set_user_team(iPlayer,(id == g_iLeader[TEAM_TERRORIST]) ? TEAM_TERRORIST : TEAM_CT);
			
			rg_round_respawn(iPlayer);					
			
			client_print_color(0,id,"%s %L",PUG_MOD_HEADER,LANG_SERVER,"PUG_LEADER_PICK",id,iPlayer);
			
			set_task(2.0,"PUG_LeaderMenu",g_iLeader[(id == g_iLeader[TEAM_TERRORIST]) ? TEAM_CT : TEAM_TERRORIST]);
		}
	}
	else
	{
		PUG_LeaderMenu(id);
	}
}

public HOOK_CurWeapon(id)
{
	engclient_cmd(id,"weapon_knife");
	
	return PLUGIN_HANDLED;
}

public HOOK_RoundEnd(WinStatus:Status,ScenarioEventEndRound:Event)
{
	if(Status != WINSTATUS_NONE)
	{
		if(ROUND_CTS_WIN <= Event <= ROUND_TERRORISTS_WIN)
		{
			DisableHookChain(g_hRoundEnd);
			
			disable_event(g_hCurWeapon);
			
			PUG_VoteStart(VOTE_SWAP);
		}
		else
		{
			PUG_LO3(1);
			
			client_print_color(0,print_team_default,"%s %L",PUG_MOD_HEADER,LANG_SERVER,"PUG_DRAW_ROUND");
		}
	}
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
		
		show_hudmessage(0,"%L",LANG_SERVER,"PUG_TEAM_TYPE_5");
	}
}
