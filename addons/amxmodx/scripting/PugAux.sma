#include <amxmodx>
#include <fakemeta>
#include <csx>

#include <PugCore>
#include <PugStocks>
#include <PugCS>

new bool:g_Live;

public plugin_init()
{
	register_plugin("Pug Mod (Aux)",PUG_VERSION,PUG_AUTHOR);

	register_dictionary("PugCore.txt");
	register_dictionary("PugAux.txt");

	PugRegCommand("hp","HP",ADMIN_ALL,"PUG_DESC_HP");
	PugRegCommand("dmg","DamageDone",ADMIN_ALL,"PUG_DESC_DMG");
	PugRegCommand("rdmg","DamageReceived",ADMIN_ALL,"PUG_DESC_RDMG");
	PugRegCommand("sum","Summary",ADMIN_ALL,"PUG_DESC_SUM");
}

public PugEvent(State)
{
	g_Live = (State == STATE_FIRSTHALF || State == STATE_SECONDHALF || State == STATE_OVERTIME);
}

public HP(id)
{
	if(g_Live && JoinedTeam(id) && (!is_user_alive(id) || get_gamerules_int("CHalfLifeMultiplay","m_bRoundTerminating")))
	{
		new Players[MAX_PLAYERS],Num;
		get_players(Players,Num,"aeh",(get_user_team(id) == 1) ? "CT" : "TERRORIST");
		
		if(Num)
		{
			new Name[MAX_NAME_LENGTH],Player;
			
			for(new i;i < Num;i++)
			{
				Player = Players[i];
			
				get_user_name(Player,Name,charsmax(Name));
				
				client_print_color(id,Player,"%s %L",PUG_HEADER,LANG_SERVER,"PUG_HP_CMD",Name,get_user_health(Player),get_user_armor(Player));
			}
		}
		else
		{
			PugMsg(id,"PUG_HP_NONE");
		}
	}
	else
	{
		PugMsg(id,"PUG_CMD_ERROR");
	}
	
	return PLUGIN_HANDLED;
}

public DamageDone(id)
{
	if(g_Live && JoinedTeam(id) && (!is_user_alive(id) || get_gamerules_int("CHalfLifeMultiplay","m_bRoundTerminating")))
	{
		new Name[MAX_NAME_LENGTH];
		new Stats[STATSX_MAX_STATS],BodyHits[MAX_BODYHITS];
		
		new bool:HasStats = false;
		
		new Players[MAX_PLAYERS],Num,Player;
		get_players(Players,Num,"e",(get_user_team(id) == 1) ? "CT" : "TERRORIST");
		
		for(new i;i < Num;i++)
		{
			Player = Players[i];
			
			arrayset(Stats,0,sizeof(Stats));
			
			if(get_user_vstats(id,Player,Stats,BodyHits))
			{				
				HasStats = true;
				
				get_user_name(Player,Name,charsmax(Name));

				client_print_color(id,Player,"%s %L",PUG_HEADER,LANG_SERVER,"PUG_DMG",Name,Stats[STATSX_HITS],Stats[STATSX_DAMAGE]);
			}
		}
		
		if(!HasStats)
		{
			PugMsg(id,"PUG_NODMG");
		}
		
	}
	else
	{
		PugMsg(id,"PUG_CMD_ERROR");
	}
	
	return PLUGIN_HANDLED;
}

public DamageReceived(id)
{
	if(g_Live && JoinedTeam(id) && (!is_user_alive(id) || get_gamerules_int("CHalfLifeMultiplay","m_bRoundTerminating")))
	{
		new Name[MAX_NAME_LENGTH];
		new Stats[STATSX_MAX_STATS],BodyHits[MAX_BODYHITS];
		
		new bool:HasStats = false;
		
		new Players[MAX_PLAYERS],Num,Player;
		get_players(Players,Num,"e",(get_user_team(id) == 1) ? "CT" : "TERRORIST");
		
		for(new i;i < Num;i++)
		{
			Player = Players[i];
			
			arrayset(Stats,0,sizeof(Stats));
			
			if(get_user_astats(id,Player,Stats,BodyHits))
			{
				HasStats = true;
				
				get_user_name(Player,Name,charsmax(Name));
				
				client_print_color(id,Player,"%s %L",PUG_HEADER,LANG_SERVER,"PUG_RDMG",Name,Stats[STATSX_HITS],Stats[STATSX_DAMAGE]);
			}
		}
		
		if(!HasStats)
		{
			PugMsg(id,"PUG_NORDMG");
		}		
	}
	else
	{
		PugMsg(id,"PUG_CMD_ERROR");
	}
	
	return PLUGIN_HANDLED;
}

public Summary(id)
{
	if(g_Live && JoinedTeam(id) && (!is_user_alive(id) || get_gamerules_int("CHalfLifeMultiplay","m_bRoundTerminating")))
	{
		new Name[MAX_NAME_LENGTH];
		new Stats[2][STATSX_MAX_STATS],BodyHits[2][MAX_BODYHITS];
		
		new bool:HasStats = false;
		
		new Players[MAX_PLAYERS],Num,Player;
		get_players(Players,Num,"e",(get_user_team(id) == 1) ? "CT" : "TERRORIST");
		
		for(new i;i < Num;i++)
		{
			Player = Players[i];
			
			arrayset(Stats[0],0,sizeof(Stats[]));
			arrayset(Stats[1],0,sizeof(Stats[]));
			
			if(get_user_vstats(id,Player,Stats[0],BodyHits[0]) || get_user_astats(id,Player,Stats[1],BodyHits[1]))
			{
				HasStats = true;
				
				get_user_name(Player,Name,charsmax(Name));
				
				client_print_color(id,Player,"%s %L",PUG_HEADER,LANG_SERVER,"PUG_SUM",Stats[0][STATSX_DAMAGE],Stats[0][STATSX_HITS],Stats[1][STATSX_DAMAGE],Stats[1][STATSX_HITS],Name,is_user_alive(Player) ? get_user_health(Player) : 0);
			}
		}
		
		if(!HasStats)
		{
			PugMsg(id,"PUG_NOSUM");
		}
	}
	else
	{
		PugMsg(id,"PUG_CMD_ERROR");
	}
	
	return PLUGIN_HANDLED;
}
