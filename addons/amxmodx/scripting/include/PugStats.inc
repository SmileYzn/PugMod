#if defined _PugStats_included
	#endinput
#endif
#define _PugStats_included

/**
 * Get player accuracy
 *
 * @param Shots     	Shots fired
 * @param Hits		Hits done
 *
 * @return      	Return accuracy
 */
stock Float:PugGetAccuracy(Shots,Hits)
{
	if(!Shots)
	{
		return 0.0;
	}
	
	return (100.0 * float(Hits) / float(Shots));
}

/**
 * Get player kills efficiency
 *
 * @param Kills     	Frags
 * @param Deaths	Deaths
 *
 * @return      	Return efficiency
 */
stock Float:PugGetEfficiency(Kills,Deaths)
{
	if(!Kills)
	{
		return 0.0;
	}
	
	return (100.0 * float(Kills) / float(Kills + Deaths));
}

/**
 * Get player headshot percentage
 *
 * @param Headshots     Headshots done
 * @param Hits		Hits done
 *
 * @return      	Return headshot percentage
 */
stock Float:PugGetHSP(Headshots,Hits)
{
	if(!Hits)
	{
		return 0.0;
	}
	
	return (100.0 * float(Headshots) / float(Hits));
}

/**
 * Get player average damage per round
 *
 * @param Rounds     	Rounds played
 * @param Damage	Damage total
 *
 * @return      	Return average damage per round
 */
stock Float:PugGetADR(Rounds,Damage)
{
	if(!Rounds)
	{
		return 0.0;
	}
	
	return (float(Damage) / float(Rounds));
}

/**
 * Get player frags per round
 *
 * @param Rounds     	Rounds played
 * @param Kills		Total of kills
 *
 * @return      	Return frags per round
 */
stock Float:PugGetFPR(Rounds,Kills)
{
	if(!Rounds)
	{
		return 0.0;
	}
	
	return (float(Kills) / float(Rounds));
}

stock Float:PugGetKDR(Kills,Deaths)
{
	if(!Deaths)
	{
		return 0.0;
	}
	
	return (float(Kills) / float(Deaths));
}

stock Float:PugGetSkill(Efficiency,Accuracy)
{
	if(!Efficiency || !Efficiency)
	{
		return 0.0;
	}
	
	return ((Efficiency + Accuracy) / 2.0);
}

stock Float:PugGetWinPercentage(RoundsWin,RoundsPlay)
{
	if(!RoundsPlay)
	{
		return 0.0;
	}
	
	return (100.0 * float(RoundsWin) / float(RoundsPlay));
}

stock Float:PugGetLossPercentage(RoundsLose,RoundsPlay)
{
	if(!RoundsPlay)
	{
		return 0.0;
	}
	
	return (100.0 * float(RoundsLose) / float(RoundsPlay));
}
