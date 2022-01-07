#include <PugCore>
#include <PugStocks>
#include <sqlx>

#define PUG_STATS_ASSISTANCE_DMG 50
#define PUG_STATS_RWS_MAP_TARGET 0.7
#define PUG_STATS_RWS_C4_EXPLODE 0.3
#define PUG_STATS_RWS_C4_DEFUSED 0.3

new g_szDBHost[64];
new g_szDBName[64];
new g_szDBUser[64];
new g_szDBPass[64];
new g_szDBLink[64];

enum _:eQueryType
{
	SQL_SAVE_USER,
	SQL_SAVE_GAME,
	SQL_SAVE_STAT,
	SQL_SAVE_WEAP
};

new Handle:g_hDbTuple;

enum _:ePlayerStats
{
	Index,
	Kill,
	Assist,
	Death,
	Headshot,
	Shot,
	Hit,
	Damage,
	Money,

	RoundPlay,
	RoundLose,
	RoundWin,
	Float:RoundWinShare,

	BombPlant,
	BombDefuse,
	BombExplode,

	Streak[MAX_PLAYERS],
	Versus[MAX_PLAYERS],
};
 
enum _:eWeaponStats
{
	WEAPON_KILL,
	WEAPON_DEATH,
	WEAPON_HEADSHOT,
	WEAPON_SHOT,
	WEAPON_HIT,
	WEAPON_DAMAGE
};

new g_iGameIndex;

new g_iStats[MAX_PLAYERS+1][ePlayerStats];
new g_iStatsWeapon[MAX_PLAYERS+1][WeaponIdType][eWeaponStats];

new g_iRoundBombPlanter;
new g_iRoundBombDefuser;

new g_iRoundKill[MAX_PLAYERS+1];
new g_iRoundDamage[MAX_PLAYERS+1][MAX_PLAYERS+1];
new g_iRoundVersus[MAX_PLAYERS+1];

new g_iRoundDamageSelf[MAX_PLAYERS+1];
new g_iRoundDamageTeam[TeamName];

new HookChain:g_hCSGameRules_PlayerKilled;
new HookChain:g_hCBasePlayer_TakeDamage;
new HookChain:g_hCBasePlayer_SetAnimation;
new HookChain:h_hCSGameRules_OnRoundFreezeEnd;
new HookChain:g_hRoundEnd;
new HookChain:g_hPlantBomb;
new HookChain:g_hCGrenade_DefuseBombEnd;
new HookChain:g_hCGrenade_ExplodeBomb;
new HookChain:g_hCBasePlayer_AddAccount;

