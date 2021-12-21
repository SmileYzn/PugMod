#if defined _PugMenus_included
	#endinput
#endif
#define _PugMenus_included

#define PUG_MENU_MAPS 32
#define PUG_MENU_TEAM 6

 /**
 * Store current view menu id for players
 */
stock g_iViewMenu[MAX_PLAYERS+1] = {-1,...};

 /**
 * Display an menu for all players that are in teams
 *
 * @param iMenu	The menu to display for users
 *
 * @noreturn
 */
stock PUG_DisplayMenuAll(iMenu)
{
	new iPlayers[MAX_PLAYERS],iNum;
	get_players(iPlayers,iNum,"ch");
	
	new iPlayer;
	
	for(new i;i < iNum;i++)
	{
		iPlayer = iPlayers[i];

		if(1 <= get_user_team(iPlayer) <= 2)
		{
			PUG_DisplayMenuSingle(iPlayer,iMenu);
		}
	}
}

 /**
 * Display an menu for all players of selected team
 *
 * @param iMenu		The menu to display for users
 * @param szTeam	The team to display menu: TERRORIST, CT, SPECTATOR
 *
 * @return		Number of players that menu will display
 */
stock PUG_DisplayMenuTeam(iMenu,szTeam[])
{
	new iPlayers[MAX_PLAYERS],iNum;
	get_players(iPlayers,iNum,"che",szTeam);
	
	new iPlayer;
	
	for(new i;i < iNum;i++)
	{
		iPlayer = iPlayers[i];

		PUG_DisplayMenuSingle(iPlayer,iMenu);
	}
	
	return iNum;
}

 /**
 * Display an menu for a single target
 * and store the current menu into a variable
 *
 * @note 	If other menu is showing, this function will try
 *		to display menu every one second until reach target
 *
 * @param id	The client index
 * @param Menu	The menu to display 
 *
 * @noreturn
 */
stock PUG_DisplayMenuSingle(id,iMenu)
{
	g_iViewMenu[id] = iMenu;
	
	PUG_DisplayMenuClient(id);
}

public PUG_DisplayMenuClient(id)
{
	if((g_iViewMenu[id] != -1) && is_user_connected(id))
	{
		new iMenu[2] = {0,0};

		if(!player_menu_info(id,iMenu[0],iMenu[1]))
		{
			menu_display(id,g_iViewMenu[id]);	
		}
		else
		{
			set_task(1.0,"PUG_DisplayMenuClient",id);	
		}
	}
}

/**
 * Check if players have any PUG menu opened
 *
 * @param id	The client index
 *
 * @noreturn
 */
stock PUG_InMenu(id)
{
	return (g_iViewMenu[id] != -1);
}

/**
 * Cancel a client menu and invalidate it
 *
 * @param id	The client index
 *
 * @noreturn
 */
stock PUG_CancelMenu(id)
{
	show_menu(id,0,"^n",1);
	
	if(id)
	{
		g_iViewMenu[id] = -1;
		
		menu_cancel(id);
	}
	else
	{
		new iPlayers[MAX_PLAYERS],iNum;
		get_players(iPlayers,iNum,"ch");
		
		new iPlayer;
		
		for(new i;i < iNum;i++)
		{
			iPlayer = iPlayers[i];
			
			g_iViewMenu[iPlayer] = -1;
			
			menu_cancel(iPlayer);
		}
	}	
}
