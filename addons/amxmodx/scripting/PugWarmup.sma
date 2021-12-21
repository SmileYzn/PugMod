#include <PugCore>
#include <PugCS>

new HookChain:g_hCBasePlayer_AddAccount;
new HookChain:g_hCBasePlayer_HasRestrictItem;

public plugin_init()
{
	register_plugin("Pug Mod (Warmup)",PUG_MOD_VERSION,PUG_MOD_AUTHOR);
	
	DisableHookChain(g_hCBasePlayer_AddAccount = RegisterHookChain(RG_CBasePlayer_AddAccount,"HOOK_CBasePlayer_AddAccount",false));
	
	DisableHookChain(g_hCBasePlayer_HasRestrictItem = RegisterHookChain(RG_CBasePlayer_HasRestrictItem,"HOOK_CBasePlayer_HasRestrictItem",false));
}

public PUG_Event(iState)
{
	if(iState == STATE_WARMUP || iState == STATE_HALFTIME)
	{
		EnableHookChain(g_hCBasePlayer_AddAccount);
		
		EnableHookChain(g_hCBasePlayer_HasRestrictItem);
	}
	else
	{
		DisableHookChain(g_hCBasePlayer_AddAccount);
		
		DisableHookChain(g_hCBasePlayer_HasRestrictItem);
	}
}

public HOOK_CBasePlayer_AddAccount(id,iAmount,RewardType:Type)
{
	if(Type == RT_PLAYER_BOUGHT_SOMETHING)
	{
		SetHookChainArg(2,ATYPE_INTEGER,0);
	}
	
	return HC_CONTINUE;
}

public HOOK_CBasePlayer_HasRestrictItem(id,ItemID:Item,ItemRestType:Type)
{
	if(Type == ITEM_TYPE_BUYING)
	{
		if(Item == ITEM_SHIELDGUN || Item == ITEM_HEGRENADE || Item == ITEM_FLASHBANG || Item == ITEM_SMOKEGRENADE)
		{
			client_print(id,print_center,"#Cstrike_TitlesTXT_Weapon_Not_Available");
			
			SetHookChainReturn(ATYPE_BOOL,true);
			
			return HC_SUPERCEDE;
		}
	}
	
	return HC_CONTINUE;
}
