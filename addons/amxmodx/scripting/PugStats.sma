#include <PugCore>
#include <PugStocks>
#include <PugCS>

new g_iState;
new g_szState[32];

new g_iStatsRoundEnd;
new g_iStatsScoreboard;
new g_iStatsGameDescription;

enum _:eStats
{
	Hit,
	Dmg
};

new g_iStats[MAX_PLAYERS+1][MAX_PLAYERS+1][eStats];

new HookChain:g_hCBasePlayer_TakeDamage;
new HookChain:g_hCSGameRules_OnRoundFreezeEnd;
new HookChain:g_hRoundEnd;

public plugin_init()
{
	register_plugin("Pug Mod (Stats)",PUG_MOD_VERSION,PUG_MOD_AUTHOR,PUG_MOD_LINK,"Round Stats Tracker");

	register_dictionary("PugStats.txt");
	
	bind_pcvar_num(create_cvar("pug_stats_round_end","1",FCVAR_NONE,"Display round stats at console when round end"),g_iStatsRoundEnd);
	
	bind_pcvar_num(create_cvar("pug_stats_scoreboard","1",FCVAR_NONE,"Store team scores on scoreboard after swap teams"),g_iStatsScoreboard);
	
	bind_pcvar_num(create_cvar("pug_stats_game_desc","1",FCVAR_NONE,"Show scores and match status at game description"),g_iStatsGameDescription);
	
	DisableHookChain(g_hCBasePlayer_TakeDamage = RegisterHookChain(RG_CBasePlayer_TakeDamage,"HOOK_CBasePlayer_TakeDamage",true));
	
	DisableHookChain(g_hCSGameRules_OnRoundFreezeEnd = RegisterHookChain(RG_CSGameRules_OnRoundFreezeEnd,"HOOK_CSGameRules_OnRoundFreezeEnd",true));
	
	DisableHookChain(g_hRoundEnd = RegisterHookChain(RG_RoundEnd,"HOOK_RoundEnd",true));
	
	PUG_RegCommand("hp","PUG_HP",ADMIN_ALL,"PUG_DESC_HP");
	PUG_RegCommand("dmg","PUG_DamageDone",ADMIN_ALL,"PUG_DESC_DMG");
	PUG_RegCommand("rdmg","PUG_DamageReceived",ADMIN_ALL,"PUG_DESC_RDMG");
	PUG_RegCommand("sum","PUG_DamageSummary",ADMIN_ALL,"PUG_DESC_SUM");
}

public PUG_Event(iState,szState[])
{
	g_iState = iState;
	
	copy(g_szState,charsmax(g_szState),szState);
	
	if(iState == STATE_FIRST_HALF || iState == STATE_SECOND_HALF || iState == STATE_OVERTIME)
	{
		EnableHookChain(g_hCBasePlayer_TakeDamage);
		
		EnableHookChain(g_hCSGameRules_OnRoundFreezeEnd);
		
		if(g_iStatsRoundEnd)
		{
			EnableHookChain(g_hRoundEnd);
		}
		
		if(iState == STATE_FIRST_HALF)
		{
			if(g_iStatsScoreboard || g_iStatsGameDescription)
			{
				register_message(get_user_msgid("TeamScore"),"HOOK_TeamScore");
			}
		}
	}
	else
	{
		DisableHookChain(g_hCBasePlayer_TakeDamage);
		
		DisableHookChain(g_hCSGameRules_OnRoundFreezeEnd);
		
		if(g_iStatsRoundEnd)
		{
			DisableHookChain(g_hRoundEnd);
		}
	}
}

public HOOK_CSGameRules_OnRoundFreezeEnd()
{
	for(new i;i <= MaxClients;i++)
	{
		for(new j;j <= MaxClients;j++)
		{
			g_iStats[i][j][Hit] = 0;
			g_iStats[i][j][Dmg] = 0;
		}
	}
}

public HOOK_CBasePlayer_TakeDamage(id,pevInflictor,pevAttacker,Float:fDamage)
{
	if((1 <= pevAttacker <= MaxClients) && (id != pevAttacker) && rg_is_player_can_takedamage(id,pevAttacker))
	{
		g_iStats[pevAttacker][id][Hit]++;
		
		g_iStats[pevAttacker][id][Dmg] += floatround(fDamage);
	}
}

