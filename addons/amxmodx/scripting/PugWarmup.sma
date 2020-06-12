#include <PugCore>
#include <PugStocks>
#include <PugCS>

new bool:g_bWarmup;

new g_iMsgMoney;
new g_iMsgHideWeapon;

new HamHook:g_hHamKilled;
new HamHook:g_hHamSpawn;

new g_iFMSetModel;
new g_iFMCmdStart;

#define HUD_HIDE_TIMER (1<<4)
#define HUD_HIDE_MONEY (1<<5)

public plugin_init()
{
	register_plugin("Pug Mod (Warmup)",PUG_MOD_VERSION,PUG_MOD_AUTHOR);
	
	DisableHamForward(g_hHamKilled = RegisterHamPlayer(Ham_Killed,"HOOK_HamKilled",true));
	DisableHamForward(g_hHamSpawn = RegisterHamPlayer(Ham_Spawn,"HOOK_HamSpawn",true));
	
	register_clcmd("joinclass","HOOK_JoinedClass");
	register_clcmd("menuselect","HOOK_JoinedClass");
}

public PUG_Event(iState)
{
	g_bWarmup = (iState == STATE_WARMUP || iState == STATE_HALFTIME);
	
	if(g_bWarmup)
	{
		g_iMsgMoney = register_message(get_user_msgid("Money"),"HOOK_MsgMoney");
		g_iMsgHideWeapon = register_message(get_user_msgid("HideWeapon"),"HOOK_MsgHideWeapon");
		
		EnableHamForward(g_hHamKilled);
		EnableHamForward(g_hHamSpawn);
		
		g_iFMSetModel = register_forward(FM_SetModel,"HOOK_SetModel",true);
		g_iFMCmdStart = register_forward(FM_CmdStart,"HOOK_CmdStart",true);
	}
	else
	{
		unregister_message(get_user_msgid("Money"),g_iMsgMoney);
		unregister_message(get_user_msgid("HideWeapon"),g_iMsgHideWeapon);
		
		DisableHamForward(g_hHamKilled);
		DisableHamForward(g_hHamSpawn);
		
		unregister_forward(FM_SetModel,g_iFMSetModel,true);
		unregister_forward(FM_CmdStart,g_iFMCmdStart,true);
	}
	
	PUG_SetMapObjectives(g_bWarmup);
}

public CS_OnBuy(id,Weapon)
{
	if(g_bWarmup)
	{
		if((Weapon == CSI_SHIELD || Weapon == CSI_SHIELDGUN) || (cs_get_weapon_class(Weapon) == CS_WEAPONCLASS_GRENADE))
		{
			return PLUGIN_HANDLED;
		}
	}

	return PLUGIN_CONTINUE;
}

public HOOK_MsgMoney(iMsg,iDest,id)
{
	if(g_bWarmup)
	{
		if(is_user_alive(id))
		{
			cs_set_user_money(id,16000,0);
		}

		return PLUGIN_HANDLED;
	}

	return PLUGIN_CONTINUE;
}

public HOOK_MsgHideWeapon()
{
	if(g_bWarmup)
	{
		set_msg_arg_int(1,ARG_BYTE,get_msg_arg_int(1)|HUD_HIDE_TIMER|HUD_HIDE_MONEY);
	}
}

public HOOK_HamKilled(id)
{
	if(g_bWarmup)
	{
		set_task(0.75,"PUG_Respawn",id);
	}
}

public PUG_Respawn(id)
{
	if(g_bWarmup)
	{
		if(is_user_connected(id))
		{
			if(!is_user_alive(id) && PUG_PLAYER_IN_TEAM(id))
			{
				ExecuteHamB(Ham_CS_RoundRespawn,id);
			}
		}
	}
}

public HOOK_HamSpawn(id)
{
	if(g_bWarmup)
	{
		if(is_user_alive(id))
		{
			set_pev(id,pev_takedamage,DAMAGE_NO);
		}		
	}
}

public HOOK_CmdStart(id,hHandle)
{
	if(g_bWarmup)
	{
		if(is_user_alive(id))
		{
			if(get_uc(hHandle,UC_Buttons) & (IN_FORWARD|IN_BACK|IN_MOVELEFT|IN_MOVERIGHT|IN_ATTACK|IN_ATTACK2))
			{
				if(pev(id,pev_takedamage) == DAMAGE_NO)
				{
					set_pev(id,pev_takedamage,DAMAGE_AIM);
				}
			}
		}
	}
}

public HOOK_SetModel(iEntity)
{
	if(g_bWarmup)
	{
		if(pev_valid(iEntity))
		{
			new szName[MAX_NAME_LENGTH];
			pev(iEntity,pev_classname,szName,charsmax(szName));
			
			if(equali(szName,"weaponbox"))
			{
				set_pev(iEntity,pev_effects,EF_NODRAW);
				set_pev(iEntity,pev_nextthink,(get_gametime() + 0.1));
			}
			else if(equali(szName,"weapon_shield"))
			{
				set_pev(iEntity,pev_effects,EF_NODRAW);
				RequestFrame("PUG_ThinkEntity",iEntity);
			}
		}
	}
}

public PUG_ThinkEntity(iEntity)
{
	if(pev_valid(iEntity))
	{
		dllfunc(DLLFunc_Think,iEntity);
	}
}

public HOOK_JoinedClass(id)
{
	if(g_bWarmup)
	{		
		if(get_ent_data(id,"CBasePlayer","m_iMenu") == CS_Menu_ChooseAppearance)
		{
			set_task(0.75,"PUG_Respawn",id);
		}
	}
}