public plugin_init()
{
	register_plugin("Pug Mod (Ranking)",PUG_MOD_VERSION,PUG_MOD_AUTHOR,PUG_MOD_LINK,"Ranking And Stats tracker");

	register_dictionary("PugRank.txt");

	bind_pcvar_string(create_cvar("pug_db_host","",FCVAR_NONE,"Stats DB Host"),g_szDBHost,charsmax(g_szDBHost));
	bind_pcvar_string(create_cvar("pug_db_name","",FCVAR_NONE,"Stats DB Name"),g_szDBName,charsmax(g_szDBName));
	bind_pcvar_string(create_cvar("pug_db_user","",FCVAR_NONE,"Stats DB User"),g_szDBUser,charsmax(g_szDBUser));
	bind_pcvar_string(create_cvar("pug_db_pass","",FCVAR_NONE,"Stats DB Pass"),g_szDBPass,charsmax(g_szDBPass));
	bind_pcvar_string(create_cvar("pug_db_link","",FCVAR_NONE,"Stats DB Link (Ie. http://statspage.com)"),g_szDBLink,charsmax(g_szDBLink));

	RegisterHookChain(RG_CBasePlayer_GetIntoGame,"HOOK_CBasePlayer_GetIntoGame",true);

	DisableHookChain(g_hCSGameRules_PlayerKilled = RegisterHookChain(RG_CSGameRules_PlayerKilled,"HOOK_CSGameRules_PlayerKilled",true));

	DisableHookChain(g_hCBasePlayer_TakeDamage = RegisterHookChain(RG_CBasePlayer_TakeDamage,"HOOK_CBasePlayer_TakeDamage",true));

	DisableHookChain(g_hCBasePlayer_SetAnimation = RegisterHookChain(RG_CBasePlayer_SetAnimation,"HOOK_CBasePlayer_SetAnimation",true));

	DisableHookChain(h_hCSGameRules_OnRoundFreezeEnd = RegisterHookChain(RG_CSGameRules_OnRoundFreezeEnd,"HOOK_CSGameRules_OnRoundFreezeEnd",true));

	DisableHookChain(g_hRoundEnd = RegisterHookChain(RG_RoundEnd,"HOOK_RoundEnd",true));

	DisableHookChain(g_hPlantBomb = RegisterHookChain(RG_PlantBomb,"HOOK_PlantBomb",true));

	DisableHookChain(g_hCGrenade_DefuseBombEnd = RegisterHookChain(RG_CGrenade_DefuseBombEnd,"HOOK_CGrenade_DefuseBombEnd",true));

	DisableHookChain(g_hCGrenade_ExplodeBomb = RegisterHookChain(RG_CGrenade_ExplodeBomb,"HOOK_CGrenade_ExplodeBomb",true));

	DisableHookChain(g_hCBasePlayer_AddAccount = RegisterHookChain(RG_CBasePlayer_AddAccount,"HOOK_CBasePlayer_AddAccount",true));

	PUG_RegCommand("rank","PUG_Rank",ADMIN_ALL,"PUG_DESC_RANK");
	PUG_RegCommand("stats","PUG_Stats",ADMIN_ALL,"PUG_DESC_STATS");
}

public OnConfigsExecuted()
{
	g_hDbTuple = SQL_MakeDbTuple(g_szDBHost,g_szDBUser,g_szDBPass,g_szDBName);
}

public plugin_end()
{
	if(g_hDbTuple)
	{
		SQL_FreeHandle(g_hDbTuple);
	}
}

public HOOK_CBasePlayer_GetIntoGame(id)
{	
	arrayset(g_iStats[id],0,sizeof(g_iStats[]));

	for(new WeaponIdType:iWeapon;iWeapon <= WEAPON_P90;iWeapon++)
	{
		arrayset(g_iStatsWeapon[id][iWeapon],0,sizeof(g_iStatsWeapon[]));
	}

	g_iRoundKill[id] = 0;
	
	g_iRoundVersus[id] = 0;

	if(g_hDbTuple)
	{
		SQL_SavePlayer(id);
	}
	else
	{
		set_task(5.0,"SQL_SavePlayer",id);
	}
}

public client_disconnected(id)
{
	SQL_SaveStats(id);
}

public PUG_Event(iState)
{
	if(iState == STATE_FIRST_HALF || iState == STATE_SECOND_HALF || iState == STATE_OVERTIME)
	{
		EnableHookChain(g_hCSGameRules_PlayerKilled);

		EnableHookChain(g_hCBasePlayer_TakeDamage);

		EnableHookChain(g_hCBasePlayer_SetAnimation);

		EnableHookChain(h_hCSGameRules_OnRoundFreezeEnd);

		EnableHookChain(g_hRoundEnd);

		EnableHookChain(g_hPlantBomb);

		EnableHookChain(g_hCGrenade_DefuseBombEnd);

		EnableHookChain(g_hCGrenade_ExplodeBomb);

		EnableHookChain(g_hCBasePlayer_AddAccount);
	}
	else
	{
		DisableHookChain(g_hCSGameRules_PlayerKilled);

		DisableHookChain(g_hCBasePlayer_TakeDamage);

		DisableHookChain(g_hCBasePlayer_SetAnimation);

		EnableHookChain(h_hCSGameRules_OnRoundFreezeEnd);

		DisableHookChain(g_hRoundEnd);

		DisableHookChain(g_hPlantBomb);

		DisableHookChain(g_hCGrenade_DefuseBombEnd);

		DisableHookChain(g_hCGrenade_ExplodeBomb);

		DisableHookChain(g_hCBasePlayer_AddAccount);
	}

	if(iState == STATE_FIRST_HALF || iState == STATE_HALFTIME || iState == STATE_END)
	{
		SQL_SaveGame();
	}
}