public HOOK_RoundEnd(WinStatus:Status)
{
	if(Status != WINSTATUS_NONE)
	{
		new iPlayers[MAX_PLAYERS],iNum;
		get_players(iPlayers,iNum,"h");
		
		new id,iPlayer;
		
		for(new i = 0;i < iNum;i++)
		{
			id = iPlayers[i];
			
			for(new j = 0;j < iNum;j++)
			{
				iPlayer = iPlayers[j];
				
				if((id != iPlayer) && (g_iStats[id][iPlayer][Hit] || g_iStats[iPlayer][id][Hit]))
				{
					client_print
					(
						id,
						print_console,
						"%L",
						LANG_SERVER,
						"PUG_ROUND_DMG",
						g_iStats[id][iPlayer][Dmg],
						g_iStats[id][iPlayer][Hit],
						g_iStats[iPlayer][id][Dmg],
						g_iStats[iPlayer][id][Hit],
						iPlayer
					);
				}
			}
		}
	}
}

public HOOK_TeamScore()
{
	new bool:bRunning = (STATE_FIRST_HALF <= g_iState <= STATE_END);
	
	if(g_iStatsScoreboard && bRunning)
	{
		new szTeam[4] = {0};
		get_msg_arg_string(1,szTeam,charsmax(szTeam));
		
		set_msg_arg_int(2,ARG_SHORT,(szTeam[0] == 'T') ? PUG_GetScore(TEAM_TERRORIST) : PUG_GetScore(TEAM_CT));
	}
	
	if(g_iStatsGameDescription)
	{
		new szText[64] = {0};
		
		if(bRunning)
		{
			formatex(szText,charsmax(szText),"%s: TR %i - %i CT",g_szState,PUG_GetScore(TEAM_TERRORIST),PUG_GetScore(TEAM_CT));
		}
		else
		{
			formatex(szText,charsmax(szText),"%s: %s",PUG_MOD_PLUGIN,g_szState);
		}
		
		set_member_game(m_GameDesc,szText);
	}
}

public PUG_HP(id)
{
	if((g_iState == STATE_FIRST_HALF || g_iState == STATE_SECOND_HALF || g_iState == STATE_OVERTIME) && (!is_user_alive(id) || get_member_game(m_bRoundTerminating)))
	{
		new TeamName:iTeam = get_member_s(id,m_iTeam);
		
		if(TEAM_TERRORIST <= iTeam <= TEAM_CT)
		{
			new iPlayers[MAX_PLAYERS],iNum;
			get_players(iPlayers,iNum,"aeh",(iTeam == TEAM_TERRORIST) ? "CT" : "TERRORIST");
			
			if(iNum)
			{
				new iPlayer;
				
				for(new i;i < iNum;i++)
				{
					iPlayer = iPlayers[i];
					
					client_print_color(id,iPlayer,"%s %L",PUG_MOD_HEADER,LANG_SERVER,"PUG_HP_CMD",iPlayer,get_user_health(iPlayer),get_user_armor(iPlayer));
				}
			}
			else
			{
				client_print_color(id,id,"%s %L",PUG_MOD_HEADER,LANG_SERVER,"PUG_HP_NONE");
			}
			
			return PLUGIN_HANDLED;
		}
	}

	client_print_color(id,id,"%s %L",PUG_MOD_HEADER,LANG_SERVER,"PUG_CMD_ERROR");
	
	return PLUGIN_HANDLED;
}

