#include <PugCore>

enum _:ePlayerStats
{
	Kill,					// Test
	Assist,					// Test
	Death,					// Test
	Headshot,				// Test
	Shot,					// Test
	Hit,					// Test
	Damage,					// Test
	Float:WinShare, 		// Create

	RoundPlay, 				// Test
	RoundLose, 				// Test
	RoundWin, 				// Test

	BombPlant, 				// Test
	BombDefuse, 			// Test
	BombExplode, 			// Test

	Streak[MAX_PLAYERS],	// Test
	Versus[MAX_PLAYERS],	// Create
};

new g_Stats[MAX_PLAYERS+1][ePlayerStats];

new g_iRoundKill[MAX_PLAYERS+1];
new g_iRoundDamage[MAX_PLAYERS+1][MAX_PLAYERS+1];

new HookChain:g_hCSGameRules_PlayerKilled;
new HookChain:g_hCBasePlayer_TakeDamage;
new HookChain:g_hCBasePlayer_SetAnimation;
new HookChain:h_hCSGameRules_OnRoundFreezeEnd;
new HookChain:g_hRoundEnd;
new HookChain:g_hPlantBomb;
new HookChain:g_hCGrenade_DefuseBombEnd;
new HookChain:g_hCGrenade_ExplodeBomb;

public plugin_init()
{
	register_plugin("Pug Mod (Ranking)",PUG_MOD_VERSION,PUG_MOD_AUTHOR);

	DisableHookChain(g_hCSGameRules_PlayerKilled = RegisterHookChain(RG_CSGameRules_PlayerKilled,"HOOK_CSGameRules_PlayerKilled",true));

	DisableHookChain(g_hCBasePlayer_TakeDamage = RegisterHookChain(RG_CBasePlayer_TakeDamage,"HOOK_CBasePlayer_TakeDamage",true));

	DisableHookChain(g_hCBasePlayer_SetAnimation = RegisterHookChain(RG_CBasePlayer_SetAnimation,"HOOK_CBasePlayer_SetAnimation",true));

	DisableHookChain(h_hCSGameRules_OnRoundFreezeEnd = RegisterHookChain(RG_CSGameRules_OnRoundFreezeEnd,"HOOK_CSGameRules_OnRoundFreezeEnd",true));

	DisableHookChain(g_hRoundEnd = RegisterHookChain(RG_RoundEnd,"HOOK_RoundEnd",true));

	DisableHookChain(g_hPlantBomb = RegisterHookChain(RG_PlantBomb,"HOOK_PlantBomb",true));

	DisableHookChain(g_hCGrenade_DefuseBombEnd = RegisterHookChain(RG_CGrenade_DefuseBombEnd,"HOOK_CGrenade_DefuseBombEnd",true));

	DisableHookChain(g_hCGrenade_ExplodeBomb = RegisterHookChain(RG_CGrenade_ExplodeBomb,"HOOK_CGrenade_ExplodeBomb",true));
}

public client_putinserver(id)
{
	arrayset(g_Stats[id],0,sizeof(g_Stats[]));

	arrayset(g_iRoundKill[id],0,sizeof(g_iRoundKill[]));
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
	}
}

public HOOK_CSGameRules_PlayerKilled(const Victim,const Killer,const Inflictor)
{
	if((Victim != Killer) && is_user_connected(Victim) && is_user_connected(Killer))
	{
		g_Stats[Killer][Kill]++;

		g_Stats[Victim][Death]++;

		g_iRoundKill[Killer]++;

		if(get_member(Victim,m_bHeadshotKilled))
		{
			g_Stats[Killer][Headshot]++;
		}

		for(new iPlayer = 1;iPlayer <= MaxClients;iPlayer++)
		{
			if((g_iRoundDamage[iPlayer][Victim] > 50) && (iPlayer != Killer))
			{
				g_Stats[iPlayer][Assist]++;
			}
		}
	}
}

public HOOK_CBasePlayer_TakeDamage(This,pevInflictor,pevAttacker,Float:fDamage)
{
	if((1 <= pevAttacker <= MaxClients) && (This != pevAttacker) && rg_is_player_can_takedamage(This,pevAttacker))
	{
		new iDamage = get_member_s(This,m_lastDamageAmount);

		g_Stats[pevAttacker][Hit]++;
		
		g_Stats[pevAttacker][Damage] += iDamage;

		g_iRoundDamage[pevAttacker][This] += iDamage;
	}
}

public HOOK_CBasePlayer_SetAnimation(const This,PLAYER_ANIM:PlayerAnim)
{
	if((PlayerAnim == PLAYER_ATTACK1) && is_user_connected(This))
	{
		new iActiveItem = get_member(This,m_pActiveItem);

		if(is_entity(iActiveItem))
		{
			if(1 <= rg_get_iteminfo(iActiveItem,ItemInfo_iSlot) <= 2)
			{
				g_Stats[This][Shot]++;
			}
		}
	}
}

public HOOK_CSGameRules_OnRoundFreezeEnd()
{
	for(new iPlayer = 1;iPlayer <= MaxClients;iPlayer++)
	{
		g_iRoundKill[iPlayer] = 0;

		arrayset(g_iRoundDamage[iPlayer],0,sizeof(g_iRoundDamage[]));
	}
}

public HOOK_RoundEnd(WinStatus:Status,ScenarioEventEndRound:Event,Float:tmDelay)
{
	if(Status != WINSTATUS_NONE && Status != WINSTATUS_DRAW)
	{
		new iPlayers[MAX_PLAYERS],iNum;

		get_players(iPlayers,iNum,"h");

		new iPlayer;

		new TeamName:iWinner = (Status == WINSTATUS_TERRORISTS) ? TEAM_TERRORIST : TEAM_CT;

		new TeamName:iTeam = TEAM_UNASSIGNED;

		for(new i;i < iNum;i++)
		{
			iPlayer = iPlayers[i];

			g_Stats[iPlayer][RoundPlay]++;

			g_Stats[iPlayer][Streak][g_iRoundKill[iPlayer]]++;

			iTeam = get_member_s(iPlayer,m_iTeam);

			if(TEAM_TERRORIST <= iTeam <= TEAM_CT)
			{
				if(iWinner == iTeam)
				{
					g_Stats[iPlayer][RoundWin]++;
				}
				else
				{
					g_Stats[iPlayer][RoundLose]++;
				}
			}
		}
	}
}

public HOOK_PlantBomb(const Player)
{
	g_Stats[Player][BombPlant]++;
}

public HOOK_CGrenade_DefuseBombEnd(const This,const Player,bool:bDefused)
{
	if(bDefused)
	{
		g_Stats[Player][BombDefuse]++;
	}
}

public HOOK_CGrenade_ExplodeBomb(const This)
{
	new Player = get_entvar(This,var_owner);

	if(is_user_connected(Player))
	{
		g_Stats[Player][BombExplode]++;
	}
}