public PUG_Rank(id)
{
	if(TEAM_TERRORIST <= get_member_s(id,m_iTeam) <= TEAM_CT)
	{
		show_motd(id,g_szDBLink,fmt("%L",LANG_SERVER,"PUG_RANK_TITLE"));
	}

	return PLUGIN_HANDLED;
}

public PUG_Stats(id)
{
	if(TEAM_TERRORIST <= get_member_s(id,m_iTeam) <= TEAM_CT)
	{
		new szParam[MAX_AUTHID_LENGTH];
		read_argv(1,szParam,charsmax(szParam));
		
		new iPlayer = cmd_target(id,szParam,CMDTARGET_OBEY_IMMUNITY);
		
		if(iPlayer)
		{
			get_user_authid(iPlayer,szParam,charsmax(szParam));

			if(is_user_bot(iPlayer))
			{
				format(szParam,charsmax(szParam),"%s_%n",szParam,iPlayer);
			}
		}
		else
		{
			get_user_authid(id,szParam,charsmax(szParam));
		}

		show_motd(id,fmt("%s/player.php?auth=%s",g_szDBLink,szParam),fmt("%L",LANG_SERVER,"PUG_STATS_TITLE"));
	}

	return PLUGIN_HANDLED;
}

public HOOK_CSGameRules_PlayerKilled(const Victim,const Killer,const Inflictor)
{
	if((Victim != Killer) && is_user_connected(Victim) && is_user_connected(Killer))
	{
		g_iStats[Killer][Kill]++;

		g_iStats[Victim][Death]++;

		g_iRoundKill[Killer]++;

		new bool:bHeadshotKilled = get_member_s(Victim,m_bHeadshotKilled);

		if(bHeadshotKilled)
		{
			g_iStats[Killer][Headshot]++;
		}

		new iActiveItem = get_member_s(Killer,m_pActiveItem);

		if(!is_nullent(iActiveItem))
		{
			new WeaponIdType:iWeapon = get_member_s(iActiveItem,m_iId);

			g_iStatsWeapon[Killer][iWeapon][WEAPON_KILL]++;

			g_iStatsWeapon[Victim][iWeapon][WEAPON_DEATH]++;

			if(bHeadshotKilled)
			{
				g_iStatsWeapon[Killer][iWeapon][WEAPON_HEADSHOT]++;
			}
		}

		new szTeam[12] = {0};
		
		new iPlayers[MAX_PLAYERS],iNum;

		for(new iPlayer = 1;iPlayer <= MaxClients;iPlayer++)
		{
			if((g_iRoundDamage[iPlayer][Victim] > PUG_STATS_ASSISTANCE_DMG) && (iPlayer != Killer))
			{
				g_iStats[iPlayer][Assist]++;
			}
			
			if(!g_iRoundVersus[iPlayer] && is_user_alive(iPlayer))
			{
				if(1 <= get_user_team(iPlayer,szTeam,charsmax(szTeam)) <= 2)
				{
					get_players(iPlayers,iNum,"ae",szTeam);
					
					if(iNum == 1)
					{
						get_players(iPlayers,iNum,"ae",(szTeam[0] == 'T') ? "CT" : "TERRORIST");
						
						if(iNum)
						{
							g_iRoundVersus[iPlayer] = iNum;
						}
					}
				}
			}
		}
	}
}

