#include <amxmodx>
#include <amxmisc>

#include <PugCore>
#include <PugStocks>

#define VOTEKICK_PLAYERS_MIN 3

new bool:g_Votes[MAX_PLAYERS+1][MAX_PLAYERS+1];
new g_VotesNeedTeam[3];

public plugin_init()
{
	register_plugin("Pug Mod (Vote Kick)",PUG_VERSION,PUG_AUTHOR);
	
	register_dictionary("PugVotekick.txt");
	
	PugRegCommand("votekick","VoteKick",ADMIN_ALL,"PUG_VOTEKICK_DESC",true);
}

public client_putinserver(id)
{
	for(new i;i <= MaxClients;i++)
	{
		g_Votes[i][id] = false;
		g_Votes[id][i] = false;
	}
}

public VoteKick(id)
{
	new Team[12];
	new TeamIndex = get_user_team(id,Team,charsmax(Team));
	
	if(1 <= TeamIndex <= 2)
	{
		new Players[MAX_PLAYERS],PlayersCount,Player;
		get_players(Players,PlayersCount,"e",Team);
		
		if(PlayersCount > VOTEKICK_PLAYERS_MIN)
		{
			new Menu = menu_create("Vote Kick:","MenuHandle");
			
			new Option[MAX_NAME_LENGTH];
			
			g_VotesNeedTeam[TeamIndex] = 0;
			
			for(new i;i < PlayersCount;i++)
			{
				Player = Players[i];
				
				if(Player != id && !access(Player,ADMIN_IMMUNITY))
				{
					get_user_name(Player,Option,charsmax(Option));				
	
					menu_additem
					(
						Menu,
						Option,
						fmt("%d",Player), 
						.callback = g_Votes[id][Player] ? menu_makecallback("MenuHandleDisabled") : -1
					);
					
					g_VotesNeedTeam[TeamIndex]++;
				}
			}
			
			menu_display(id,Menu);
		}
		else
		{
			client_print_color(id,id,"%s %L",PUG_HEADER,LANG_SERVER,"PUG_VOTEKICK_NEED_PLAYERS",VOTEKICK_PLAYERS_MIN);
		}
	}
	
	return PLUGIN_HANDLED;
}

public MenuHandleDisabled(id,Menu,Key)
{
	return ITEM_DISABLED;
}

public MenuHandle(id,Menu,Key)
{
	if(Key != MENU_EXIT)
	{
		new Info[3],Option[MAX_NAME_LENGTH];
		menu_item_getinfo(Menu,Key,_,Info,charsmax(Info),Option,charsmax(Option));
		
		new Player = str_to_num(Info);
		
		if(is_user_connected(Player))
		{
			g_Votes[id][Player] = true;		

			new VoteCount = GetVoteCount(Player);
			new VotesNeed = g_VotesNeedTeam[get_user_team(id)];
			new VotesLack = (VotesNeed - VoteCount);
		
			if(!VotesLack)
			{
				server_cmd("kick #%i ^"Kicked by Vote Kick^"",get_user_userid(Player));
				
				client_print_color(0,Player,"%s %L",PUG_HEADER,LANG_SERVER,"PUG_VOTEKICK_KICK_MESSAGE",Option,VotesNeed);
			}
			else
			{
				new Name[MAX_NAME_LENGTH];
				get_user_name(id,Name,charsmax(Name));
				
				client_print_color(0,id,"%s %L",PUG_HEADER,LANG_SERVER,"PUG_VOTEKICK_NEED_MESSAGE",Name,Option,VotesLack);
			}
		}
	}
	
	return PLUGIN_HANDLED;
}

GetVoteCount(Player)
{
	new Count = 0;
	
	for(new i;i <= MaxClients;i++)
	{
		if(g_Votes[i][Player])
		{
			Count++;
		}
	}
	
	return Count;
}
