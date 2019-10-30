#include <amxmodx>
#include <fakemeta>
#include <cstrike>
#include <csx>

#include <PugCore>
#include <PugStocks>
#include <PugCS>

new g_Hits[MAX_PLAYERS+1][MAX_PLAYERS+1];
new g_Damage[MAX_PLAYERS+1][MAX_PLAYERS+1];

new bool:g_Live;
new bool:g_Round;

new g_FM_Voice_SetClientListening;

public plugin_init()
{
	register_plugin("Pug Mod (Aux)",PUG_VERSION,PUG_AUTHOR);

	register_dictionary("PugCore.txt");
	register_dictionary("PugAux.txt");

	PugRegCommand("hp","HP",ADMIN_ALL,"PUG_DESC_HP");
	PugRegCommand("dmg","Damage",ADMIN_ALL,"PUG_DESC_DMG");
	PugRegCommand("rdmg","RecivedDamage",ADMIN_ALL,"PUG_DESC_RDMG");
	PugRegCommand("sum","Summary",ADMIN_ALL,"PUG_DESC_SUM");
	
	register_logevent("RoundStart",2,"1=Round_Start");
	register_logevent("RoundEnd",2,"1=Round_End");
}

public PugEvent(State)
{
	g_Live = (State == STATE_FIRSTHALF || State == STATE_SECONDHALF || State == STATE_OVERTIME);
	
	if(g_Live)
	{
		g_FM_Voice_SetClientListening = register_forward(FM_Voice_SetClientListening,"FMVoiceSetClientListening",false);
	}
	else
	{		
		unregister_forward(FM_Voice_SetClientListening,g_FM_Voice_SetClientListening,false);
	}
}

public client_putinserver(id)
{
	for(new i;i < MAX_PLAYERS;i++)
	{
		g_Hits[i][id] = 0;
		g_Damage[i][id] = 0;
	}
}

public client_damage(Attacker,Victim,AttackDamage)
{
	g_Hits[Attacker][Victim]++;
	g_Damage[Attacker][Victim] += AttackDamage;
}

public RoundStart()
{
	g_Round = true;
	
	for(new i;i < MAX_PLAYERS;i++)
	{
		arrayset(g_Hits[i],0,sizeof(g_Hits));
		arrayset(g_Damage[i],0,sizeof(g_Damage));	
	}
}

public RoundEnd()
{
	g_Round = false;
}

public HP(id)
{
	if(g_Live)
	{
		if((is_user_alive(id) && g_Round) || !JoinedTeam(id))
		{
			PugMsg(id,"PUG_CMD_ERROR");
		}
		else
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
					
					client_print_color(id,print_team_red,"%s %L",g_Head,LANG_SERVER,"PUG_HP_CMD",Name,get_user_health(Player),get_user_armor(Player));
				}
			}
			else
			{
				PugMsg(id,"PUG_HP_NONE");
			}
		}
	}
	else
	{
		PugMsg(id,"PUG_CMD_ERROR");
	}
	
	return PLUGIN_HANDLED;
}

public Damage(id)
{
	if(g_Live)
	{
		if((is_user_alive(id) && g_Round) || !JoinedTeam(id))
		{
			PugMsg(id,"PUG_CMD_ERROR");
		}
		else
		{
			new Players[MAX_PLAYERS],Num,Player;
			get_players(Players,Num,"h");
			
			new Name[MAX_NAME_LENGTH],bool:HaveHits;
			
			for(new i;i < Num;i++)
			{
				Player = Players[i];
				
				if(g_Hits[id][Player])
				{
					HaveHits = true;
					
					if(Player == id)
					{
						client_print_color(id,print_team_red,"%s %L",g_Head,LANG_SERVER,"PUG_DMG_SELF",g_Hits[id][Player],g_Damage[id][Player]);
					}
					else
					{
						get_user_name(Player,Name,charsmax(Name));
						
						client_print_color(id,print_team_red,"%s %L",g_Head,LANG_SERVER,"PUG_DMG",Name,g_Hits[id][Player],g_Damage[id][Player]);
					}
				}
			}
			
			if(!HaveHits)
			{
				PugMsg(id,"PUG_NODMG");
			}
		}
	}
	else
	{
		PugMsg(id,"PUG_CMD_ERROR");
	}
	
	return PLUGIN_HANDLED;
}

public RecivedDamage(id)
{
	if(g_Live)
	{
		if((is_user_alive(id) && g_Round) || !JoinedTeam(id))
		{
			PugMsg(id,"PUG_CMD_ERROR");
		}
		else
		{
			new Players[MAX_PLAYERS],Num,Player;
			get_players(Players,Num,"h");
			
			new Name[MAX_NAME_LENGTH],bool:HaveHits;
			
			for(new i;i < Num;i++)
			{
				Player = Players[i];
				
				if(g_Hits[Player][id])
				{
					HaveHits = true;
					
					if(Player == id)
					{
						client_print_color(id,print_team_red,"%s %L",g_Head,LANG_SERVER,"PUG_RDMG_SELF",g_Hits[Player][id],g_Damage[Player][id]);
					}
					else
					{
						get_user_name(Player,Name,charsmax(Name));	
						
						client_print_color(id,print_team_red,"%s %L",g_Head,LANG_SERVER,"PUG_RDMG",Name,g_Hits[Player][id],g_Damage[Player][id]);
					}
				}
			}
			
			if(!HaveHits)
			{
				PugMsg(id,"PUG_NORDMG");
			}
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
	if(g_Live)
	{
		if((is_user_alive(id) && g_Round) || !JoinedTeam(id))
		{
			PugMsg(id,"PUG_CMD_ERROR");
		}
		else
		{
			new Players[MAX_PLAYERS],Num,Player;
			get_players(Players,Num,"h");
			
			new Name[MAX_NAME_LENGTH],bool:HaveHits;
			
			for(new i;i < Num;i++)
			{
				Player = Players[i];
				
				if(id == Player)
				{
					continue;
				}
				
				if(g_Hits[id][Player] || g_Hits[Player][id])
				{
					HaveHits = true;

					get_user_name(Player,Name,charsmax(Name));
					
					client_print_color(id,print_team_red,"%s %L",g_Head,LANG_SERVER,"PUG_SUM",g_Damage[id][Player],g_Hits[id][Player],g_Damage[Player][id],g_Hits[id][Player],Name,get_user_health(Player));
				}
			}
		
			if(!HaveHits)
			{
				PugMsg(id,"PUG_NOSUM");
			}
		}
	}
	else
	{
		PugMsg(id,"PUG_CMD_ERROR");
	}
	
	return PLUGIN_HANDLED;
}

public FMVoiceSetClientListening(Recv,Sender,bool:Listen)
{
	if(g_Live && (Recv != Sender))
	{
		if(is_user_connected(Recv) && is_user_connected(Sender))
		{
			if(get_user_team(Recv) == get_user_team(Sender))
			{
				engfunc(EngFunc_SetClientListening,Recv,Sender,true);
				return FMRES_SUPERCEDE;
			}
		}
	}
	
	return FMRES_IGNORED;
}