public HOOK_CBasePlayer_TakeDamage(This,pevInflictor,pevAttacker,Float:fDamage)
{
	if((1 <= pevAttacker <= MaxClients) && (This != pevAttacker) && rg_is_player_can_takedamage(This,pevAttacker))
	{
		new iDamage = get_member_s(This,m_lastDamageAmount);

		g_iStats[pevAttacker][Hit]++;
		
		g_iStats[pevAttacker][Damage] += iDamage;

		g_iRoundDamage[pevAttacker][This] += iDamage;

		g_iRoundDamageSelf[pevAttacker] += iDamage;

		new TeamName:iTeam = get_member_s(pevAttacker,m_iTeam);

		g_iRoundDamageTeam[iTeam] += iDamage;

		new iActiveItem = get_member_s(pevAttacker,m_pActiveItem);

		if(!is_nullent(iActiveItem))
		{
			new WeaponIdType:iWeapon = get_member_s(iActiveItem,m_iId);

			g_iStatsWeapon[pevAttacker][iWeapon][WEAPON_HIT]++;

			g_iStatsWeapon[pevAttacker][iWeapon][WEAPON_DAMAGE] += iDamage;
		}
	}
}

public HOOK_CBasePlayer_SetAnimation(const This,PLAYER_ANIM:PlayerAnim)
{
	if((PlayerAnim == PLAYER_ATTACK1) && is_user_connected(This))
	{
		new iActiveItem = get_member_s(This,m_pActiveItem);

		if(!is_nullent(iActiveItem))
		{
			if(1 <= rg_get_iteminfo(iActiveItem,ItemInfo_iSlot) <= 2)
			{
				g_iStats[This][Shot]++;

				new WeaponIdType:iWeapon = get_member_s(iActiveItem,m_iId);

				g_iStatsWeapon[This][iWeapon][WEAPON_SHOT]++;
			}
		}
	}
}

public HOOK_CSGameRules_OnRoundFreezeEnd()
{
	for(new iPlayer = 1;iPlayer <= MaxClients;iPlayer++)
	{
		arrayset(g_iRoundDamage[iPlayer],0,sizeof(g_iRoundDamage[]));
	}

	g_iRoundBombPlanter = -1;

	g_iRoundBombDefuser = -1;

	arrayset(g_iRoundKill,0,sizeof(g_iRoundKill));

	arrayset(g_iRoundVersus,0,sizeof(g_iRoundVersus));

	arrayset(g_iRoundDamageSelf,0,sizeof(g_iRoundDamageSelf));

	g_iRoundDamageTeam[TEAM_CT] = 0;

	g_iRoundDamageTeam[TEAM_TERRORIST] = 0;
}

public HOOK_RoundEnd(WinStatus:Status,ScenarioEventEndRound:Event,Float:tmDelay)
{
	if(WINSTATUS_CTS <= Status <= WINSTATUS_TERRORISTS)
	{
		new TeamName:iWinner = (Status == WINSTATUS_TERRORISTS) ? TEAM_TERRORIST : TEAM_CT;

		new TeamName:iTeam = TEAM_UNASSIGNED;

		if(Event == ROUND_TARGET_BOMB)
		{
			if(is_user_connected(g_iRoundBombPlanter))
			{
				g_iStats[g_iRoundBombPlanter][RoundWinShare] += PUG_STATS_RWS_C4_DEFUSED;
			}
		}
		else if(Event == ROUND_BOMB_DEFUSED)
		{
			if(is_user_connected(g_iRoundBombDefuser))
			{
				g_iStats[g_iRoundBombDefuser][g_iRoundBombDefuser] += PUG_STATS_RWS_C4_EXPLODE;
			}
		}
		
		new iPlayers[MAX_PLAYERS],iNum;
		get_players(iPlayers,iNum,"h");
		
		new iPlayer;

		new Float:fRoundWinShare = 0.0;

		for(new i = 0;i < iNum;i++)
		{
			iPlayer = iPlayers[i];
			
			g_iStats[iPlayer][RoundPlay]++;

			if(g_iRoundKill[iPlayer] > 0)
			{
				g_iStats[iPlayer][Streak][g_iRoundKill[iPlayer]]++;
			}
			
			iTeam = get_member_s(iPlayer,m_iTeam);

			if(TEAM_TERRORIST <= iTeam <= TEAM_CT)
			{
				if(iWinner == iTeam)
				{
					g_iStats[iPlayer][RoundWin]++;
					
					if(g_iRoundVersus[iPlayer] > 0)
					{
						g_iStats[iPlayer][Versus][g_iRoundVersus[iPlayer]]++;
					}
					
					if(g_iRoundDamageSelf[iPlayer])
					{
						fRoundWinShare = floatdiv(Float:g_iRoundDamageSelf[iPlayer],Float:g_iRoundDamageTeam[iWinner]);

						if(Event == ROUND_BOMB_DEFUSED || Event == ROUND_TARGET_BOMB)
						{
							fRoundWinShare = (PUG_STATS_RWS_MAP_TARGET * fRoundWinShare);
						}

						g_iStats[iPlayer][RoundWinShare] += fRoundWinShare;
					}
				}
				else
				{
					g_iStats[iPlayer][RoundLose]++;
				}
			}	
		}
	}
}

