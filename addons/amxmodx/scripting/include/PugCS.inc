#if defined _PugCS_included
	#endinput
#endif
#define _PugCS_included

#define PUG_PLAYER_IN_TEAM(%0)	(CS_TEAM_T <= cs_get_user_team(%0) <= CS_TEAM_CT)

 /**
 * Get players count in teams
 *
 * @param Bots	If true, will also count bots
 *
 * @return 	Return the number of active players in teams
 */
 stock PUG_GetPlayersNum(bool:bBots = false)
 {
	new iPlayers[MAX_PLAYERS],iNum[2];
	
	get_players(iPlayers,iNum[0],bBots ? "he" : "che","TERRORIST");
	get_players(iPlayers,iNum[1],bBots ? "he" : "che","CT");
	
	return iNum[0] + iNum[1];
}

 /**
 * Get players count in specific team
 *
 * @param Bots	If true, will also count bots
 * @param Team	Team to count
 *
 * @return 	Return the number of active players
 */
 stock PUG_GetPlayersTeam(CsTeams:iTeam,bool:bBots = true)
 {
 	if(CS_TEAM_T <= iTeam <= CS_TEAM_CT)
	{
		new iPlayers[MAX_PLAYERS],iNum;
		get_players(iPlayers,iNum,bBots ? "he" : "che",(iTeam == CS_TEAM_T) ? "TERRORIST" : "CT");
	
		return iNum;
	}
	
	return 0;
}

 /**
 * Add or remove map objectives
 *
 * @param Block		If true, map objectives will be blocked
 *			Set to false to allow map objectives
 *
 * @noreturn
 */
stock PUG_SetMapObjectives(bool:bBlock)
{
	static const szMapEntity[][] = 
	{
		"func_bomb_target",
		"info_bomb_target",
		"func_hostage_rescue",
		"info_hostage_rescue",
		"info_vip_start",
		"func_vip_safetyzone",
		"func_escapezone"
	};
	
	static const szMapEntityBlock[][] = 
	{
		"_func_bomb_target",
		"_info_bomb_target",
		"_func_hostage_rescue",
		"_info_hostage_rescue",
		"_info_vip_start",
		"_func_vip_safetyzone",
		"_func_escapezone"
	}
	
	new iEntity = FM_NULLENT;

	for(new i;i < sizeof(szMapEntity);i++)
	{
		while((iEntity = engfunc(EngFunc_FindEntityByString,iEntity,"classname",bBlock ? szMapEntity[i] : szMapEntityBlock[i])) > 0)
		{
			set_pev(iEntity,pev_classname,bBlock ? szMapEntityBlock[i] : szMapEntity[i]);
		}
	}
}

 /**
 * Add or remove full map buyzone
 *
 * @param Enable	If true, map will have full buyzone
 *			Set to false remove full map buyzone
 *
 * @noreturn
 */
stock PUG_SetFullBuyzone(bool:bEnable)
{
	static iEntityBuyZone = FM_NULLENT;
	
	if(bEnable)
	{
		if(iEntityBuyZone == FM_NULLENT)
		{
			if((iEntityBuyZone = engfunc(EngFunc_CreateNamedEntity,engfunc(EngFunc_AllocString,"func_buyzone"))) > 0)
			{
				dllfunc(DLLFunc_Spawn,iEntityBuyZone);
				engfunc(EngFunc_SetSize,iEntityBuyZone,{-8191.0,-8191.0,-8191.0},{8191.0,8191.0,8191.0});
			}
		}
	}
	else
	{
		if(iEntityBuyZone != FM_NULLENT)
		{
			engfunc(EngFunc_RemoveEntity,iEntityBuyZone);
			iEntityBuyZone = FM_NULLENT;
		}
	}
}

/**
 * Organize teams randomly
 *
 * @noreturn
 */
stock PUG_TeamsRandomize()
{
	new iPlayers[MAX_PLAYERS],iNum;
	get_players(iPlayers,iNum);
	
	for(new i;i < iNum;i++)
	{
		if(!PUG_PLAYER_IN_TEAM(iPlayers[i]))
		{
			iPlayers[i--] = iPlayers[--iNum];
		}
	}
    
	new iPlayer,iRandom;
	new CsTeams:iTeam = random(2) ? CS_TEAM_T : CS_TEAM_CT;
	
	while(iNum)
	{
		iRandom = random(iNum);
		
		iPlayer = iPlayers[iRandom];
		
		cs_set_user_team(iPlayer,iTeam);
		
		iPlayers[iRandom] = iPlayers[--iNum];
		
		iTeam = (iTeam % CS_TEAM_CT + CS_TEAM_T);
	}
}

/**
 * Organize using skill algorithm
 *
 * @noreturn
 */
stock PUG_TeamsOptimize()
{
	new iSkills[MAX_PLAYERS],iSorted[MAX_PLAYERS];
	
	new iPlayers[MAX_PLAYERS],iNum,iPlayer;
	get_players(iPlayers,iNum,"h");
	
	new iFrags;
	
	for(new i;i < iNum;i++)
	{
		iPlayer = iPlayers[i];
		
		if(PUG_PLAYER_IN_TEAM(iPlayer))
		{
			iFrags = get_user_frags(iPlayer);
			iSorted[iPlayer] = iSkills[iPlayer] = (get_user_time(iPlayer,1) / ((iFrags < 1) ? 1 : iFrags));	
		}
	}
	
	SortIntegers(iSorted,sizeof(iSorted),Sort_Descending);

	new CsTeams:iCheck = CS_TEAM_T;
	
	for(new i;i < sizeof(iSorted);i++)
	{
		for(new j;j < iNum;j++)
		{
			iPlayer = iPlayers[j];
			
			if(PUG_PLAYER_IN_TEAM(iPlayer))
			{
				if(iSkills[iPlayer] == iSorted[i])
				{
					cs_set_user_team(iPlayer,iCheck);
					
					iCheck++;
					
					if(iCheck > CS_TEAM_CT)
					{
						iCheck = CS_TEAM_T;
					}
				}	
			}
		}
	}
}

/**
 * Swap Teams
 *
 * @noreturn
 */
stock PUG_TeamsSwap()
{
	new iPlayers[MAX_PLAYERS],iNum,iPlayer;
	get_players(iPlayers,iNum,"h");
	
	for(new i;i < iNum;i++)
	{
		iPlayer = iPlayers[i];
		
		switch(cs_get_user_team(iPlayer))
		{
			case CS_TEAM_T:
			{
				cs_set_user_team(iPlayer,CS_TEAM_CT);
			}
			case CS_TEAM_CT:
			{
				cs_set_user_team(iPlayer,CS_TEAM_T);
			}
		}
	}
}