public PUG_DamageDone(id)
{
	if((g_iState == STATE_FIRST_HALF || g_iState == STATE_SECOND_HALF || g_iState == STATE_OVERTIME) && (!is_user_alive(id) || get_member_game(m_bRoundTerminating)))
	{
		new TeamName:iTeam = get_member_s(id,m_iTeam);
		
		if(TEAM_TERRORIST <= iTeam <= TEAM_CT)
		{
			new iPlayers[MAX_PLAYERS],iNum,iPlayer;
			get_players(iPlayers,iNum,"e",(iTeam == TEAM_TERRORIST) ? "CT" : "TERRORIST");
			
			new bool:bDamage = false;

			for(new i;i < iNum;i++)
			{
				iPlayer = iPlayers[i];
				
				if(g_iStats[id][iPlayer][Hit])
				{
					bDamage = true;
					
					client_print_color(id,iPlayer,"%s %L",PUG_MOD_HEADER,LANG_SERVER,"PUG_DMG",iPlayer,g_iStats[id][iPlayer][Hit],g_iStats[id][iPlayer][Dmg]);
				}
			}
			
			if(!bDamage)
			{
				client_print_color(id,id,"%s %L",PUG_MOD_HEADER,LANG_SERVER,"PUG_NODMG");
			}
			
			return PLUGIN_HANDLED;
		}
		
	}

	client_print_color(id,id,"%s %L",PUG_MOD_HEADER,LANG_SERVER,"PUG_CMD_ERROR");
	
	return PLUGIN_HANDLED;
}

public PUG_DamageReceived(id)
{
	if((g_iState == STATE_FIRST_HALF || g_iState == STATE_SECOND_HALF || g_iState == STATE_OVERTIME) && (!is_user_alive(id) || get_member_game(m_bRoundTerminating)))
	{
		new TeamName:iTeam = get_member_s(id,m_iTeam);
		
		if(TEAM_TERRORIST <= iTeam <= TEAM_CT)
		{
			new iPlayers[MAX_PLAYERS],iNum,iPlayer;
			get_players(iPlayers,iNum,"e",(iTeam == TEAM_TERRORIST) ? "CT" : "TERRORIST");
			
			new bool:bDamage = false;
			
			for(new i;i < iNum;i++)
			{
				iPlayer = iPlayers[i];
				
				if(g_iStats[iPlayer][id][Hit])
				{
					bDamage = true;
					
					client_print_color(id,iPlayer,"%s %L",PUG_MOD_HEADER,LANG_SERVER,"PUG_RDMG",iPlayer,g_iStats[iPlayer][id][Hit],g_iStats[iPlayer][id][Dmg]);
				}
			}
			
			if(!bDamage)
			{
				client_print_color(id,id,"%s %L",PUG_MOD_HEADER,LANG_SERVER,"PUG_NORDMG");
			}
			
			return PLUGIN_HANDLED;
		}		
	}

	client_print_color(id,id,"%s %L",PUG_MOD_HEADER,LANG_SERVER,"PUG_CMD_ERROR");
	
	return PLUGIN_HANDLED;
}

public PUG_DamageSummary(id)
{
	if((g_iState == STATE_FIRST_HALF || g_iState == STATE_SECOND_HALF || g_iState == STATE_OVERTIME) && (!is_user_alive(id) || get_member_game(m_bRoundTerminating)))
	{
		new TeamName:iTeam = get_member_s(id,m_iTeam);
		
		if(TEAM_TERRORIST <= iTeam <= TEAM_CT)
		{
			new iPlayers[MAX_PLAYERS],iNum,iPlayer;
			get_players(iPlayers,iNum,"e",(iTeam == TEAM_TERRORIST) ? "CT" : "TERRORIST");
			
			new bool:bDamage = false;
			
			for(new i;i < iNum;i++)
			{
				iPlayer = iPlayers[i];
				
				if(g_iStats[id][iPlayer][Hit] || g_iStats[iPlayer][id][Hit])
				{
					bDamage = true;
					
					client_print_color
					(
						id,
						iPlayer,
						"%s %L",
						PUG_MOD_HEADER,
						LANG_SERVER,
						"PUG_SUM",
						g_iStats[id][iPlayer][Dmg],
						g_iStats[id][iPlayer][Hit],
						g_iStats[iPlayer][id][Dmg],
						g_iStats[iPlayer][id][Hit],
						iPlayer,
						get_user_health(iPlayer)
					);
				}
			}
			
			if(!bDamage)
			{
				client_print_color(id,id,"%s %L",PUG_MOD_HEADER,LANG_SERVER,"PUG_NOSUM");
			}
			
			return PLUGIN_HANDLED;
		}
	}
	
	client_print_color(id,id,"%s %L",PUG_MOD_HEADER,LANG_SERVER,"PUG_CMD_ERROR");
	
	return PLUGIN_HANDLED;
}