public HOOK_PlantBomb(const iPlayer)
{
	g_iRoundBombPlanter = iPlayer;
	
	g_iStats[iPlayer][BombPlant]++;
}

public HOOK_CGrenade_DefuseBombEnd(const This,const iPlayer,bool:bDefused)
{
	if(bDefused)
	{
		g_iRoundBombDefuser = iPlayer;

		g_iStats[iPlayer][BombDefuse]++;
	}
}

public HOOK_CGrenade_ExplodeBomb(const This,TraceHandle,const bitsDamageType)
{
	if(is_user_connected(g_iRoundBombPlanter))
	{
		g_iStats[g_iRoundBombPlanter][BombExplode]++;
	}
}

public HOOK_CBasePlayer_AddAccount(id,iAmount,RewardType:Type)
{
	if(Type == RT_PLAYER_BOUGHT_SOMETHING)
	{
		g_iStats[id][Money] += abs(iAmount);
	}
}

public SQL_SavePlayer(id)
{
	if(g_hDbTuple && is_user_connected(id))
	{
		new szData[3] = {SQL_SAVE_USER};
		
		szData[0] = SQL_SAVE_USER;
		szData[1] = id;
		
		new szAuth[MAX_AUTHID_LENGTH];
		get_user_authid(id,szAuth,charsmax(szAuth));

		if(is_user_bot(id))
		{
			format(szAuth,charsmax(szAuth),"%s_%n",szAuth,id);
		}

		new szQuery[512] = {0};

		formatex
		(
			szQuery,
			charsmax(szQuery),
			"INSERT INTO player (auth, name) VALUES ('%s', '%n') ON DUPLICATE KEY UPDATE id = LAST_INSERT_ID(id), name = '%n'; SELECT LAST_INSERT_ID() AS id;",
			szAuth,
			id,
			id,
			szAuth
		);
		
		SQL_ThreadQuery
		(
			g_hDbTuple,
			"SQL_Handler",
			szQuery,
			szData,
			sizeof(szData)
		);
	}
}

SQL_SaveGame()
{
	if(g_hDbTuple)
	{
		new szData[2] = {SQL_SAVE_GAME};
		
		new szQuery[512] = {0};
		
		if(!g_iGameIndex)
		{
			new szName[MAX_NAME_LENGTH];
			get_user_name(0,szName,charsmax(szName));
			
			new szIP[MAX_IP_WITH_PORT_LENGTH];
			get_user_ip(0,szIP,charsmax(szIP));
			
			formatex
			(
				szQuery,
				charsmax(szQuery),
				"INSERT INTO game VALUES (null, '%s', '%s', '%s', 0, 0, 0, 0, NOW(), 1);",
				szName,
				MapName,
				szIP
			);	
		}
		else
		{
			new iScoreTR = PUG_GetScore(TEAM_TERRORIST);
			new iScoreCT = PUG_GetScore(TEAM_CT);
			new iTeamWin = 0;

			if(iScoreTR != iScoreCT)
			{
				iTeamWin = (iScoreTR > iScoreCT) ? 1 : 2;
			}

			formatex
			(
				szQuery,
				charsmax(szQuery),
				"UPDATE game SET rounds = %d, score1 = %d, score2 = %d, winner = %d WHERE id = %d;",
				PUG_GetRound(),
				iScoreTR,
				iScoreCT,
				iTeamWin,
				g_iGameIndex
			);
		}
		
		SQL_ThreadQuery(g_hDbTuple,"SQL_Handler",szQuery,szData,sizeof(szData));
	}
}

