#if defined _PugCS_included
	#endinput
#endif
#define _PugCS_included

#define PUG_CHECK_TEAM(%0)	(TEAM_TERRORIST <= get_member_s(%0,m_iTeam) <= TEAM_CT)

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
	
	return (iNum[0] + iNum[1]);
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
		if(!PUG_CHECK_TEAM(iPlayers[i]))
		{
			iPlayers[i--] = iPlayers[--iNum];
		}
	}
    
	new iPlayer,iRandom;
	new TeamName:iTeam = random(2) ? TEAM_TERRORIST : TEAM_CT;
	
	while(iNum)
	{
		iRandom = random(iNum);
		
		iPlayer = iPlayers[iRandom];
		
		rg_set_user_team(iPlayer,iTeam);
		
		iPlayers[iRandom] = iPlayers[--iNum];
		
		iTeam = (iTeam % TEAM_CT + TEAM_TERRORIST);
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
		
		if(PUG_CHECK_TEAM(iPlayer))
		{
			iFrags = get_user_frags(iPlayer);
			
			iSorted[iPlayer] = iSkills[iPlayer] = (get_user_time(iPlayer,1) / ((iFrags < 1) ? 1 : iFrags));	
		}
	}
	
	SortIntegers(iSorted,sizeof(iSorted),Sort_Descending);

	new TeamName:iCheck = TEAM_TERRORIST;
	
	for(new i;i < sizeof(iSorted);i++)
	{
		for(new j;j < iNum;j++)
		{
			iPlayer = iPlayers[j];
			
			if(PUG_CHECK_TEAM(iPlayer))
			{
				if(iSkills[iPlayer] == iSorted[i])
				{
					rg_set_user_team(iPlayer,iCheck);
					
					iCheck++;
					
					if(iCheck > TEAM_CT)
					{
						iCheck = TEAM_TERRORIST;
					}
				}	
			}
		}
	}
}