SQL_SaveStats(id)
{
	if(g_hDbTuple)
	{
		if(g_iStats[id][Index] && g_iStats[id][RoundPlay])
		{
			new szData[3] = {SQL_SAVE_STAT};

			szData[0] = SQL_SAVE_STAT;
			szData[1] = id;

			new szQuery[2048] = {};

			formatex
			(
				szQuery,
				charsmax(szQuery),
				"INSERT INTO playerStats VALUES (null, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %f, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, NOW(), 1)",

				g_iGameIndex,
				g_iStats[id][Index],
				get_member_s(id,m_iTeam),
				
				g_iStats[id][Kill],
				g_iStats[id][Assist],
				g_iStats[id][Death],
				g_iStats[id][Headshot],
				g_iStats[id][Shot],
				g_iStats[id][Hit],
				g_iStats[id][Damage],
				g_iStats[id][Money],

				g_iStats[id][RoundPlay],
				g_iStats[id][RoundLose],
				g_iStats[id][RoundWin],
				g_iStats[id][RoundWinShare],
				
				g_iStats[id][BombPlant],
				g_iStats[id][BombDefuse],
				g_iStats[id][BombExplode],
				
				g_iStats[id][Streak][1],
				g_iStats[id][Streak][2],
				g_iStats[id][Streak][3],
				g_iStats[id][Streak][4],
				g_iStats[id][Streak][5],
				
				g_iStats[id][Versus][1],
				g_iStats[id][Versus][2],
				g_iStats[id][Versus][3],
				g_iStats[id][Versus][4],
				g_iStats[id][Versus][5]
			);

			for(new WeaponIdType:iWeapon;iWeapon <= WEAPON_P90;iWeapon++)
			{
				if(g_iStatsWeapon[id][iWeapon][WEAPON_HIT])
				{
					format
					(
						szQuery,
						charsmax(szQuery),
						"%s; INSERT INTO weaponStats VALUES (null, %d, %d, %d, %d, %d, %d, %d, %d, %d, NOW(), 1)",

						szQuery,

						g_iGameIndex,
						g_iStats[id][Index],

						iWeapon,

						g_iStatsWeapon[id][iWeapon][WEAPON_KILL],
						g_iStatsWeapon[id][iWeapon][WEAPON_DEATH],
						g_iStatsWeapon[id][iWeapon][WEAPON_HEADSHOT],
						g_iStatsWeapon[id][iWeapon][WEAPON_SHOT],
						g_iStatsWeapon[id][iWeapon][WEAPON_HIT],
						g_iStatsWeapon[id][iWeapon][WEAPON_DAMAGE]
					);
				}
			}
			
			SQL_ThreadQuery(g_hDbTuple,"SQL_Handler",szQuery,szData,sizeof(szData));
		}
	}
}

public SQL_Handler(iFailState,Handle:hQuery,szError[],iErrorNum,szData[],iDataSize,Float:fQueueTime)
{
	if(iFailState == TQUERY_SUCCESS)
	{
		switch(szData[0])
		{
			case SQL_SAVE_USER:
			{
				new iPlayerIndex = SQL_GetInsertId(hQuery);

				if(iPlayerIndex && is_user_connected(szData[1]))
				{
					g_iStats[szData[1]][Index] = iPlayerIndex;
				}
			}
			case SQL_SAVE_GAME:
			{
				new iGameIndex = SQL_GetInsertId(hQuery);

				if(iGameIndex)
				{
					g_iGameIndex = iGameIndex;
				}
			}
		}
	}
	
	if(hQuery != Empty_Handle)
	{
		SQL_FreeHandle(hQuery);
	}
}